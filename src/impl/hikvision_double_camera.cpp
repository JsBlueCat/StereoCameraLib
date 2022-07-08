#include "hikvision_double_camera.h"
#include "config.h"
#include "stereo_clib.h"
#include <filesystem>
#include <future>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>

std::shared_ptr<StereoCamera> StereoCamera::instance{nullptr};
ErrorInfo StereoCamera::cam_st{ErrorInfo::Success};

std::shared_ptr<StereoCamera> StereoCamera::GetInstance() {
  std::cout << "get instance" << std::endl;
  if (instance == nullptr) {
    instance.reset(new StereoCamera());
    return instance;
  } else {
    return instance;
  }
}

ErrorInfo StereoCamera::QueryCamera(int id) {
  switch (id) {
  case /* constant-expression */ 1:
    return left_camera.m_hDevHandle != NULL ? ErrorInfo::Success : ErrorInfo::CamNotConnected;
    break;

  case /* constant-expression */ 2:
    return right_camera.m_hDevHandle != NULL ? ErrorInfo::Success : ErrorInfo::CamNotConnected;
    break;

  default:
    return ErrorInfo::Unknown;
    break;
  }
}

StereoCamera::StereoCamera() {
  std::cout << "find the connected device" << std::endl;
  cam_st = ErrorInfo::CamNotFound;
  memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
  auto times = 1;
  do {
    int nRet = left_camera.EnumDevices(&stDeviceList);
    if (MV_OK != nRet) {
      std::cerr << "find camera failed! code ：nRet [0x" << nRet << "]"
                << std::endl;
      // perror("101");
      return;
    } else {
      if (stDeviceList.nDeviceNum > 0) { // find device
        for (auto pDeviceInfo : stDeviceList.pDeviceInfo) {
          if (pDeviceInfo != nullptr &&
              pDeviceInfo->nTLayerType == MV_GIGE_DEVICE) {
            int nIp4 =
                (pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
            camera_ip_map[nIp4] = *pDeviceInfo;
          }
        }
        auto left = false, right = false; // init
        for (auto iter = camera_ip_map.begin(); iter != camera_ip_map.end();
             iter++) {
          if (iter->first == LEFT_CAMERA_IP4)
            left = true;
          if (iter->first == RIGHT_CAMERA_IP4)
            right = true;
        }
        if (!left) {
          std::cerr << "can not find left camera , reconnecting..."
                    << std::endl;
        }
        if (!right) {
          std::cerr << "can not find right camera , reconnecting..."
                    << std::endl;
        }
        if (left && right) {
          std::cout << "connect left and right camera done!" << std::endl;
          break;
        }
      } else {
        std::cerr << "can not find camera , please check..." << std::endl;
      }
    }
  } while (times++ < 20);
  if (times > 20) {
    std::cerr << "fail with connect 20 times , abort..." << std::endl;
  } else {
    cam_st = ErrorInfo::Success;
    int leftStatus = left_camera.Open(&camera_ip_map[LEFT_CAMERA_IP4]);
    int rightStatus = right_camera.Open(&camera_ip_map[RIGHT_CAMERA_IP4]);
    if (leftStatus != MV_OK || rightStatus !=MV_OK){
      cam_st = ErrorInfo::CamNotConnected;
      return;
    }
    auto cam_init = [](CMyCamera &cam) {
      auto nPacketSize = cam.GetOptimalPacketSize();
      cam.SetIntValue("GevSCPSPacketSize", nPacketSize);
      cam.SetEnumValue("TriggerMode", MV_TRIGGER_MODE_OFF);
      cam.GetIntValue("PayloadSize", &cam.m_nBufSizeForDriver);
      cam.m_pBufForDriver = (unsigned char *)malloc(cam.m_nBufSizeForDriver);
      cam.StartGrabbing();
    };
    cam_init(left_camera);
    cam_init(right_camera);
  }
}

StereoCamera::~StereoCamera() {
  std::cout << "close left camera..." << std::endl;
  left_camera.StopGrabbing();
  left_camera.Close();
  std::cout << "close right camera..." << std::endl;
  right_camera.StopGrabbing();
  right_camera.Close();
}

ErrorInfo StereoCamera::GrabImageDoubleCamera() {
#ifndef CAMREA_DEBUG
  auto grab_img = [&](CMyCamera &cam, cv::Mat &out) -> ErrorInfo {
    MV_FRAME_OUT_INFO_EX stImageInfo = {0};
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    unsigned int pnDatalen = 0;
    auto result =
        cam.GetOneFrameTimeout(cam.m_pBufForDriver, &pnDatalen,
                               cam.m_nBufSizeForDriver, &stImageInfo, 200);
    if (result != MV_OK)
        return ErrorInfo::Cp3NotFound;
    Convert2Mat(&stImageInfo, cam.m_pBufForDriver, out);
    return ErrorInfo::Success;
  };
  auto err1 = grab_img(left_camera, leftImg);
  Assert_Error(err1);
  auto err2 = grab_img(right_camera, rightImg);
  Assert_Error(err2);
#endif
  time_t curr_time;
	curr_time = time(NULL);
    struct tm tm_local;
	localtime_s(&tm_local,&curr_time);
  std::string day = std::to_string(tm_local.tm_year + 1900) + "-"
    + std::to_string(tm_local.tm_mon+1) + "-" + std::to_string(tm_local.tm_mday);

  std::string time = std::to_string(tm_local.tm_hour) + "_"
    + std::to_string(tm_local.tm_min) + "_" + std::to_string(tm_local.tm_sec);

  auto const &config = Config::get_single();
  config.save_img((config.debug_images/day).string(),time+"_left.bmp",leftImg);
  config.save_img((config.debug_images/day).string(),time+"_right.bmp",rightImg);

  return ErrorInfo::Success;

  // auto grab1 = std::async(std::launch::async,grab_img,left_camera,leftImg);
  // auto grab2 = std::async(std::launch::async,grab_img,right_camera,rightImg);
  // grab1.wait();
  // grab2.wait();
}

ErrorInfo StereoCamera::GrabClibImg(int i) {
#ifndef CAMERA_DEBUG
  auto err = GrabImageDoubleCamera();
  Assert_Error(err);
  err = SaveGrabImg(i);
  Assert_Error(err);
#endif
  return ErrorInfo::Success;
}

ErrorInfo StereoCamera::SaveGrabImg(int i) {
  auto const &config = Config::get_single();
  cv::imwrite((config.images_path / (std::to_string(i) + "_left.bmp")).string(),
              leftImg);
  cv::imwrite(
      (config.images_path / (std::to_string(i) + "_right.bmp")).string(),
      rightImg);
  return ErrorInfo::Success;
}

ErrorInfo StereoCamera::RemapImg() {
  cv::Mat map11, map12, map21, map22;
  cv::Mat img1r, img2r;
  initUndistortRectifyMap(M1, D1, R1, P1, leftImg.size(), CV_16SC2, map11,
                          map12);
  initUndistortRectifyMap(M2, D2, R2, P2, leftImg.size(), CV_16SC2, map21,
                          map22);

  remap(leftImg, img1r, map11, map12, cv::INTER_LINEAR);
  remap(rightImg, img2r, map21, map22, cv::INTER_LINEAR);
  return ErrorInfo::Success;
}

ErrorInfo StereoCamera::SaveTestImg(int i) {
  auto const &config = Config::get_single();
  auto save_image = [](std::filesystem::path path, std::string &filename,
                       cv::Mat &image) {
    cv::imwrite((path / filename).string(), image);
  };
  save_image(config.debug_images, std::to_string(i) + "_left.bmp", leftImg);
  save_image(config.debug_images, std::to_string(i) + "_right.bmp", rightImg);

  auto save_split_img = [&]() {
    cv::Mat left, middle, right;
    auto left_left =
        cv::Rect(cv::Point(1715, 0), cv::Point(2135, leftImg.rows));
    auto left_middle =
        cv::Rect(cv::Point(2135, 0), cv::Point(4709, leftImg.rows));
    auto left_right =
        cv::Rect(cv::Point(4709, 0), cv::Point(5112, leftImg.rows));
    auto right_left = cv::Rect(cv::Point(227, 0), cv::Point(617, leftImg.rows));
    auto right_middle =
        cv::Rect(cv::Point(617, 0), cv::Point(3185, leftImg.rows));
    auto right_right =
        cv::Rect(cv::Point(3185, 0), cv::Point(3593, leftImg.rows));
    SplitImg(leftImg, left_left, left_middle, left_right, left, middle, right);
    save_image(config.debug_images, std::to_string(i) + "_left_left.bmp", left);
    save_image(config.debug_images, std::to_string(i) + "_left_middle.bmp",
               middle);
    save_image(config.debug_images, std::to_string(i) + "_left_right.bmp",
               right);
    SplitImg(rightImg, right_left, right_middle, right_right, left, middle,
             right);
    save_image(config.debug_images, std::to_string(i) + "_right_left.bmp",
               left);
    save_image(config.debug_images, std::to_string(i) + "_right_middle.bmp",
               middle);
    save_image(config.debug_images, std::to_string(i) + "_right_right.bmp",
               right);
  };
  save_split_img();
  return ErrorInfo::Success;
}

ErrorInfo StereoCamera::ClibCam() {
  auto isClibFile = [](std::filesystem::path path) -> bool {
    std::string pattern{"\\d+_\\w*.bmp"};
    std::regex re(pattern);
    return std::regex_match(path.string(), re);
  };
  auto const &config = Config::get_single();
  std::filesystem::directory_iterator list(config.images_path);
  std::vector<std::string> image_list;
  for (auto &it : list) {
    if (isClibFile(it.path().filename())) {
      image_list.push_back(it.path().string());
    }
  }
  return StereoCalibInerAndExter(image_list, config.boardSize, config.squareSize,
                          false, true, true);
                      
}

ErrorInfo StereoCamera::LoadParam() {
  LoadTransformParam(affine_R,affine_T);
  return LoadInerAndExterParam(M1, D1, M2, D2, R, T, R1, P1, R2, P2, Q);
  // LoadTransformParam(affine_R, affine_T);
}

ErrorInfo StereoCamera::MatchSingleFrame(int i, std::vector<cv::Mat> &results) {
  auto const &config = Config::get_single();
  auto left_left = cv::Rect(cv::Point(1715, 0), cv::Point(2135, leftImg.rows));
  auto left_middle =
      cv::Rect(cv::Point(2135, 0), cv::Point(4709, leftImg.rows));
  auto left_right = cv::Rect(cv::Point(4709, 0), cv::Point(5112, leftImg.rows));
  auto right_left = cv::Rect(cv::Point(227, 0), cv::Point(617, leftImg.rows));
  auto right_middle =
      cv::Rect(cv::Point(617, 0), cv::Point(3185, leftImg.rows));
  auto right_right =
      cv::Rect(cv::Point(3185, 0), cv::Point(3593, leftImg.rows));
  std::vector<std::vector<cv::Point2f>> left_points, right_points;
  auto split_img_and_find_point =
      [&](cv::Rect r1, cv::Rect r2, cv::Rect r3, cv::Mat &img,
          std::vector<std::vector<cv::Point2f>> &out) -> ErrorInfo{
    cv::Mat left, middle, right;
    SplitImg(img, r1, r2, r3, left, middle, right);
    auto corase_area = CoraselyFindCp3(middle);
    if(corase_area.second != ErrorInfo::Success) {
      return corase_area.second;
    }
    auto save_clip = [&]() {
      int count = 0;
      for (auto area : corase_area.first) {
        cv::Mat tmp = middle(area);
        cv::imwrite((config.debug_images /
                     ("current" + std::to_string(++count) + ".bmp"))
                        .string(),
                    tmp);
      }
    };
    save_clip();
    std::vector<std::vector<cv::Point2f>> points(corase_area.first.size());
    MutiFindCp3(middle, corase_area.first, points);
    MutiFixROIList(points, corase_area.first, r2.tl());
    out = points;
    return ErrorInfo::Success;
  };
  auto res_left = split_img_and_find_point(left_left, left_middle, left_right,
                                           leftImg, left_points);
  auto res_right = split_img_and_find_point(
      right_left, right_middle, right_right, rightImg, right_points);
  if (res_left == ErrorInfo::Success && res_right == ErrorInfo::Success) {
    MutiMatchCp3(left_points, right_points, Q, results);
  } else {
    std::cout << "Match MutiCp3 Error" << std::endl;
    return ErrorInfo::Cp3NotFound;
  }
  time_t curr_time;
	curr_time = time(NULL);
  struct tm tm_local;
	localtime_s(&tm_local,&curr_time);
  std::string day = std::to_string(tm_local.tm_year + 1900) + "-"
    + std::to_string(tm_local.tm_mon+1) + "-" + std::to_string(tm_local.tm_mday);
  std::string time = std::to_string(tm_local.tm_hour) + "_"
    + std::to_string(tm_local.tm_min) + "_" + std::to_string(tm_local.tm_sec);

  std::ofstream result_file;
  result_file.open(config.result_path/(day+".txt"), std::ios::app);
  for (auto &result : results) {
    cv::Mat new_mat;
    TransfromPoint(result(cv::Rect(0,0,1,3)),affine_R,affine_T,new_mat); // 转换参数空间
    result_file << "采集时间" <<time << "\n" << "cp3: x: " << new_mat.at<double>(0,0) << " ,y: " << new_mat.at<double>(1,0) << " ,z:" << new_mat.at<double>(2,0) << std::endl;
  }
  return ErrorInfo::Success;
}

// convert data stream in Mat format
bool Convert2Mat(MV_FRAME_OUT_INFO_EX *pstImageInfo, unsigned char *pData,
                 cv::Mat &srcImage) {
  // cv::Mat srcImage;
  if (pstImageInfo->enPixelType == PixelType_Gvsp_Mono8) {
    srcImage =
        cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC1, pData);
  } else if (pstImageInfo->enPixelType == PixelType_Gvsp_RGB8_Packed) {
    RGB2BGR(pData, pstImageInfo->nWidth, pstImageInfo->nHeight);
    srcImage =
        cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC3, pData);
  } else {
    printf("unsupported pixel format\n");
    return false;
  }

  if (NULL == srcImage.data) {
    return false;
  }

  return true;
}

int RGB2BGR(unsigned char *pRgbData, unsigned int nWidth,
            unsigned int nHeight) {
  if (NULL == pRgbData) {
    return MV_E_PARAMETER;
  }

  for (unsigned int j = 0; j < nHeight; j++) {
    for (unsigned int i = 0; i < nWidth; i++) {
      unsigned char red = pRgbData[j * (nWidth * 3) + i * 3];
      pRgbData[j * (nWidth * 3) + i * 3] =
          pRgbData[j * (nWidth * 3) + i * 3 + 2];
      pRgbData[j * (nWidth * 3) + i * 3 + 2] = red;
    }
  }

  return MV_OK;
}
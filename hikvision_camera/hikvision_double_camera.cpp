#include "hikvision_double_camera.h"
#include "../stereo_lib/stereo_clib.h"
#include <future>
#include <regex>
#include <string>
#include <vector>

#include <filesystem>
#include <iostream>

StereoCamera* StereoCamera::instance{nullptr};

StereoCamera* StereoCamera::GetInstance(){
    if(instance==nullptr){
        instance = new StereoCamera();
        return instance;
    }else{
        return instance;
    }
}

bool StereoCamera::QueryCamera(int id){
    switch (id)
    {
    case /* constant-expression */ 1 :
        return left_camera.m_hDevHandle != NULL;
        break;
    
    case /* constant-expression */ 2 :
        return right_camera.m_hDevHandle != NULL;
        break;
    
    default:
        break;
    }
    return false;
}

StereoCamera::StereoCamera()
{
    std::cout << "查找已连接的设备." << std::endl;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    auto times=1;
    do{
        int nRet = left_camera.EnumDevices(&stDeviceList);
        if (MV_OK != nRet)
        {
            std::cerr << "查找相机失败! 失败参数：nRet [0x" << nRet << "]" << std::endl;
            // perror("101");
            return;
        }else{
            if (stDeviceList.nDeviceNum > 0) { // 找到设备了
                for(auto pDeviceInfo : stDeviceList.pDeviceInfo){
                    if (pDeviceInfo!=nullptr && pDeviceInfo->nTLayerType == MV_GIGE_DEVICE){
                        int nIp4 = (pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
                        camera_ip_map[nIp4] = *pDeviceInfo;
                    }
                }
                auto left = false ,right = false; // 设置没找到相机
                for(auto iter = camera_ip_map.begin(); iter != camera_ip_map.end(); iter++){
                    if(iter->first == LEFT_CAMERA_IP4) left = true;
                    if(iter->first == RIGHT_CAMERA_IP4) right = true;
                }
                if(!left) {std::cerr << "左相机未找到，正在尝试重连..." << std::endl;}
                if(!right) {std::cerr << "右相机未找到，正在尝试重连..." << std::endl;}
                if(left && right) { 
                    std::cout <<  "左右相机连接完成" << std::endl;
                    break; 
                }
            }else{
                 std::cerr<< "未连接到设备，请检查连接吧..." <<std::endl;
            }
        }
    }while(times++ < 20);
    if(times > 20) { 
        std::cerr<< "连接失败20次，请检查设备吧..." <<std::endl;
        // perror("102");
        return;
    }
    left_camera.Open(&camera_ip_map[LEFT_CAMERA_IP4]);
    right_camera.Open(&camera_ip_map[RIGHT_CAMERA_IP4]);

	auto cam_init = [](CMyCamera &cam) {
        auto nPacketSize = cam.GetOptimalPacketSize();
        cam.SetIntValue("GevSCPSPacketSize",nPacketSize);
        cam.SetEnumValue("TriggerMode",MV_TRIGGER_MODE_OFF);
        cam.GetIntValue("PayloadSize",&cam.m_nBufSizeForDriver);
        cam.m_pBufForDriver = (unsigned char *)malloc(cam.m_nBufSizeForDriver);
        cam.StartGrabbing();
	};
	cam_init(left_camera);
	cam_init(right_camera);
}



StereoCamera::~StereoCamera(){
    std::cout << "正关闭左相机..." << std::endl;
    left_camera.StopGrabbing();
    left_camera.Close();
    std::cout << "正关闭右相机..." << std::endl;
    right_camera.StopGrabbing();
    right_camera.Close();
}

void StereoCamera::GrabImageDoubleCamera(){

    auto grab_img = [&](CMyCamera &cam,cv::Mat &out){
        MV_FRAME_OUT_INFO_EX stImageInfo = {0};
        memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
        unsigned int pnDatalen = 0;
        cam.GetOneFrameTimeout(cam.m_pBufForDriver,&pnDatalen,cam.m_nBufSizeForDriver,&stImageInfo,200);
        Convert2Mat(&stImageInfo,cam.m_pBufForDriver,out);
    };
	grab_img(left_camera, leftImg);
	grab_img(right_camera, rightImg);
    //auto grab1 = std::async(std::launch::async,grab_img,left_camera,leftImg);
    //auto grab2 = std::async(std::launch::async,grab_img,right_camera,rightImg);
    //grab1.wait();
    //grab2.wait();
}

void StereoCamera::GrabClibImg(int i){
    GrabImageDoubleCamera();
    SaveGrabImg(i);
}

void StereoCamera::SaveGrabImg(int i){
    cv::imwrite("images/"+ std::to_string(i) + "_left.bmp", leftImg);
    cv::imwrite("images/" + std::to_string(i) + "_right.bmp", rightImg);
}

void StereoCamera::RemapImg(){
    cv::Mat map11, map12, map21, map22;
    cv::Mat img1r, img2r;
    initUndistortRectifyMap(M1, D1, R1, P1, leftImg.size(), CV_16SC2, map11, map12);
    initUndistortRectifyMap(M2, D2, R2, P2, leftImg.size(), CV_16SC2, map21, map22);

    remap(leftImg, img1r, map11, map12, cv::INTER_LINEAR);
    remap(rightImg, img2r, map21, map22, cv::INTER_LINEAR);
}

void StereoCamera::SaveTestImg(int i) {
	cv::imwrite("test/" + std::to_string(i) + "_left.bmp", leftImg);
	cv::imwrite("test/" + std::to_string(i) + "_right.bmp", rightImg);
	auto save_split_img = [&]() {
		cv::Mat left, middle, right;
		auto left_left = cv::Rect(cv::Point(1715, 0), cv::Point(2135, leftImg.rows));
		auto left_middle = cv::Rect(cv::Point(2135, 0), cv::Point(4709, leftImg.rows));
		auto left_right = cv::Rect(cv::Point(4709, 0), cv::Point(5112, leftImg.rows));
		auto right_left = cv::Rect(cv::Point(227, 0), cv::Point(617, leftImg.rows));
		auto right_middle = cv::Rect(cv::Point(617, 0), cv::Point(3185, leftImg.rows));
		auto right_right = cv::Rect(cv::Point(3185, 0), cv::Point(3593, leftImg.rows));
		SplitImg(leftImg, left_left, left_middle, left_right, left, middle, right);
		cv::imwrite("test/" + std::to_string(i) + "_left_left.bmp", left);
		cv::imwrite("test/" + std::to_string(i) + "_left_middle.bmp", middle);
		cv::imwrite("test/" + std::to_string(i) + "_left_right.bmp", right);
		SplitImg(rightImg, right_left, right_middle, right_right, left, middle, right);
		cv::imwrite("test/" + std::to_string(i) + "_right_left.bmp", left);
		cv::imwrite("test/" + std::to_string(i) + "_right_middle.bmp", middle);
		cv::imwrite("test/" + std::to_string(i) + "_right_right.bmp", right);
	};
	save_split_img();
}


void StereoCamera::ClibCam(){
    auto isClibFile = [](std::filesystem::path path)->bool{
        std::string pattern{"\\d+_\\w*.bmp"};
        std::regex re(pattern);
        return std::regex_match(path.string(), re);
    };
    std::filesystem::path str("images");
    std::filesystem::directory_iterator list(str);
    std::vector<std::string> image_list;
    for(auto& it:list){
        if(isClibFile(it.path().filename())){
            image_list.push_back(it.path().string());
        }
    }
    auto boardSize = cv::Size(4,11);
    auto squareSize = 14.0;
    StereoCalibInerAndExter(image_list,boardSize,squareSize,false,true,true);
}

void StereoCamera::LoadParam(){
    LoadInerAndExterParam(M1, D1, M2, D2, R, T, R1, P1, R2, P2, Q);
    // LoadTransformParam(affine_R, affine_T);
}

void StereoCamera::MatchSingleFrame(int i,std::vector<cv::Mat> &results) {
	auto left_left = cv::Rect(cv::Point(1715,0), cv::Point(2135, leftImg.rows));
	auto left_middle = cv::Rect(cv::Point(2135, 0), cv::Point(4709, leftImg.rows));
	auto left_right = cv::Rect(cv::Point(4709, 0), cv::Point(5112, leftImg.rows));
	auto right_left = cv::Rect(cv::Point(227, 0), cv::Point(617, leftImg.rows));
	auto right_middle = cv::Rect(cv::Point(617, 0), cv::Point(3185, leftImg.rows));
	auto right_right = cv::Rect(cv::Point(3185, 0), cv::Point(3593, leftImg.rows));
	std::vector<std::vector<cv::Point2f>> left_points, right_points;
	auto split_img_and_find_point = [&](cv::Rect r1, cv::Rect r2 , cv::Rect r3, cv::Mat &img, std::vector<std::vector<cv::Point2f>>& out) {
		cv::Mat left, middle, right;
		SplitImg(img,r1,r2,r3,left,middle,right);
        auto corase_aera = CoraselyFindCp3(middle);
		auto save_clip = [&]() {
			int count = 0;
			for (auto area : corase_aera){
				cv::Mat tmp = middle(area);
				cv::imwrite("current" + std::to_string(++count)+ ".bmp",tmp);
			}
		};
		save_clip();
		std::vector<std::vector<cv::Point2f>> points(corase_aera.size());
		MutiFindCp3(middle, corase_aera, points);
		MutiFixROIList(points, corase_aera, r2.tl());
		out = points;
	};
	split_img_and_find_point(left_left, left_middle, left_right, leftImg, left_points);
	split_img_and_find_point(right_left, right_middle, right_right, rightImg, right_points);
	
	MutiMatchCp3(left_points, right_points, Q, results);
	for (auto&result : results) {
		std::cout << result << std::endl;
	}
}


// convert data stream in Mat format
bool Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char * pData, cv::Mat& srcImage)
{
    // cv::Mat srcImage;
    if ( pstImageInfo->enPixelType == PixelType_Gvsp_Mono8 )
    {
        srcImage = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC1, pData);
    }
    else if ( pstImageInfo->enPixelType == PixelType_Gvsp_RGB8_Packed )
    {
        RGB2BGR(pData, pstImageInfo->nWidth, pstImageInfo->nHeight);
        srcImage = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC3, pData);
    }
    else
    {
        printf("unsupported pixel format\n");
        return false;
    }

    if ( NULL == srcImage.data )
    {
        return false;
    }

    return true;
}

int RGB2BGR( unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight )
{
    if ( NULL == pRgbData )
    {
        return MV_E_PARAMETER;
    }

    for (unsigned int j = 0; j < nHeight; j++)
    {
        for (unsigned int i = 0; i < nWidth; i++)
        {
            unsigned char red = pRgbData[j * (nWidth * 3) + i * 3];
            pRgbData[j * (nWidth * 3) + i * 3]     = pRgbData[j * (nWidth * 3) + i * 3 + 2];
            pRgbData[j * (nWidth * 3) + i * 3 + 2] = red;
        }
    }

    return MV_OK;
}
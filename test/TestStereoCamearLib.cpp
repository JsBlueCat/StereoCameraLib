#include <iostream>
#include <filesystem>
#include "gtest/gtest.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "config.h"
#include "hikvision_double_camera.h"
#include <ctime>

TEST(StereoCameraLib, TEST_Hello){
    std::cout << "Hello, from StereoCamearLib!\n";
}

TEST(StereoCameraLib, TEST_CXX17){
    std::filesystem::path str("D:\\");
}

TEST(StereoCameraLib, TEST_CV){
    cv::Mat test;
}

TEST(StereoCameraLib, TEST_TIME){
	time_t curr_time;
	curr_time = time(NULL);
    struct tm tm_local;
	localtime_s(&tm_local,&curr_time);
    std::string day = std::to_string(tm_local.tm_year + 1900) + "-"
    + std::to_string(tm_local.tm_mon+1) + "-" + std::to_string(tm_local.tm_mday);

    std::string time = std::to_string(tm_local.tm_hour) + "_"
    + std::to_string(tm_local.tm_min) + "_" + std::to_string(tm_local.tm_sec);

    auto const &config = Config::get_single();
    cv::Mat Test_img = cv::Mat::zeros(224,224,CV_32F);
    config.save_img((config.debug_images/day).string(),time+"_left.bmp",Test_img);
    config.save_img((config.debug_images/day).string(),time+"_right.bmp",Test_img);
}

TEST(StereoCameraLib, TEST_STEREO_CAMERA){
    auto cam = StereoCamera::GetInstance();
}

TEST(StereoCameraLib, TEST_FOUND){
    auto cam = StereoCamera::GetInstance();
    auto const &config = Config::get_single();
    auto match_points = [&](std::string &path){
        cv::Mat timg = cv::imread(path,0);
        // show_img(timg);
        cv::SimpleBlobDetector::Params params;
                    params.filterByColor = true;
                    params.filterByArea = true;
                    params.minArea = 200;
                    params.maxArea = 1e5;
                    params.blobColor = 255;
        cv::Ptr<cv::FeatureDetector> blobDetector = cv::SimpleBlobDetector::create(params);
        std::vector<cv::Point2f> corners(16);
        auto found = cv::findCirclesGrid(timg, cv::Size(8,2), corners, cv::CALIB_CB_SYMMETRIC_GRID | cv::CALIB_CB_CLUSTERING, blobDetector);
        ASSERT_EQ(found,true);
        drawChessboardCorners(timg, cv::Size(8,2), corners, found);
        show_img(timg);
        cornerSubPix(timg, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 0.01));
    };
    std::filesystem::directory_iterator list(config.images_path);
    for (auto &it : list) {
        match_points(it.path().string());
    }
}

TEST(StereoCameraLib, TEST_CLIB){
    auto cam = StereoCamera::GetInstance();
    Check_Api(cam->ClibCam());
}
#include <iostream>
#include <filesystem>
#include "gtest/gtest.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "config.h"
#include "hikvision_double_camera.h"

TEST(StereoCameraLib, TEST_Hello){
    std::cout << "Hello, from StereoCamearLib!\n";
}

TEST(StereoCameraLib, TEST_CXX17){
    std::filesystem::path str("D:\\");
}

TEST(StereoCameraLib, TEST_CV){
    cv::Mat test;
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
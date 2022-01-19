#include <iostream>
#include <filesystem>
#include "gtest/gtest.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "config.h"
#include "hikvision_double_camera.h"
#include <ctime>

TEST(ImageProcess, TEST_hold){
    auto const &config = Config::get_single();
    cv::Mat img = cv::imread("d:\\debug\\test.jpg",0);
    // GaussianBlur( img, img, cv::Size(9, 9), 2, 2 );
    cv::Mat bright;
    cv::convertScaleAbs(img,bright,1.5,60);
    cv::Mat tran_img,tran_img2;
    cv::threshold(bright,tran_img,200,255,2);
    cv::imwrite("d:\\debug\\test_trans.jpg",tran_img);
}


TEST(ImageProcess, TEST_preprocess){
    auto match_points = [&](std::string &path){
        cv::Mat timg = cv::imread(path,0);
        show_img(timg);
        cv::waitKey(500);
        cv::SimpleBlobDetector::Params params;
                    params.filterByColor = true;
                    params.filterByArea = true;
                    params.minArea = 200;
                    params.maxArea = 1e5;
                    params.blobColor = 255;
        cv::Ptr<cv::FeatureDetector> blobDetector = cv::SimpleBlobDetector::create(params);
        std::vector<cv::Point2f> corners(27);
        auto found = cv::findCirclesGrid(timg, cv::Size(3,9), corners, cv::CALIB_CB_ASYMMETRIC_GRID | cv::CALIB_CB_CLUSTERING, blobDetector);
        ASSERT_EQ(found,true);
        drawChessboardCorners(timg, cv::Size(3,9), corners, found);
        show_img(timg);
        cornerSubPix(timg, corners, cv::Size(7, 7), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 80000, 1e-4));
    };
    match_points(std::string("d:\\debug\\test.jpg"));
}
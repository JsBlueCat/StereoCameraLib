#include <iostream>
#include <filesystem>
#include "gtest/gtest.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "config.h"
#include "stereo_clib.h"
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

TEST(ImageProcess,TEST_Multicp3){
    auto const &config = Config::get_single();
    cv::Mat M1, D1, M2, D2, R, T, R1, P1, R2, P2, Q, affine_R, affine_T;
    LoadInerAndExterParam(M1, D1, M2, D2, R, T, R1, P1, R2, P2, Q);
    LoadTransformParam(affine_R,affine_T);
    std::vector<cv::Mat> results;
    std::filesystem::directory_iterator list(config.debug_images/"2500-03");
    std::vector<std::string> cp3_pictures; 
    for (auto &it : list) {
        // std::cout << it.path().string() << std::endl;
        cp3_pictures.push_back(it.path().string());
    }
    ASSERT_TRUE(cp3_pictures.size()%2==0);
    auto match_cp3_points = [&](std::string &left_path,std::string& right_path){
        cv::Mat left_img = cv::imread(left_path,0);
        cv::Mat right_img = cv::imread(right_path,0);
        cv::Mat map11, map12, map21, map22;
        cv::Mat img1r, img2r;
        initUndistortRectifyMap(M1, D1, R1, P1, left_img.size(), CV_16SC2, map11,
                                map12);
        initUndistortRectifyMap(M2, D2, R2, P2, right_img.size(), CV_16SC2, map21,
                                map22);
        remap(left_img, img1r, map11, map12, cv::INTER_LINEAR);
        remap(right_img, img2r, map21, map22, cv::INTER_LINEAR);
        left_img = img1r;
        right_img = img2r;
        show_img(left_img);
        show_img(right_img);
        cv::waitKey(500);

        time_t curr_time;
        curr_time = time(NULL);
        struct tm tm_local;
            localtime_s(&tm_local,&curr_time);
        std::string day = std::to_string(tm_local.tm_year + 1900) + "-"
            + std::to_string(tm_local.tm_mon+1) + "-" + std::to_string(tm_local.tm_mday);
        std::string time = std::to_string(tm_local.tm_hour) + "_"
            + std::to_string(tm_local.tm_min) + "_" + std::to_string(tm_local.tm_sec);

        auto left_left = cv::Rect(cv::Point(0, 0), cv::Point(2244, left_img.rows));
        auto left_middle =
            cv::Rect(cv::Point(2244, 0), cv::Point(4524, left_img.rows));
        auto left_right = cv::Rect(cv::Point(4524, 0), cv::Point(left_img.cols, left_img.rows));
        auto right_left = cv::Rect(cv::Point(0, 0), cv::Point(948, left_img.rows));
        auto right_middle =
            cv::Rect(cv::Point(948, 0), cv::Point(3288, left_img.rows));
        auto right_right =
            cv::Rect(cv::Point(3288, 0), cv::Point(left_img.cols, left_img.rows));
        std::vector<std::vector<cv::Point2f>> left_points, right_points;
        
        auto save_image = [](std::filesystem::path path, std::string filename,
                       cv::Mat &image) {
            cv::imwrite((path / filename).string(), image);
        };
        auto save_split_img = [&]() {
            cv::Mat left, middle, right;
            save_image(config.split_images, time + "left_img.bmp", left_img);
            SplitImg(left_img, left_left, left_middle, left_right, left, middle, right);
            save_image(config.split_images, time + "_left_left.bmp", left);
            save_image(config.split_images, time + "_left_middle.bmp",
                    middle);
            save_image(config.split_images, time + "_left_right.bmp",
                    right);
            save_image(config.split_images, time + "right_img.bmp", right_img);
            SplitImg(right_img, right_left, right_middle, right_right, left, middle,
                    right);
            save_image(config.split_images, time + "_right_left.bmp",
                    left);
            save_image(config.split_images, time + "_right_middle.bmp",
                    middle);
            save_image(config.split_images, time + "_right_right.bmp",
                    right);
        };

        save_split_img();

        auto split_img_and_find_point =
        [&](cv::Rect r1, cv::Rect r2, cv::Rect r3, cv::Mat &img,
            std::vector<std::vector<cv::Point2f>> &out) -> ErrorInfo{
            cv::Mat left, middle, right;
            SplitImg(img, r1, r2, r3, left, middle, right);
            auto corase_area = CoraselyFindCp3(middle);
            if(corase_area.second != ErrorInfo::Success) {
                return corase_area.second;
            }
            std::vector<std::vector<cv::Point2f>> points(corase_area.first.size());
            MutiFindCp3(middle, corase_area.first, points);
            MutiFixROIList(points, corase_area.first, r2.tl());
            out = points;
            return ErrorInfo::Success;
        };

        auto res_left = split_img_and_find_point(left_left, left_middle, left_right,left_img, left_points);
        auto res_right = split_img_and_find_point(right_left, right_middle, right_right, right_img, right_points);

        if (res_left == ErrorInfo::Success && res_right == ErrorInfo::Success) {
            MutiMatchCp3(left_points, right_points, Q, results);
        } else {
            std::cout << "Match MutiCp3 Error" << std::endl;
            return ErrorInfo::Cp3NotFound;
        }

        std::ofstream result_file;
        std::ofstream result_file_xyz;
        result_file.open(config.result_path/(day+".txt"), std::ios::app);
        result_file_xyz.open(config.result_path/(day+"_+xyz.txt"), std::ios::app);
        for (auto &result : results) {
            cv::Mat new_mat;
            std::cout << affine_R << affine_T << std::endl;
            TransfromPoint(result(cv::Rect(0,0,1,3)),affine_R,affine_T,new_mat);
            std::cout << new_mat << std::endl;
            result_file << time << " | " << result << std::endl;
            result_file_xyz << new_mat.at<double>(0,0) << " " << new_mat.at<double>(1,0) << " " <<  new_mat.at<double>(2,0) << std::endl;
        }
        return ErrorInfo::Success;
    };
    for(int i=0; i < cp3_pictures.size(); i+=2){
        match_cp3_points(cp3_pictures[i],cp3_pictures[i+1]);
    }
    
}
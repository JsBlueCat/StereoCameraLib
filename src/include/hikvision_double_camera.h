#pragma once
#include <iostream>
#include <memory>
#include <map>
#include <memory>
#include "MyCamera.h"
#include "debug.h"
#include <opencv2/opencv.hpp>

#define LEFT_CAMERA_IP4 207
#define RIGHT_CAMERA_IP4 122

class /*_declspec(dllexport)*/ StereoCamera{
    public:
        static std::shared_ptr<StereoCamera> GetInstance();
        static std::shared_ptr<StereoCamera> instance;
        static ErrorInfo cam_st;
        ErrorInfo get_camera_error(){return cam_st;}

        StereoCamera();
        ~StereoCamera();

        ErrorInfo GrabImageDoubleCamera();
        ErrorInfo GrabClibImg(int i);
        ErrorInfo SaveGrabImg(int i);
        ErrorInfo ClibCam();
		ErrorInfo LoadParam();
		ErrorInfo SaveTestImg(int i);
		ErrorInfo MatchSingleFrame(int i,std::vector<cv::Mat> &results);
        ErrorInfo QueryCamera(int);
        ErrorInfo RemapImg();
        cv::Mat leftImg,rightImg;
        cv::Mat M1, D1, M2, D2, R, T, R1, P1, R2, P2, Q, affine_R, affine_T;
    private:
        MV_CC_DEVICE_INFO_LIST stDeviceList;
        CMyCamera left_camera;
        CMyCamera right_camera;
        std::map<int, MV_CC_DEVICE_INFO> camera_ip_map;
};

_declspec(dllexport) bool Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char * pData, cv::Mat& srcImage);
_declspec(dllexport) int RGB2BGR( unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight);
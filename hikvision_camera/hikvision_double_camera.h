#pragma once
#include <iostream>
#include <memory>
#include <map>
#include "MyCamera.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#define LEFT_CAMERA_IP4 207
#define RIGHT_CAMERA_IP4 122

class StereoCamera{

    public:
    // 构造函数
        StereoCamera();
    // 析构函数
        ~StereoCamera();
    //抓取一对图像，左右双相机 
        void GrabImageDoubleCamera();
        cv::Mat leftImg,rightImg;
    private:
        MV_CC_DEVICE_INFO_LIST stDeviceList;
        CMyCamera left_camera;
        CMyCamera right_camera;
        std::map<int, MV_CC_DEVICE_INFO> camera_ip_map;
};

bool Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char * pData, cv::Mat& srcImage);
int RGB2BGR( unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight);
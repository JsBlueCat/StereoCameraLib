#pragma once

#include "MyCamera.h"

class StereoCamera{

    public:
    // 构造函数
        StereoCamera();
    // 析构函数
        ~StereoCamera();
    private:
        CMyCamera left_camera;
        CMyCamera right_camera;
};
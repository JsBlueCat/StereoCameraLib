#include "hikvision_double_camera.h"
#include "../stereo_lib/stereo_clib.h"
#include <future>

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
            perror("101");
        }else{
            if (stDeviceList.nDeviceNum > 0) { // 找到设备了
                for(auto pDeviceInfo : stDeviceList.pDeviceInfo){
                    if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE){
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
    }while(times++ < 100);
    if(times > 100) { 
        std::cerr<< "连接失败100次，请检查设备吧..." <<std::endl;
        perror("102");
    }
    left_camera.Open(&camera_ip_map[LEFT_CAMERA_IP4]);
    right_camera.Open(&camera_ip_map[RIGHT_CAMERA_IP4]);
}

StereoCamera::~StereoCamera(){
    free(&stDeviceList);
    std::cout << "正关闭左相机..." << std::endl;
    left_camera.Close();
    std::cout << "正关闭右相机..." << std::endl;
    right_camera.Close();
}

void StereoCamera::GrabImageDoubleCamera(){
    auto grab_img = [&](CMyCamera &cam,cv::Mat &out){
        MV_FRAME_OUT_INFO_EX stImageInfo = {0};
        memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
        cam.GetIntValue("PayloadSize",&cam.m_nBufSizeForDriver);
        cam.m_pBufForDriver = (unsigned char *)malloc(cam.m_nBufSizeForDriver);
        unsigned int pnDatalen = 0;
        cam.GetOneFrameTimeout(cam.m_pBufForDriver,&pnDatalen,cam.m_nBufSizeForDriver,&stImageInfo,200);
        Convert2Mat(&stImageInfo,cam.m_pBufForDriver,out);
    };
    auto grab1 = std::async(std::launch::async,grab_img,left_camera,leftImg);
    auto grab2 = std::async(std::launch::async,grab_img,right_camera,rightImg);
    grab1.wait();
    grab2.wait();
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
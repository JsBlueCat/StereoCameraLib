#include <gtest/gtest.h>
#include "hikvision_double_camera.h"
#include <filesystem>
#include <iostream>
class CameraTests : public ::testing::Test
{
};

TEST_F(CameraTests, hello ){
    std::cout << "hello";
    ASSERT_EQ(1,1);
}

TEST_F(CameraTests, fileDir ){
    std::filesystem::path str("images");
    std::filesystem::directory_iterator list(str);
    for(auto &it:list){
        std::cout << it.path() << std::endl;
    }
}

TEST_F(CameraTests, test_stereo_camera){
    // auto cam = new StereoCamera();
    // delete cam;
}

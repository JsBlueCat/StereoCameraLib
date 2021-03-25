#include <gtest/gtest.h>
#include "hikvision_double_camera.h"
class CameraTests : public ::testing::Test
{
};

TEST_F(CameraTests, hello ){
    std::cout << "hello";
    ASSERT_EQ(1,1);
}

TEST_F(CameraTests, test_stereo_camera){
    auto cam = new StereoCamera();
    delete cam;
    ASSERT_EQ(1,2);
}

#include "hikvision_double_camera.h"
#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include "config.hpp"
class CameraTests : public ::testing::Test {};

TEST(CameraTests, hello) {
  std::cout << "hello";
  ASSERT_EQ(1, 1);
}

TEST(CameraTests, fileDir) {
  std::filesystem::path str("images");
  std::filesystem::directory_iterator list(str);
  for (auto &it : list) {
    std::cout << it.path() << std::endl;
  }
}

TEST(CameraTests, test_stereo_camera) {
  auto cam = new StereoCamera();
  delete cam;
}

TEST(CameraTests, test_config) {
    auto const& config = Config::get_single();
}
#define NODE_API_SWALLOW_UNTHROWABLE_EXCEPTIONS
#include "config.h"
#include "hikvision_double_camera.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>



int main() {
  auto const &config = Config::get_single("D://Debug", cv::Size(3, 9), 50);
  auto Fine_Tune_Device = new StereoCamera();
  Check_Api(Fine_Tune_Device->get_camera_error());
  Check_Api(Fine_Tune_Device->LoadParam());
  int b = 1;
  int count = 1;
  std::cout << "1 continue , 0 quit , max 30..." << std::endl;
  std::vector<cv::Mat> results;
  std::ofstream outfile(config.root_path / "celiang.txt", std::ios::app);
  while (std::cin >> b, b) {
    Check_Api(Fine_Tune_Device->GrabImageDoubleCamera());
    Check_Api(Fine_Tune_Device->RemapImg());
    Check_Api(Fine_Tune_Device->SaveTestImg(count++));
    Check_Api(Fine_Tune_Device->MatchSingleFrame(count, results));
    for (auto &result : results) {
      outfile << result.at<double>(0, 0) << " " << result.at<double>(1, 0)
              << " " << result.at<double>(2, 0) << std::endl;
    }
    std::cout << "1 continue , 0 quit , max 30..." << std::endl;
  }
  delete Fine_Tune_Device;
  return 0;
}
#include "hikvision_double_camera.h"
#include <fstream>
#include <exception>  
#include <iostream>
#include "config.h"


int main() {
	auto const &config = Config::get_single("D://Debug",cv::Size(3,9),50);
  auto Fine_Tune_Device = new StereoCamera();
  Check_Api(Fine_Tune_Device->get_camera_error());
  int b = 1;

  int count = 0;
  std::cout << "1 continue , 0 quit , max 60..." << std::endl;
  while (std::cin >> b, b) {
    if (count > 60)
      break;
    Check_Api(Fine_Tune_Device->GrabClibImg(++count));
    Check_Api(Fine_Tune_Device->SaveGrabImg(count));
    std::cout << "current now " << count << " picture..." << std::endl;
    std::cout << "1 continue , 0 quit , max 60..." << std::endl;
  }
  Check_Api(Fine_Tune_Device->ClibCam());
  delete Fine_Tune_Device;
  return 0;
}
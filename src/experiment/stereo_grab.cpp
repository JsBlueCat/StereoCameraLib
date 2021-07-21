#include "hikvision_double_camera.h"
#include <fstream>
#include <iostream>


int main() {
  auto Fine_Tune_Device = new StereoCamera();
  int b = 1;

  int count = 0;
  std::cout << "1 continue , 0 quit , max 30..." << std::endl;
	auto const &config = Config::get_single("D://Debug",cv::Size(8,2),30);
  while (std::cin >> b, b) {
    if (count > 30)
      break;
    Fine_Tune_Device->GrabClibImg(++count);
    Fine_Tune_Device->SaveGrabImg(count);
    std::cout << "current now " << count << " picture..." << std::endl;
    std::cout << "1 continue , 0 quit , max 30..." << std::endl;
  }
  Fine_Tune_Device->ClibCam();
  delete Fine_Tune_Device;
  return 0;
}
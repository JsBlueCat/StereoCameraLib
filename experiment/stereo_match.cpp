#include <iostream>
#include <fstream>
#include "../hikvision_camera/hikvision_double_camera.h"

int main() {
	system("chcp 65001");
	auto Fine_Tune_Device = new StereoCamera();
	Fine_Tune_Device->LoadParam();
	int b = 1;
	int count = 1;
	std::cout << "1 continue , 0 quit , max 30..." << std::endl;
	std::vector<cv::Mat> results;
	while (std::cin >> b, b) {
		Fine_Tune_Device->GrabImageDoubleCamera();
		Fine_Tune_Device->SaveTestImg(count++);
		Fine_Tune_Device->MatchSingleFrame(count,results);
		std::cout << "1 continue , 0 quit , max 30..." << std::endl;
	}
	delete Fine_Tune_Device;
	return 0;
}
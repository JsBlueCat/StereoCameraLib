#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "hikvision_double_camera.h"

int main() {
	auto Fine_Tune_Device = new StereoCamera();
	Fine_Tune_Device->LoadParam();
	int b = 1;
	int count = 1;
	std::cout << "1 continue , 0 quit , max 30..." << std::endl;
	std::vector<cv::Mat> results;
	std::ofstream outfile("celiang.txt", std::ios::app);
	while (std::cin >> b, b) {
		Fine_Tune_Device->GrabImageDoubleCamera();
		Fine_Tune_Device->RemapImg();
		Fine_Tune_Device->SaveTestImg(count++);
		Fine_Tune_Device->MatchSingleFrame(count,results);
		for (auto &result : results){
			outfile << result.at<double>(0, 0) << " " << result.at<double>(1, 0) << " " << result.at<double>(2, 0) << std::endl;
		}
		std::cout << "1 continue , 0 quit , max 30..." << std::endl;
	}
	delete Fine_Tune_Device;
	return 0;
}
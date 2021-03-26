#include <iostream>
#include <fstream>
#include "../hikvision_camera/hikvision_double_camera.h"

int main() {
	auto Fine_Tune_Device = new StereoCamera();
	Fine_Tune_Device->LoadParam();
	delete Fine_Tune_Device;
	return 0;
}
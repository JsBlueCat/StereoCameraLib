#include "stereo_clib.h"
#include "config.h"
using namespace std;
using namespace cv;

ErrorInfo StereoCalibInerAndExter(const std::vector<std::string> &imagelist, cv::Size boardSize, float squareSize, bool displayCorners , bool useCalibrated , bool showRectified )
{
	if (imagelist.size() % 2 != 0)
	{
		std::cout << "Error: the image list contains odd (non-even) number of elements\n";
		return ErrorInfo::ImageNotPaired;
	}

	const int maxScale = 2;
	// ARRAY AND VECTOR STORAGE:

	std::vector<std::vector<cv::Point2f>> imagePoints[2];
	std::vector<std::vector<cv::Point3f>> objectPoints;
	cv::Size imageSize;

	int i, j, k, nimages = (int)imagelist.size() / 2;

	imagePoints[0].resize(nimages);
	imagePoints[1].resize(nimages);
	std::vector<std::string> goodImageList;

	for (i = j = 0; i < nimages; i++)
	{
		for (k = 0; k < 2; k++)
		{
			const std::string &filename = imagelist[i * 2 + k];
			auto match_points_func = [&](std::string path, std::vector<cv::Point2f> &corners) -> bool {
				cv::Mat timg = cv::imread(path,0);
				if(imageSize == cv::Size())
					imageSize = timg.size();
				// show_img(timg);
				cv::SimpleBlobDetector::Params params;
							params.filterByColor = true;
							params.filterByArea = true;
							params.minArea = 200;
							params.maxArea = 1e5;
							params.blobColor = 255;
				cv::Ptr<cv::FeatureDetector> blobDetector = cv::SimpleBlobDetector::create(params);
				auto found = cv::findCirclesGrid(timg, boardSize, corners, cv::CALIB_CB_ASYMMETRIC_GRID | cv::CALIB_CB_CLUSTERING, blobDetector);
				if(!found){ return found;}
				drawChessboardCorners(timg, boardSize, corners, found);
				std::cout << "before subpix" <<cv::Mat(corners);
				show_img(timg);
				cornerSubPix(timg, corners, Size(11,11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 10000, 1e-5));
				std::cout << "after subpix" <<cv::Mat(corners);
				return true;
			};
			auto found = match_points_func(filename,std::move(imagePoints[k][j]));
		}
		if (k == 2)
		{
			goodImageList.push_back(imagelist[i * 2]);
			goodImageList.push_back(imagelist[i * 2 + 1]);
			j++;
		}
	}
	std::cout << j << " pairs have been successfully detected.\n";
	nimages = j;
	if (nimages < 2)
	{
		std::cout << "Error: too little pairs to run the calibration\n";
		return ErrorInfo::NotSufficientImage;
	}

	imagePoints[0].resize(nimages);

	imagePoints[1].resize(nimages);
	objectPoints.resize(nimages);

	for (i = 0; i < nimages; i++)
	{
		for (j = 0; j < boardSize.height; j++)
			for (k = 0; k < boardSize.width; k++)
				objectPoints[i].push_back(cv::Point3f(float(( 2 * k + j % 2) * squareSize), float(j * squareSize), 0));
				// objectPoints[i].push_back(cv::Point3f(float(j * squareSize), float(k * squareSize), 0));
	}

	std::cout << "Running stereo calibration ...\n";
//  method 1 
/*
	std::vector<cv::Mat> cameraMatrix = { cv::Mat::eye(3, 3, CV_64F),cv::Mat::eye(3, 3, CV_64F) };
	std::vector<cv::Mat> distCoeffs = { cv::Mat::zeros(5, 1, CV_64F),cv::Mat::zeros(5, 1, CV_64F) };
	cameraMatrix[0].at<double>(0,0) = (5472*16/12.8)/(3648*16/9.6);
	// cameraMatrix[0].at<double>(0, 0) = 5472*16/12.8;
	// cameraMatrix[0].at<double>(0, 2) = 2736;
	// cameraMatrix[0].at<double>(1, 1) = 3648*16/9.6;
	// cameraMatrix[0].at<double>(1, 2) = 1824;

	cameraMatrix[1] = cameraMatrix[0];
	{ // method 1 find the point with clibrate two camera
		// double aspectRatio = (double)9.6/12.8;
		// cv::Mat cameraMatrix[2], distCoeffs[2], r, t;
		for (int i = 0; i < 2; i++) {
			cv::Mat r, t;
			double err = calibrateCamera(objectPoints, imagePoints[i], imageSize, cameraMatrix[i],
				distCoeffs[i], r, t,
				CALIB_FIX_ASPECT_RATIO +
				CALIB_FIX_PRINCIPAL_POINT + 
				CALIB_ZERO_TANGENT_DIST +
				CALIB_FIX_K4 +            
				CALIB_FIX_K5
				);
		}

	}
	std::cout << cameraMatrix[0];
	std::cout << cameraMatrix[1];
*/

	cv::Mat R, T, E, F ,perViewErrors;
	Mat cameraMatrix[2], distCoeffs[2];
    cameraMatrix[0] = initCameraMatrix2D(objectPoints,imagePoints[0],imageSize, 0.5);
    cameraMatrix[1] = initCameraMatrix2D(objectPoints,imagePoints[1],imageSize, 0.5);
	double rms = stereoCalibrate(objectPoints, imagePoints[0], imagePoints[1],
								 cameraMatrix[0], distCoeffs[0],
								 cameraMatrix[1], distCoeffs[1],
								 imageSize, R, T, E, F, perViewErrors,
								 CALIB_FIX_ASPECT_RATIO +
								 CALIB_ZERO_TANGENT_DIST +
								 CALIB_USE_INTRINSIC_GUESS +
								 CALIB_SAME_FOCAL_LENGTH +
								 CALIB_RATIONAL_MODEL + 
								 CALIB_FIX_K3 + CALIB_FIX_K4 + CALIB_FIX_K5,
								 TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 10000, 1e-6)
								 );
	std::cout << "done with RMS error=" << rms << std::endl;
	std::cout << cameraMatrix[0];
	std::cout << cameraMatrix[1];
	std::cout << R << T;

	// CALIBRATION QUALITY CHECK
	// because the output fundamental matrix implicitly
	// includes all the output information,
	// we can check the quality of calibration using the
	// epipolar geometry constraint: m2^t*F*m1=0
	double err = 0;
	int npoints = 0;
	std::vector<cv::Vec3f> lines[2];
	for (i = 0; i < nimages; i++)
	{
		int npt = (int)imagePoints[0][i].size();
		cv::Mat imgpt[2];
		for (k = 0; k < 2; k++)
		{
			imgpt[k] = cv::Mat(imagePoints[k][i]);
			undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoeffs[k], cv::Mat(), cameraMatrix[k]);
			computeCorrespondEpilines(imgpt[k], k + 1, F, lines[k]);
		}
		for (j = 0; j < npt; j++)
		{
			double errij = fabs(imagePoints[0][i][j].x * lines[1][j][0] +
								imagePoints[0][i][j].y * lines[1][j][1] + lines[1][j][2]) +
						   fabs(imagePoints[1][i][j].x * lines[0][j][0] +
								imagePoints[1][i][j].y * lines[0][j][1] + lines[0][j][2]);
			err += errij;
		}
		npoints += npt;
	}
	std::cout << "average epipolar err = " << err / npoints << std::endl;

	auto const & config = Config::get_single();
	// save intrinsic parameters
	cv::FileStorage fs((config.config_path /  "intrinsics.yml").string(), cv::FileStorage::WRITE);
	if (fs.isOpened())
	{
		fs << "M1" << cameraMatrix[0] << "D1" << distCoeffs[0] << "M2" << cameraMatrix[1] << "D2" << distCoeffs[1];
		fs.release();
	}
	else
		std::cout << "Error: can not save the intrinsic parameters\n";

	cv::Mat R1, R2, P1, P2, Q;
	cv::Rect validRoi[2];

	cv::stereoRectify(cameraMatrix[0], distCoeffs[0],
				  cameraMatrix[1], distCoeffs[1],
				  imageSize, R, T, R1, R2, P1, P2, Q,
				  cv::CALIB_ZERO_DISPARITY, 1, imageSize, &validRoi[0], &validRoi[1]);
	

	fs.open((config.config_path /  "extrinsics.yml").string(), cv::FileStorage::WRITE);
	if (fs.isOpened())
	{
		fs << "R" << R << "T" << T << "R1" << R1 << "R2" << R2 << "P1" << P1 << "P2" << P2 << "Q" << Q;
		fs.release();
	}
	else
		std::cout << "Error: can not save the extrinsic parameters\n";

	// OpenCV can handle left-right
	// or up-down camera arrangements
	bool isVerticalStereo = fabs(P2.at<double>(1, 3)) > fabs(P2.at<double>(0, 3));

	// COMPUTE AND DISPLAY RECTIFICATION
	if (!showRectified)
		return ErrorInfo::Success;

	cv::Mat rmap[2][2];
	// IF BY CALIBRATED (BOUGUET'S METHOD)
	if (useCalibrated)
	{
		// we already computed everything
	}
	// OR ELSE HARTLEY'S METHOD
	else
	// use intrinsic parameters of each camera, but
	// compute the rectification transformation directly
	// from the fundamental matrix
	{
		std::vector<cv::Point2f> allimgpt[2];
		for (k = 0; k < 2; k++)
		{
			for (i = 0; i < nimages; i++)
				std::copy(imagePoints[k][i].begin(), imagePoints[k][i].end(), back_inserter(allimgpt[k]));
		}
		F = cv::findFundamentalMat(cv::Mat(allimgpt[0]), cv::Mat(allimgpt[1]), cv::FM_8POINT, 0, 0);
		cv::Mat H1, H2;
		stereoRectifyUncalibrated(cv::Mat(allimgpt[0]), cv::Mat(allimgpt[1]), F, imageSize, H1, H2, 3);

		R1 = cameraMatrix[0].inv() * H1 * cameraMatrix[0];
		R2 = cameraMatrix[1].inv() * H2 * cameraMatrix[1];
		P1 = cameraMatrix[0];
		P2 = cameraMatrix[1];
	}

	//Precompute maps for cv::remap()
	initUndistortRectifyMap(cameraMatrix[0], distCoeffs[0], R1, P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
	initUndistortRectifyMap(cameraMatrix[1], distCoeffs[1], R2, P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);

	cv::Mat canvas;
	double sf;
	int w, h;
	if (!isVerticalStereo)
	{
		sf = 600. / MAX(imageSize.width, imageSize.height);
		w = cvRound(imageSize.width * sf);
		h = cvRound(imageSize.height * sf);
		canvas.create(h, w * 2, CV_8UC3);
	}
	else
	{
		sf = 300. / MAX(imageSize.width, imageSize.height);
		w = cvRound(imageSize.width * sf);
		h = cvRound(imageSize.height * sf);
		canvas.create(h * 2, w, CV_8UC3);
	}

	for (i = 0; i < nimages; i++)
	{
		for (k = 0; k < 2; k++)
		{
			cv::Mat img = cv::imread(goodImageList[i * 2 + k], 0), rimg, cimg;
			remap(img, rimg, rmap[k][0], rmap[k][1], cv::INTER_LINEAR);
			cvtColor(rimg, cimg, cv::COLOR_GRAY2BGR);
			cv::Mat canvasPart = !isVerticalStereo ? canvas(cv::Rect(w * k, 0, w, h)) : canvas(cv::Rect(0, h * k, w, h));
			cv::resize(cimg, canvasPart, canvasPart.size(), 0, 0, cv::INTER_AREA);
			if (useCalibrated)
			{
				cv::Rect vroi(cvRound(validRoi[k].x * sf), cvRound(validRoi[k].y * sf),
						  cvRound(validRoi[k].width * sf), cvRound(validRoi[k].height * sf));
				rectangle(canvasPart, vroi, cv::Scalar(0, 0, 255), 3, 8);
			}
		}

		if (!isVerticalStereo)
			for (j = 0; j < canvas.rows; j += 16)
				line(canvas, cv::Point(0, j), cv::Point(canvas.cols, j), cv::Scalar(0, 255, 0), 1, 8);
		else
			for (j = 0; j < canvas.cols; j += 16)
				line(canvas, cv::Point(j, 0), cv::Point(j, canvas.rows), cv::Scalar(0, 255, 0), 1, 8);
		imshow("rectified", canvas);
		char c = (char)cv::waitKey();
		if (c == 27 || c == 'q' || c == 'Q')
			break;
	}
	return ErrorInfo::Success;
}

ErrorInfo LoadInerAndExterParam(cv::Mat &M1,cv::Mat &D1,cv::Mat &M2,cv::Mat &D2,cv::Mat &R,cv::Mat &T,cv::Mat &R1,cv::Mat &P1,cv::Mat &R2,cv::Mat &P2,cv::Mat & Q) {
	auto const &config = Config::get_single();
	cv::FileStorage fs((config.config_path / "intrinsics.yml").string(), cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		printf("Failed to open file %s\n", "intrinsics.yml");
		return ErrorInfo::ConfigNotFound;
	}

	fs["M1"] >> M1;
	fs["D1"] >> D1;
	fs["M2"] >> M2;
	fs["D2"] >> D2;

	fs.open((config.config_path / "extrinsics.yml").string(), cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		printf("Failed to open file %s\n", "extrinsics.yml");
		return ErrorInfo::ConfigNotFound;
	}
	fs["R"] >> R;
	fs["T"] >> T;
	fs["R1"] >> R1;
	fs["P1"] >> P1;
	fs["R2"] >> R2;
	fs["P2"] >> P2;
	fs["Q"] >> Q;
	return ErrorInfo::Success;
}

ErrorInfo LoadTransformParam(cv::Mat &affine_R,cv::Mat &affine_T){
	auto const &config = Config::get_single();
	cv::FileStorage fs((config.config_path / "transform.yml").string(), cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		printf("Failed to open file %s\n", "transform.yml");
		return ErrorInfo::ConfigNotFound;
	}
	cv::Mat transform;
	fs["transform"]>>transform;
	affine_R = transform.colRange(cv::Range(0,3));
	affine_T = transform.colRange(cv::Range(3,4));
	return ErrorInfo::Success;
}

std::future<bool> FindCp3PointFromOneClipAsync(cv::Mat& img, std::vector<cv::Point2f> &target_point){
	auto opencv_find_cycles = [&]() -> bool {
		cv::SimpleBlobDetector::Params params;
		params.filterByColor = true;
		params.filterByArea = true;
		params.minArea = 10;
		params.maxArea = 1e5;
		params.blobColor = 255;
		cv::Ptr<cv::FeatureDetector> blobDetector = cv::SimpleBlobDetector::create(params);
		return findCirclesGrid(img, cv::Size(2, 2), target_point, cv::CALIB_CB_SYMMETRIC_GRID | cv::CALIB_CB_CLUSTERING, blobDetector);
	};
	return std::async(std::launch::async, opencv_find_cycles);
}

void SplitImg(cv::Mat &img, cv::Rect left, cv::Rect middle, cv::Rect right, cv::Mat &left_known, cv::Mat &target, cv::Mat &right_known) {
	left_known = img(left);
	target = img(middle);
	right_known = img(right);
}

void SplitMutliCp3Img(cv::Mat &img_middle, std::vector<cv::Rect> &areas, std::vector<cv::Mat> &cached_imgs){
	for(int i = 0 ;i < areas.size(); i ++){
		auto area = areas[i];
		cached_imgs[i] = img_middle(area);
	}
}

ErrorInfo MatchCp3(std::vector<cv::Point2f> &target_l_points, std::vector<cv::Point2f> &target_r_points, cv::Mat &Q, cv::Mat &avg) {
	if(target_l_points.size() != target_r_points.size()){
		return ErrorInfo::Cp3PointNotEqual;
	}
	avg = cv::Mat::zeros(4, 1, CV_64FC1);
	for (int i = 0; i < target_l_points.size(); i++) {
		double distance_l = _ABS(target_r_points[i].x - target_l_points[i].x);//_ABS(target_r_points[i].x - target_l_points[i].x);
		cv::Mat temp(4, 1, CV_64FC1);
		//cout << target_r_points[i].x << ","<< target_r_points[i].y << endl;
		temp.at<double>(0, 0) = target_l_points[i].x;
		temp.at<double>(1, 0) = target_l_points[i].y;
		temp.at<double>(2, 0) = distance_l;
		temp.at<double>(3, 0) = 1.;
		cv::Mat result = (Q * temp);
		double W = result.at<double>(3, 0);
		result /= W;
		result.at<double>(3, 0) = distance_l;
		avg += result;
	}
	avg /= static_cast<double>(target_l_points.size());
	avg.at<double>(1, 0) *= -1;
	return ErrorInfo::Success;
}

std::future<bool> DetectCyclesFromWholeImgAsync(cv::Mat &middle, std::vector<cv::Vec3f> &circles){
	auto canny_func = [&]()-> bool {
		cv::Mat cimg;
		cimg = middle.clone();
		// medianBlur(cimg, cimg, 5);
		//cv::cvtColor(middle,cimg,cv::COLOR_BGR2GRAY);
		GaussianBlur(cimg, cimg, cv::Size(9, 9), 2, 2);
		//   medianBlur(cimg, cimg, 5);
		// Canny(cimg,cimg,10,250,5);
		// imshow("canny",cimg);
		HoughCircles(cimg, circles, cv::HOUGH_GRADIENT, 1, 30 ,200, 30 , 5, 100 );
		return circles.size() > 0;
	};
	return std::async(std::launch::async, canny_func);
}

// 从中间图像中获取canny结果,并将图像等分并返回 所需截图区域
std::pair<std::vector<cv::Rect>,ErrorInfo> CoraselyFindCp3(cv::Mat& middle){
	std::vector<cv::Rect> result;
	std::vector<cv::Vec3f> points;
	auto point_list = DetectCyclesFromWholeImgAsync(middle, points);
	point_list.wait();
	// 必须找到4个坐标点
	if(points.size() % POINTS_ON_EACH_CP3 != 0 ){
		return std::make_pair<std::vector<cv::Rect>,ErrorInfo>({},ErrorInfo::Cp3PointFoundNotPair);
	}
	auto num_of_cp3 = points.size() /  POINTS_ON_EACH_CP3;
	std::sort(points.begin(),points.end(),[=](const cv::Vec3f & vec1, const cv::Vec3f & vec2){
		return vec1[0] < vec2[0];
	});
	for(int i = 0; i < points.size(); i+=POINTS_ON_EACH_CP3){
		// 生成切图的区域
		auto min_x = std::min({points[i][0],points[i+1][0],points[i+2][0],points[i+3][0]});
		auto min_y = std::min({points[i][1],points[i+1][1],points[i+2][1],points[i+3][1]});
		auto max_x = std::max({points[i][0],points[i+1][0],points[i+2][0],points[i+3][0]});
		auto max_y = std::max({points[i][1],points[i+1][1],points[i+2][1],points[i+3][1]});

		auto width = _ABS(max_x - min_x) / 2;
		auto height = _ABS(max_y - min_y) / 2;

		auto left_top_point = cv::Point( int(min_x - width), int(min_y - height));
		auto right_bottom_point = cv::Point( int(max_x + width),int(max_y + height));
		result.push_back(cv::Rect(left_top_point,right_bottom_point));
	}
	return std::make_pair<std::vector<cv::Rect>,ErrorInfo>(std::move(result),ErrorInfo::Success);
}

ErrorInfo MutiFindCp3(cv::Mat &middle_img,std::vector<cv::Rect> &area,std::vector<std::vector<cv::Point2f>> &target_point_list){
	for(auto &list:target_point_list){
		list.clear();
	}
	std::vector<cv::Mat> cached_imgs(area.size());
	SplitMutliCp3Img(middle_img,area,cached_imgs);
	std::vector<std::future<bool>> tasklist;
	for(int i = 0 ; i < cached_imgs.size() ;  i ++){
		auto t1 = FindCp3PointFromOneClipAsync(cached_imgs[i],target_point_list[i]);
		tasklist.push_back(std::move(t1));
	}
	for (auto &&task : tasklist) { 
		task.wait(); 
		auto flag = task.get();
	}
	return ErrorInfo::Success;
}

void MutiFixROIList(std::vector<std::vector<cv::Point2f>>& corner_list, std::vector<cv::Rect>& rect_list , cv::Point2f roi) {
	for(int i = 0; i < rect_list.size(); i++ ){
		for(auto &corner:corner_list[i]){
			corner+=(cv::Point2f)rect_list[i].tl() + roi;
		}
	}
}

ErrorInfo MutiMatchCp3(std::vector<std::vector<cv::Point2f>> &left_target_points,std::vector<std::vector<cv::Point2f>> &right_target_points,cv::Mat &Q, std::vector<cv::Mat> &out){
	if(left_target_points.size() != right_target_points.size()){
		return ErrorInfo::Cp3PointNotEqual;
	}
	out.clear();
	std::vector<cv::Mat> results;
	for(int i = 0; i < left_target_points.size() ;i ++){
		cv::Mat tmp; 
		MatchCp3(left_target_points[i],right_target_points[i],Q,tmp);
		results.push_back(tmp);
	}
	out = results;
	return ErrorInfo::Success;
}

ErrorInfo TransfromPoint(cv::Mat& points, cv::Mat& affine_R, cv::Mat &affine_T ,cv::Mat & out){
	out = affine_R * points + affine_T;
	return ErrorInfo::Success;
}

// std::pair<std::vector<cv::Rect>,ErrorInfo> CoraselyFindCp3(cv::Mat& middle){

// }

// void FixROI(std::vector<std::vector<cv::Point2f>> &corners, cv::Point2f &roi) {
// 	// 消除整张图的偏移
// 	for(auto &list:corners){
// 		for(auto &corner:list){
// 			corner+=roi;
// 		}
// 	}
// }


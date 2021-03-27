#pragma once
//===========================================
#define _ABS(x) ((x) > 0 ? (x) : -(x))
const int POINTS_ON_EACH_CP3 = 4;


//===========================================
#include <opencv2/opencv.hpp>
#include<iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <future>


typedef enum{
	LEFT,
	RIGHT
} CAM_MODE;

void StereoCalibInerAndExter(const std::vector<std::string> &, cv::Size,float,bool = false,bool = true ,bool = true); // false ture true
void LoadInerAndExterParam(cv::Mat &M1,cv::Mat &D1,cv::Mat &M2,cv::Mat &D2,cv::Mat &R,cv::Mat &T,cv::Mat &R1,cv::Mat &P1,cv::Mat &R2,cv::Mat &P2,cv::Mat & Q);
void LoadTransformParam(cv::Mat &affine_R,cv::Mat &affine_T);
void FirstRectify(const std::vector<std::string> &);
// 找到一张切图中间的cp3点
std::future<bool> FindCp3PointFromOneClipAsync(cv::Mat&, std::vector<cv::Point2f> &);
void MatchCp3(std::vector<cv::Point2f> &target_l_points, std::vector<cv::Point2f> &target_r_points, cv::Mat &Q, cv::Mat &avg);
// 切分相机的左靶标，右靶标，中间区域
void SplitImg(cv::Mat &img, cv::Rect , cv::Rect , cv::Rect , cv::Mat &, cv::Mat &, cv::Mat &);
void SplitMutliCp3Img(cv::Mat &img_middle, std::vector<cv::Rect> &areas, std::vector<cv::Mat> &cached_imgs);
// 使用candy 算子和 高斯模糊查找图片中的原点序列
std::future<bool> DetectCyclesFromWholeImgAsync(cv::Mat &middle, std::vector<cv::Vec3f> &circles);
std::vector<cv::Rect> CoraselyFindCp3(cv::Mat& middle);

void MutiFindCp3(cv::Mat &middle_img,std::vector<cv::Rect> &area,std::vector<std::vector<cv::Point2f>> &target_point_list);
void MutiFixROIList(std::vector<std::vector<cv::Point2f>>& corner_list, std::vector<cv::Rect>& rect_list, cv::Point2f roi);
void MutiMatchCp3(std::vector<std::vector<cv::Point2f>> &,std::vector<std::vector<cv::Point2f>> &,cv::Mat &, std::vector<cv::Mat> &);

// static void FixROI(std::vector<cv::Point2f>& corners, cv::Point2f &roi);
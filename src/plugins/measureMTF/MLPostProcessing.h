#pragma once
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
enum class enImageMirror
{
	Non = 0,
	LeftRight = 1,
	UpDown = 2,
};

enum class enImageRotate
{
	R0 = 0,
	R90 = 1,
	R180 = 2,
	R270 = 3,
};
class MLPostProcessing
{
public:
	MLPostProcessing();
	~MLPostProcessing();

	cv::Mat ImageRotate(enImageRotate enRotate,cv::Mat src);
	cv::Mat ImageMirror(enImageMirror enMirror,cv::Mat src);

	cv::Mat Binning(cv::Mat& src, int bin);
	cv::Mat BinningZoom(cv::Mat& src, int bin);
	//cv::Mat Postprocessing(cv::Mat src, enImageRotate rotate, enImageMirror mirror);
private:

};


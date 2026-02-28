#pragma once
#include<deque>
#include <opencv2/opencv.hpp>
#include "coreplugin/service/ml.h"
//class __declspec(dllexport)  ML_Frame;
using namespace cv;
using cv::Point2f;

class __declspec(dllexport) CrossCenterPro
{
public:
	CrossCenterPro();
	~CrossCenterPro();

	void  SetFocalLength(double fl = 94.45);

	void  SetFramePixel(double pixel = 0.0055);

	void  SetBaseCenter(int pointx = 1024, int pointy = 1024);

	bool  Execute(Mat frame, int average_amount = 0);

	CORE::ML_POINT2F  GetCrossPiont();

	double  GetAngle();

	double  GetH();

	double  GetV();

	double  GetAngleX();

	double  GetAngleY();

private:

	double focallength;
	double pixel;
	cv::Point2f basePoint;
	cv::Point2f currCenter;
	double angle;
	std::deque<cv::Point2f> m_averagePointArray;

	double H;
	double V;

	double AngleX;
	double AngleY;
};


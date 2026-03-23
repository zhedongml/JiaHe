#pragma once
#include<deque>
#include <opencv2/opencv.hpp>
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif

//class __declspec(dllexport)  ML_Frame;
//using namespace ML;

enum CrossMode
{
	HoffLine,
	Gaussian,
	GaussianEnhance
};

class ALGORITHM_API CrossCenterPro
{
public:
	CrossCenterPro();
	~CrossCenterPro();

	void  SetFocalLength(double fl = 94.45);

	void  SetFramePixel(double pixel = 0.0055);

	void  SetBaseCenter(double pointx = 1024, double pointy = 1024);

	void SetCrossModel(int crossnumber);

	bool  Execute(cv::Mat frame, int average_amount = 0, CrossMode crossMode = CrossMode::Gaussian);

	cv::Point2f  GetCrossPiont();

	cv::Point2f  GetBasePiont();

	double  GetAngle();

	double  GetH();

	double  GetV();

	double  GetAngleX();

	double  GetAngleY();

	double GetHorizontalLineAngle();

private:

	double m_focallength;
	double m_pixel;
	cv::Point2f m_basePoint;
	cv::Point2f m_currCenter;
	double m_angle;
	std::deque<cv::Point2f> m_averagePointArray;

	double m_H;
	double m_V;

	double m_AngleX;
	double m_AngleY;

	int m_crossnumber;

	double m_fLineAngle;
};


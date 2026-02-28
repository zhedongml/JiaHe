#include "CrossCenterPro.h"
#include"CrossCenter.h"
#include"qlog.h"
#include<string>
#include "ml_two_cross.h"

using namespace std;    
using namespace cv;

CrossCenterPro::CrossCenterPro()
	:m_focallength(94.45)
	, m_pixel(0.0055)
	, m_basePoint(1024.0, 1024.0)
	, m_currCenter(0.0, 0.0)
	, m_angle(0)
	, m_H(0)
	, m_V(0)
	, m_AngleX(0)
	, m_AngleY(0)
	, m_crossnumber(1)
	, m_fLineAngle(0)
{
}


CrossCenterPro::~CrossCenterPro()
{
}


void CrossCenterPro::SetFocalLength(double fl)
{
	m_focallength = fl;
}

void CrossCenterPro::SetFramePixel(double p)
{
	m_pixel = p;
}

void CrossCenterPro::SetBaseCenter(double x, double y)
{
	m_basePoint = Point2f(x, y);
}

void CrossCenterPro::SetCrossModel(int crossnumber)
{
	if (crossnumber == 1 || crossnumber == 2)
	{
		m_crossnumber = crossnumber;
	}
	
}

bool CrossCenterPro::Execute(Mat mlframe, int average, CrossMode crossMode)
{

	if (m_focallength <= 0 || m_pixel <= 0 || mlframe.empty())
	{
		return false;
	}


	if (mlframe.depth() == CV_16U)
	{
		Mat outtmp;// = Mat(roi.cols, roi.rows, CV_8UC1);

		double minv = 0.0, maxv = 0.0;
		double* minp = &minv;
		double* maxp = &maxv;
		minMaxIdx(mlframe, minp, maxp);

		if (maxv > 4095.1)
		{
			cv::Mat tmp = mlframe / 65535 * 255;

			tmp.convertTo(outtmp, CV_8UC1);
		}
		else
		{
			cv::Mat tmp = mlframe / 4095.0 * 255;

			tmp.convertTo(outtmp, CV_8UC1);
		}

		mlframe = outtmp;
	}

	if (mlframe.empty())
	{
		QLog::GetInstance().qlog("CrossCenterPro:mlframe empty/n");
		return false;
	}

	Point2f center(0, 0);

	if (m_crossnumber == 1)
	{
		MLImageDetection::CrossCenter c;

		if (crossMode == CrossMode::Gaussian)
			center = c.find_centerGaussian(mlframe.clone(), true);//c.get_crossCenterLINES();
		else if (crossMode == CrossMode::HoffLine)
			center = c.get_crossCenterLINES();
		//else if (crossMode == CrossMode::GaussianEnhance)
		//	center = c.get_crossCenterGaussianEnhance();

		m_fLineAngle = c.getHorizontalLineAngle();
	}
	else if (m_crossnumber == 2)
	{
		algorithm::MLTwoCross my_cross;
		center = my_cross.GetTwoCrossCenter(mlframe.clone());
	}

	if (center.x != 0)
	{
		m_averagePointArray.push_front(center);

		if (m_averagePointArray.size() > average + 1)
		{
			m_averagePointArray.pop_back();
		}

		Point2f averagePoint;
		for (size_t i = 0; i < m_averagePointArray.size(); i++)
		{
			averagePoint.x += m_averagePointArray[i].x;
			averagePoint.y += m_averagePointArray[i].y;
		}
		averagePoint.x = averagePoint.x / m_averagePointArray.size();
		averagePoint.y = averagePoint.y / m_averagePointArray.size();

		m_currCenter = averagePoint;

	}
	else
	{
		return false;
	}
	double distance;

	distance = powf((m_basePoint.x - m_currCenter.x), 2) + powf((m_basePoint.y - m_currCenter.y), 2);

	distance = sqrtf(distance) * m_pixel;

	m_angle = atan2(distance, m_focallength) * 180.0 / CV_PI;
	
	double x = m_currCenter.x - m_basePoint.x;
	double y = m_currCenter.y - m_basePoint.y;

	m_H = x;
	m_V = y;

	m_AngleX = atan2(m_H* m_pixel, m_focallength) * 180.0 / CV_PI;
	m_AngleY = atan2(m_V* m_pixel, m_focallength) * 180.0 / CV_PI;

	return true;
}

cv::Point2f CrossCenterPro::GetCrossPiont()
{
	cv::Point2f pt;
	pt.x = m_currCenter.x;
	pt.y = m_currCenter.y;
	return pt;
}

cv::Point2f CrossCenterPro::GetBasePiont()
{
	cv::Point2f pt;
	pt.x = m_basePoint.x;
	pt.y = m_basePoint.y;
	return pt;
}

double CrossCenterPro::GetAngle()
{
	return m_angle;
}

double CrossCenterPro::GetH()
{
	return m_H;
}

double CrossCenterPro::GetV()
{
	return m_V;
}

double CrossCenterPro::GetAngleX()
{
	return m_AngleX;
}

double CrossCenterPro::GetAngleY()
{
	return m_AngleY;
}

double CrossCenterPro::GetHorizontalLineAngle()
{

	return m_fLineAngle;
}

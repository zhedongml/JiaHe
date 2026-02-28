#include "CrossCenterPro.h"
#include "CrossCenter.h"

#include<string>
using namespace std;

CrossCenterPro::CrossCenterPro()
	:focallength(94.45)
	, pixel(0.0055)
	, basePoint(1024.0, 1024.0)
	, currCenter(0.0, 0.0)
	, angle(0)
	, H(0)
	, V(0)
	, AngleX(0)
	, AngleY(0)
{
}


CrossCenterPro::~CrossCenterPro()
{

}


void CrossCenterPro::SetFocalLength(double fl)
{
	focallength = fl;
}

void CrossCenterPro::SetFramePixel(double p)
{
	pixel = p;
}

void CrossCenterPro::SetBaseCenter(int x, int y)
{
	basePoint = cv::Point2f(x, y);
}

bool CrossCenterPro::Execute(Mat mlframe, int average)
{

	if (focallength <= 0 || pixel <= 0 || mlframe.empty())
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
	CrossCenter c(mlframe.clone(), 10);
	cv::Point2f center = c.get_crossCenterLINES();

	if (center.x != 0)
	{
		m_averagePointArray.push_front(center);

		if (m_averagePointArray.size() > average + 1)
		{
			m_averagePointArray.pop_back();
		}

		cv::Point2f averagePoint;
		for (size_t i = 0; i < m_averagePointArray.size(); i++)
		{
			averagePoint.x += m_averagePointArray[i].x;
			averagePoint.y += m_averagePointArray[i].y;
		}
		averagePoint.x = averagePoint.x / m_averagePointArray.size();
		averagePoint.y = averagePoint.y / m_averagePointArray.size();

		currCenter = averagePoint;

	}
	else
	{
		return false;
	}
	double distance;

	distance = powf((basePoint.x - currCenter.x), 2) + powf((basePoint.y - currCenter.y), 2);

	distance = sqrtf(distance) * pixel;

	angle = atan2(distance, focallength) * 180.0 / PI;
	
	double x = currCenter.x - basePoint.x;
	double y = currCenter.y - basePoint.y;

	H = x;
	V = -y;

	AngleX = atan2(H* pixel, focallength) * 180.0 / PI;
	AngleY = atan2(V* pixel, focallength) * 180.0 / PI;

	return true;
}

CORE::ML_POINT2F CrossCenterPro::GetCrossPiont()
{
    CORE::ML_POINT2F pt;
	pt.x = currCenter.x;
	pt.y = currCenter.y;
	return pt;
}

double CrossCenterPro::GetAngle()
{
	return angle;
}

double CrossCenterPro::GetH()
{
	return H;
}

double CrossCenterPro::GetV()
{
	return V;
}

double CrossCenterPro::GetAngleX()
{
	return AngleX;
}

double CrossCenterPro::GetAngleY()
{
	return AngleY;
}

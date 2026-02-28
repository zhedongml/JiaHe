
#include "ML_Math.h"
#include"SlantedEdge.h"
#include "SlantedCross.h"
#include "CTF.h"
#include "CrossCenter.h"
MLMATH_BEGIN


void setPixel(double pixel)
{
	SlantedEdge::SetSlantedPixel( pixel);
	SlantedCross::SetCrossPixel ( pixel);
}

bool checkSlantROI(Mat roi, unsigned char& direction)
{
	return SlantedEdge::check_roi(roi, direction);
}

bool checkCrossROI(cv::Mat img, unsigned char &dir)
{
	return SlantedCross::check_roi(img, dir);
}

double getSmoothingSpline(double*x, double*y, int bin_len, double need_x)
{
	if ((x == NULL) || (y == NULL)) return 0;

	SplineSpace::Spline sp(x, y, bin_len, SplineSpace::GivenSecondOrder);

	double need_y;

	sp.SinglePointInterp(need_x, need_y);

	return need_y;
}

void  getCTF(unsigned char* buffer, int height, int width, int type, OUT double& frequency, OUT double& contrast)
{
	if (type == 8)
	{
		type = CV_8UC1;
	}
	else
	{
		type = CV_8UC3;
	}

	Mat img(height, width, type, buffer);
	CTF my(img);

	frequency = my.get_frequency() / SlantedEdge::pixel;
	contrast = my.get_contrast();
}

double get_point_angle(cv::Point2f pointO, cv::Point2f pointA)
{
	double angle = 0;
	cv::Point2f point;
	double temp;

	point = cv::Point2f((pointA.x - pointO.x), (pointA.y - pointO.y));

	if ((0 == point.x) && (0 == point.y))
	{
		return 0;
	}

	if (0 == point.x)
	{
		angle = 90;
		return angle;
	}

	if (0 == point.y)
	{
		angle = 0;
		return angle;
	}

	temp = fabsf(float(point.y) / float(point.x));
	temp = atan(temp);
	temp = temp * 180 / CV_PI;

	if ((0 < point.x) && (0 < point.y))
	{
		angle = 360 - temp;
		return angle;
	}

	if ((0 > point.x) && (0 < point.y))
	{
		angle = 360 - (180 - temp);
		return angle;
	}

	if ((0 < point.x) && (0 > point.y))
	{
		angle = temp;
		return angle;
	}

	if ((0 > point.x) && (0 > point.y))
	{
		angle = 180 - temp;
		return angle;
	}

	//printf("sceneDrawing :: getAngle error!");
	return -1;
}

void Smooth(double *value, int half_window, int len)
{
	SlantedEdge::smooth(value, half_window, len);
}

cv::Point2f  getCrossCenter(cv::Mat img, int size )
{
	CrossCenter c(img.clone(),10);
	cv::Point2f center = c.get_crossCenterLINES();
	return center;
}

double getDistance(cv::Point2f pointO, cv::Point2f pointA)
{
	double distance;

	distance = powf((pointO.x - pointA.x), 2) + powf((pointO.y - pointA.y), 2);

	distance = sqrtf(distance) * SlantedCross::pixelc;

	return distance;
}

vector<cv::Point2f> circleCenter(cv::Mat img)
{
	Mat img1 = img.clone();
	Mat img_gray;
	if (img.channels() == 3)//转灰度图
		cvtColor(img, img_gray, CV_BGR2GRAY);
	else
		img_gray = img;
	Mat img_th;
	Point minp, maxp;
	double minv = 0.0, maxv = 0.0;
	minMaxLoc(img_gray, &minv, &maxv, &minp, &maxp);

	threshold(img_gray, img_th, 0, 255, CV_THRESH_OTSU);
	//threshold(img_gray, img_th, (minv + maxv) / 2, 255, CV_THRESH_BINARY);

	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(img_th, img_th, element1);//膨胀，让黑块分开

	Mat img_blur;
	GaussianBlur(img_gray, img_blur, Size(9, 9), 4, 4);


	////通过canny算法找轮廓，这样 findcontours 的结果会好些
	//Mat canny_out;
	//Canny(img_gray, canny_out, 45, 100, 3, false);

	//寻找轮廓
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	//findContours(img_th, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1));

	findContours(img_th, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> contours_new;
	vector<Vec4i> hierachy_new;
	for (int i = 0; i < contours.size(); i++)
	{
		if (hierachy[i][2] != -1 && hierachy[i][3] != -1 && contourArea(contours[i], false) > 200)
		{
			contours_new.push_back(contours[i]);
			hierachy_new.push_back(hierachy[i]);
		}


	}

	for (int i = 0; i < contours_new.size(); i++)
	{

		drawContours(img, contours_new, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 3, 8);

	}


	//定义圆形、方形、旋转矩形、椭圆的存储容器
	vector<RotatedRect> ellipse_ploy;//注意，这里是画椭圆，但是容器类型是 RotatedRect

	for (size_t i = 0; i < contours_new.size(); i++)
	{
		RotatedRect temp2 = fitEllipse(contours_new[i]);
		ellipse_ploy.push_back(temp2);

	}


	vector<cv::Point2f> corner;

	for (size_t i = 0; i < ellipse_ploy.size(); i++)
	{

		corner.push_back(ellipse_ploy[i].center);

	}

	//for (size_t i = 0; i < ellipse_ploy.size(); i++)
	//{
	//	ellipse(img1, ellipse_ploy[i], Scalar(rand() % 255, rand() % 255, rand() % 255), 1, 8);
	//	//Point center(cvRound(circles2[i].circle[0]), cvRound(circles2[i].circle[1]));
	//	//int radius = cvRound(circles2[i].circle[2]);
	//	//绘制圆心
	//	//circle(img_gray, center, 3, Scalar(0, 255, 0), -1, 8, 0);
	//	//绘制圆的轮廓
	//	//circle(img, center, radius, Scalar(155, 50, 255), 3, 8, 0);

	//}


	//img1.copyTo(img1, canny_out);

	if (corner.size()>0)
	{
		cornerSubPix(img_gray, corner, cv::Size(5, 5), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 4000, 0.001));
	}
	


	return corner;
}

vector<cv::Point> getCrossAutoROI(cv::Mat img, int offset)
{
	if (img.empty())
	{
		vector<cv::Point> p;
		return p;
	}

	CrossCenter c(img.clone(), offset);

	cv::Point2f p = c.get_crossCenterLINES();

	return c.get_roi_center();
}

void getCrossAutoROI(cv::Mat img, int offset, vector<cv::Point>& vp)
{
	if (img.empty())
	{
		return;
	}

	CrossCenter c(img.clone(), offset);

	cv::Point2f p = c.get_crossCenterLINES();

	vp = c.get_roi_center();
}

double getInterp(double* x, double *y, int len, double need)
{
	vector<double> xx = vector<double>(x, x + len - 1);
	vector<double> yy = vector<double>(y, y + len - 1);

	if (len == NULL) return 0;
	SplineSpace::Spline sp(x, y, len, SplineSpace::GivenSecondOrder);
	double y_need;
	int a = 0;
	sp.SinglePointInterp(need, y_need);
	if (isinf(*y))
	{
		a = 10;
	}
	if (isnan(y_need))
	{
		vector<double> xxx = vector<double>(x, x + len - 1);
		vector<double> yyy = vector<double>(y, y + len - 1);
		a = 9;
	}
	return y_need;
}

MLMATH_END
#include "pch.h"
#include "ML_ColorUniformity.h"
using namespace MLImageDetection;
using namespace MLIQMetrics;
using namespace cv;

MLIQMetrics::MLColorUniformity::MLColorUniformity()
{
}

MLIQMetrics::MLColorUniformity::~MLColorUniformity()
{
}

ColorUniformityRe MLIQMetrics::MLColorUniformity::getColorUniformity(cv::Mat x, cv::Mat y)
{
	ColorUniformityRe re;
	return ColorUniformityRe();
}

ColorUniformityRe MLIQMetrics::MLColorUniformity::getColorUniformity(cv::Mat X, cv::Mat Y, cv::Mat Z)
{
	string info = "---getColorUniformity---";
	ColorUniformityRe re;
	if (X.empty() || Y.empty() || Z.empty())
	{
		re.flag = false;
		re.errMsg = info + "The input images are NULL";
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(X.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	X = GetROIMat(X, ROIRect);
	Y = GetROIMat(Y, ROIRect);
	Z = GetROIMat(Z, ROIRect);
	cv::Mat img8 = convertToUint8(Y);
	cv::Mat imgdraw = convertTo3Channels(img8);
	//map<string, cv::Point2f>cenMap = get13RectCenterSort(img8,imgdraw);
	vector<cv::Point2f>cenVec = get13RectCenter(img8, imgdraw, binNum);
	cv::Mat x = (X) / (X + Y + Z);
	cv::Mat y = (Y) / (X + Y + Z);
	vector<double>xVec;
	vector<double>yVec;
	// ROI set need to add to config
	int len = 200;
	for (int i = 0; i < cenVec.size(); i++)
	{
		cv::Rect rect0(cenVec[i].x - len / 2, cenVec[i].y - len / 2, len, len);
		double x0 = cv::mean(x(rect0).clone())(0);
		double y0 = cv::mean(y(rect0).clone())(0);
		xVec.push_back(x0);
		yVec.push_back(y0);
		String str = "(" + numToString(x0) + "," + numToString(y0) + ")";
		putTextOnImage(imgdraw, str, rect0.tl());
	}
	double xmax = *max_element(xVec.begin(), xVec.end());
	double xmin = *min_element(xVec.begin(), xVec.end());
	double ymax = *max_element(yVec.begin(), yVec.end());
	double ymin = *min_element(yVec.begin(), yVec.end());
	double deltx = xmax - xmin;
	double delty = ymax - ymin;
	re.deltx = deltx;
	re.delty = delty;
	re.imgdraw = imgdraw;
	return re;
}

map<string, cv::Point2f> MLIQMetrics::MLColorUniformity::get13RectCenterSort(cv::Mat img, cv::Mat& imgdraw)
{
	return map<string, cv::Point2f>();
}

vector<cv::Point2f> MLIQMetrics::MLColorUniformity::get13RectCenter(cv::Mat img, cv::Mat& imgdraw, int binNum)
{
	cv::GaussianBlur(img, img, cv::Size(5, 5), 0, 0);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10), Point(-1, -1));
	morphologyEx(img, img, MORPH_GRADIENT, kernel, Point(-1, -1));
	cv::Mat imgth;
	//cv::threshold(img, imgth, 0, 255, THRESH_OTSU);
	cv::threshold(img, imgth, 0, 255, THRESH_TRIANGLE);
	//cv::adaptiveThreshold(img, imgth, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 21, 0);
	//morphologyEx(img, img, MORPH_OPEN, kernel, Point(-1, -1));

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(imgth, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	vector<double>xVec, yVec;
	vector<cv::Point2f>cenVec;
	double angle;
	for (int i = 0; i < contours.size(); ++i)
	{
		double area = contourArea(contours[i], false);
		cv::Rect rect = cv::boundingRect(contours[i]);
		double maxLen = max(rect.width, rect.height);
		double minLen = min(rect.width, rect.height);
		double ratio = minLen / maxLen;

		if (rect.area() > 9e4 /*&& ratio > 0.8 && ratio < 1.2*/)
		{
			cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
			// center = rectR.center;
			cv::rectangle(imgdraw, rect, Scalar(255, 0, 255), 1);
			cv::circle(imgdraw, rectR.center, 10, Scalar(0, 0, 255), -1);
			cenVec.push_back(rectR.center);
			angle = rectR.angle;
		}
	}
	if (cenVec.size() != 9)
		return vector<cv::Point2f>();

	cv::Point2f cen0;
	double xsum = 0.0, ysum = 0.0;
	for (int i = 0; i < cenVec.size(); i++)
	{
		xsum = xsum + cenVec[i].x;
		ysum = ysum + cenVec[i].y;
	}
	cen0.x = xsum / cenVec.size();
	cen0.y = ysum / cenVec.size();
	circle(imgdraw, cen0, 10, Scalar(0, 255, 0), -1);

	int index = findIndex(cenVec, cen0, 200);
	if (index < 0 || index >= cenVec.size())
		return  vector<cv::Point2f>();
	cv::Point2f p5 = cenVec[index];
	vector<cv::Point2f> ptsVec;
	for (int i = 0; i < xpixel.size(); i++)
	{
		ptsVec.push_back(p5 + Point2f(xpixel[i] / binNum, ypixel[i] / binNum));
	}
	drawPointsOnImage(imgdraw, ptsVec, 20);
	vector<cv::Point2f> ptsR = rotatePoints(ptsVec, p5, -angle);
	drawPointsOnImage(imgdraw, ptsVec, 20, Scalar(0, 255, 0));
	return ptsR;
}

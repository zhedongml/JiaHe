#include "FiducialDetect.h"
#include <opencv2/imgproc/types_c.h>
#include"ml_matchTemplate.h"
#include"CrossCenter.h"

using namespace MLImageDetection;
using namespace cv;
MLImageDetection::FiducialDetect::FiducialDetect()
{
}
MLImageDetection::FiducialDetect::~FiducialDetect()
{
}
FiducialRe MLImageDetection::FiducialDetect::getFiducialCoordinate(cv::Mat img, cv::Rect rect)
{
	FiducialRe re;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = "Input image is null";
		return re;
	}
	cv::Mat roi = GetROIMat(img, rect);
	cv::Mat gray = convertToGrayImage(roi);
	cv::Mat imgdraw = convertTo3Channels(gray);
	cv::Point2f loc = getFiducialCoordinateByContour(gray, imgdraw);
	if (loc.x < 1e-6 || loc.y < 1e-6)
	   loc = getFiducialCoordinateByHough(gray, imgdraw);
	if (loc.x < 1e-6 || loc.y < 1e-6)
	{
		re.flag = false;
		re.errMsg = "Fiducial Detection fail";
		return re;
	}
	re.loc = loc;
	re.imgdraw = imgdraw;

	return re;
}
std::vector<cv::Point2f> MLImageDetection::FiducialDetect::FiducialCoordinate(cv::Mat img, int flag)
{
	std::vector<cv::Point2f> center;
	//if (flag == 1)
	//{
	//    MLCircleFiducialDetect circledetect;
	//    center = circledetect.CircleFiducialDetect(img);
	//    m_img_draw = circledetect.getImgDraw();
	//}
	//else if (flag == 2)
	//{
	//    MLLinesFiducialDetect linesdetect;
	//    center = linesdetect.LineFiducialDetect(img);
	//    m_img_draw = linesdetect.getImgDraw();
	//}
	//else if (flag == 3)
	//{
	//    MLBMWFiducialDetect BMWdetect;
	//    center = BMWdetect.BMWFiducialDetect(img);
	//    m_img_draw = BMWdetect.getImgDraw();
	//}
	//else if (flag == 4)
	//{
	//    MLRectangleFiducialDetect rectangledetect;
	//    center = rectangledetect.RectangleFiducialDetect(img);
	//    m_img_draw = rectangledetect.getImgDraw();
	//}
	//else if (flag==5)//for the PICO fiducial
	//{

	//    ALGResult re= FiducialContour(img, center);
	//    if (!re.success)
	//    {
	//        cv::Mat templ;
	//        templ = cv::imread("./config/ALGConfig/picoTemplate.tif", 0);
	//        if (!templ.data)
	//            templ = cv::imread("../../app/config/ALGConfig/picoTemplate.tif", 0);
	//        MatchTemplate mt;
	//        re = mt.getMatchTemplateResult(img, templ, 80, center);
	//        m_img_draw = mt.getImgDraw();
	//    }  

	//}
	return center;
}
ALGResult MLImageDetection::FiducialDetect::FiducialCoordinate(cv::Mat img, FiducialType ftype,
	std::vector<cv::Point2f>& center)
{
	//  std::vector<cv::Point2f> center;
	ALGResult re;
	switch (ftype)
	{
	case CIRCLE: {

		cv::Mat templ;
		if (DEBUG == 1)
		{
			templ = cv::imread("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\ALGConfig\\circletemplate1.tiff");
		}
		else
		{
			templ = cv::imread("./config/ALGConfig/circletemplate1.tiff", 0);
		}
		MatchTemplate mt;
		re = mt.getMatchTemplateResult(img, templ, 90, center);
		for (int i = 0; i < center.size(); i++)
		{
			center[i] = center[i] + cv::Point2f(260, 135);
		}
		break;
	}
	case CROSSHAIR: {

		cv::Mat templ;
		if (DEBUG == 1)
		{
			templ =
				cv::imread("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\ALGConfig\\crosshairTemplate.tiff");
		}
		else
		{
			templ = cv::imread("./config/ALGConfig/crosshairTemplate.tiff", 0);
		}
		MatchTemplate mt;
		re = mt.getMatchTemplateResult(img, templ, 90, center);
		for (int i = 0; i < center.size(); i++)
		{
			center[i] = center[i] + cv::Point2f(62, 61);
		}
		break;
	}
	case BMW: {
		// TODO:test
		cv::Mat templ;
		if (DEBUG == 1)
		{
			templ =
				cv::imread("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\ALGConfig\\crosshairTemplate.tiff");
		}
		else
		{
			templ = cv::imread("./config/ALGConfig/crosshairTemplate.tiff", 0);
		}
		MatchTemplate mt;
		re = mt.getMatchTemplateResult(img, templ, 90, center);
		for (int i = 0; i < center.size(); i++)
		{
			center[i] = center[i];
		}
		break;
	}
	default:
		break;
	}
	m_img_draw = img.clone();
	for (int i = 0; i < center.size(); i++)
	{
		circle(m_img_draw, center[i], 10, Scalar(0, 0, 255), -1);
	}
	return re;
}
ALGResult MLImageDetection::FiducialDetect::FiducialContour(cv::Mat img, vector<cv::Point2f>& loc)
{
	ALGResult re;
	re.success = false;
	if (img.data)
	{
		cv::Mat img_draw;
		cv::Mat gray;
		if (img.channels() == 3)
		{
			cvtColor(img, gray, COLOR_BGR2GRAY);
			img_draw = img.clone();
		}
		else
		{
			gray = img.clone();
			cvtColor(img, img_draw, COLOR_GRAY2BGR);
		}

		cv::GaussianBlur(gray, gray, Size(3, 3), 0, 0);
		//  cv::equalizeHist(gray, gray);
		cv::Mat element = cv::getStructuringElement(MORPH_RECT, Size(5, 5));
		cv::Mat element1 = cv::getStructuringElement(MORPH_RECT, Size(10, 10));

		// cv::morphologyEx(gray, gray, MORPH_DILATE, element);

		cv::morphologyEx(gray, gray, MORPH_GRADIENT, element1);
		cv::Mat imgth;
		cv::threshold(gray, imgth, 0, 255, THRESH_TRIANGLE);
		NaiveRemoveNoise(imgth, 1000);
		//Clear_MicroConnected_Areas(imgth, imgth, 1000);
		ContoursRemoveNoise(imgth, 1000);
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(imgth, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		//   drawContours(img_draw, contours, -1, Scalar(0, 0, 255), 2);

		for (int i = 0; i < contours.size(); i++)
		{
			cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
			cv::Size s = rectR.size;
			cv::Rect rect = boundingRect(contours[i]);
			double area = rectR.size.area(); // cv::contourArea(contours[i]);
		 //   cv::Size s = rectR.size;
			double ratio = double(min(s.width, s.height)) / double(max(s.width, s.height));
			cv::drawContours(img_draw, contours, i, cv::Scalar(0, 0, 255), 5);
			double A = rect.area();
			//cv::contourArea(contours[i]);
			double d; //= cv::arcLength(contours[i],true);
			Point2f c0;
			float r;
			cv::minEnclosingCircle(contours[i], c0, r);
			d = 2 * CV_PI * r;
			double roundness = (4 * CV_PI * A) / (d * d);

			if (area > 2.5e4 & area < 5e4 && ratio>0.75 & ratio < 1.2 && roundness>0.6)
			{
				// cout << area << "," << roundness << endl;
				drawContours(img_draw, contours, i, Scalar(0, 255, 0), 5);
				cv::Point2f center;
				cv::RotatedRect reRo = cv::minAreaRect(contours[i]);
				center = reRo.center;
				//std::cout << center << endl;
				cv::circle(img_draw, center, 5, Scalar(0, 0, 255), -1);
				loc.push_back(center);
				re.success = true;

				/*      float r;
					  cv::minEnclosingCircle(contours[i], center, r);
					  cout << center << endl;
					  cv::circle(img_draw, center, 1, Scalar(0, 255, 0), -1);*/

			}

		}
		m_img_draw = img_draw.clone();
	}
	else
	{
		re.success = false;
		re.errorMsg = "The Input image is NULL";

	}

	return re;
}
cv::Point2f MLImageDetection::FiducialDetect::getFiducialCoordinateByContour(cv::Mat gray, cv::Mat& imgdraw)
{
	cv::Point2f loc;
	cv::Mat blur;
	cv::GaussianBlur(gray, blur, Size(3, 3), 0, 0);
	//  cv::equalizeHist(gray, gray);
	cv::Mat element = cv::getStructuringElement(MORPH_RECT, Size(5, 1));
	cv::Mat element1 = cv::getStructuringElement(MORPH_RECT, Size(3, 3));
	// cv::morphologyEx(gray, gray, MORPH_DILATE, element);
	cv::Mat grad;
	cv::morphologyEx(blur, grad, MORPH_GRADIENT, element1);
	cv::Mat imgth;
	cv::threshold(grad, imgth, 0, 255, THRESH_OTSU);
	cv::morphologyEx(imgth, imgth, MORPH_CLOSE, element);

	NaiveRemoveNoise(imgth, 500);
	//Clear_MicroConnected_Areas(imgth, imgth, 1000);
	ContoursRemoveNoise(imgth, 500);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(imgth, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	//   drawContours(img_draw, contours, -1, Scalar(0, 0, 255), 2);
	float r;
	for (int i = 0; i < contours.size(); i++)
	{
		cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
		cv::Size s = rectR.size;
		cv::Rect rect = boundingRect(contours[i]);
		double area = rectR.size.area(); // cv::contourArea(contours[i]);
	 //   cv::Size s = rectR.size;
		double ratio = double(min(s.width, s.height)) / double(max(s.width, s.height));
		cv::drawContours(imgdraw, contours, i, cv::Scalar(0, 0, 255), 1);
		double A = rect.area();
		//cv::contourArea(contours[i]);
		double d; //= cv::arcLength(contours[i],true);
		Point2f c0;
		//float r;
		cv::minEnclosingCircle(contours[i], c0, r);
		d = 2 * CV_PI * r;
		double roundness = (4 * CV_PI * A) / (d * d);
		if (area > 1e4 & area < 5e4 && ratio>0.75 & ratio < 1.2 && roundness>0.6 && r > 50 && r < 70)
		{
			// cout << area << "," << roundness << endl;
			drawContours(imgdraw, contours, i, Scalar(0, 255, 0), 1);
			cv::Point2f center;
			cv::RotatedRect reRo = cv::minAreaRect(contours[i]);
			center = reRo.center;
			//std::cout << center << endl;
			//center = getExactLocation(gray, center);
			double r;
			cv::circle(imgdraw, center, 1, Scalar(0, 255, 0), -1);
			circleLeastFit(contours[i], center, r);
			cv::circle(imgdraw, center, 1, Scalar(0, 0, 255), -1);
			cv::circle(imgdraw, center, r, Scalar(0, 0, 255), 1);
			loc = center;
			break;
		}

	}
	//cv::Vec3f cir(loc.x, loc.y, r);
	//cv::Mat sx, sy;
	//cv::Sobel(blur, sx, CV_32FC1, 1, 0, 3);
	//cv::Sobel(blur, sy, CV_32FC1, 0, 1, 3);
	//cv::Mat mag = sx.mul(sx) + sy.mul(sy);
	//cv::sqrt(mag, mag);
	//vector<cv::Point2f>fitPts = findCircleExactPoints(blur, 2, cir, imgdraw);
	//cv::Point2f c2;
	//double r1;
	//circleLeastFit(fitPts, c2, r1);
	//cv::circle(imgdraw, c2, r1, Scalar(0, 255, 0), 1);
	//cv::circle(imgdraw, c2, 2, Scalar(0, 255, 0), -1);
	//loc = c2;
	return loc;
}
cv::Point2f MLImageDetection::FiducialDetect::getFiducialCoordinateByHough(cv::Mat gray, cv::Mat& imgdraw)
{

	cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, cv::Size(5, 5));
	cv::Mat grad;
	cv::morphologyEx(gray, grad, MORPH_GRADIENT, kernel);
	cv::Mat imgth;
	cv::threshold(grad, imgth, 0, 255, THRESH_OTSU);
	cv::Mat can;
	cv::Canny(imgth, can, 30, 90);
	cv::Point2f loc;
	vector<cv::Vec3f>circles;
	cv::HoughCircles(can, circles, HOUGH_GRADIENT, 2, 100, 30, 90, 50, 80);
	if (circles.size() < 0)
		return cv::Point2f(0, 0);
	double r;
	for (int i = 0; i < circles.size(); i++)
	{
		cv::Point2f c0(circles[i][0], circles[i][1]);
		 r = circles[i][2];
		if (r > 50 & r < 70)
		{
			loc = c0;
			circle(imgdraw, c0, r, Scalar(0, 0, 255), 1);
			circle(imgdraw, c0, 1, Scalar(0, 255, 0), -1);
			break;
		}
	}	
	return loc;
}
cv::Point2f MLImageDetection::FiducialDetect::getFiducialCoordinateByTemplate(cv::Mat gray, cv::Mat& imgdraw)
{
	// string templatePath = "./config/ALGConfig/fiducial/temple.tif";
	string templatePath = "E:\\project\\JiAn\\src\\RealityQ+\\config\\ALGConfig\\fiducial\\temple.tif";
	cv::Mat templ = cv::imread(templatePath, 0);
	if (templ.empty())
		return cv::Point(0, 0);
	vector<cv::Point2f>locVec;
	MatchTemplate match;
	match.getMatchTemplateResult(gray, templ, 50, locVec);
	for (int i = 0; i < locVec.size(); i++)
	{
		circle(imgdraw, locVec[i], 1, Scalar(255, 0, 255), -1);
	}
	if (locVec.size() > 0)
		return locVec[0];
	else
		return cv::Point2f(0, 0);
}
cv::Point2f MLImageDetection::FiducialDetect::getExactLocation(cv::Mat gray, cv::Point2f c0)
{
	int len = 60;
	cv::Rect rect(c0.x - len / 2, c0.y - len / 2, len, len);
	cv::Mat roi = gray(rect).clone();
	CrossCenter cc;
	cc.setROILength(10);
	cv::bitwise_not(roi, roi);
	cv::Point2f center = cc.find_centerGaussian(roi);
	if (center.x > 1e-6 || center.y > 1e-6)
		return center + cv::Point2f(rect.tl());
	else
		return center;

	return cv::Point2f();
}
int findMaxMag(cv::Mat mag, arma::vec xvec, arma::vec yvec)
{
	mag.convertTo(mag, CV_32FC1);
	vector<float>magvec;
	for (int i = 0; i < xvec.size(); i++)
	{
		int ix = round(xvec[i]);
		int iy = round(yvec[i]);
		//circle(mag, cv::Point(ix,iy+3), 1, Scalar(255, 255, 0), -1);
		float mag0 = mag.at<float>(iy, ix);
		magvec.push_back(mag0);
	}
	int index = min_element(magvec.begin(), magvec.end()) - magvec.begin();
	return index;
}

vector<cv::Point2f> MLImageDetection::FiducialDetect::findCircleExactPoints(cv::Mat mag, int step, cv::Vec3f cir, cv::Mat& imgdraw)
{
	double r = cir[2];
	double xc = cir[0];
	double yc = cir[1];
	int len = 20;
	double gap = 1;
	vector<cv::Point2f>fitPts;
	for (int i = 0; i < 360; i = i + step)
	{
		double theta = i / 180.0 * CV_PI;
		if (cos(theta) == 0)
		{
			if (i == 280)
				r = -r;
			double dis = len * cos(0);
			double left = yc + r * cos(0) - dis;
			double right = yc + r * cos(0) + dis;
			double start = min(left, right);
			double end = max(left, right);
			double maxrow = mag.rows;
			arma::vec yvec = arma::regspace(max(start, 0.0), gap, min(end, maxrow));
			if (yvec.size() < 100)
				continue;
			double k = sin(0) / cos(0);
			double b = xc - yc * k;
			arma::vec p = { k,b };
			arma::vec xvec = arma::polyval(p, yvec);
			int index = findMaxMag(mag, xvec, yvec);
			cv::Point2f c0(xvec(index), yvec(index));
			circle(imgdraw, c0, 3, Scalar(255, 0, 255), -1);
			fitPts.push_back(c0);
		}
		else
		{
			double dis = len * cos(theta);
			double left = xc + r * cos(theta) - dis;
			double right = xc + r * cos(theta) + dis;
			double start = min(left, right);
			double end = max(left, right);
			double maxrow = mag.cols - 1;
			arma::vec xvec = arma::regspace(max(start, 0.0), gap, min(end, maxrow));
			if (xvec.size() < len)
				continue;
			double k = sin(theta) / cos(theta);
			double b = yc - k * xc;
			arma::vec p{ k, b };
			arma::vec yvec = arma::polyval(p, xvec);
			double miny = arma::min(yvec);
			double maxy = arma::max(yvec);
			arma::uvec indexvec = arma::find(yvec < mag.rows&& yvec>0);
			if (indexvec.size() < len)
				continue;
			arma::vec xvec1 = xvec(indexvec);
			arma::vec yvec1 = yvec(indexvec);
			int index = findMaxMag(mag, xvec1, yvec1);
			cv::Point2f c0(xvec1(index), yvec1(index));
			/*	for (int k = 0; k < xvec.size(); k++)
				{
					cv::Point2f c1(xvec[k], yvec[k]);
			  circle(imgdraw, c1+cv::Point2f(0,3), 1, Scalar(255, 255, 0), -1);

				}*/
			circle(imgdraw, c0, 1, Scalar(255, 0, 255), -1);
			fitPts.push_back(c0);

		}

	}
	return fitPts;

}
cv::Mat MLImageDetection::FiducialDetect::getImgDraw()
{
	return m_img_draw;
}
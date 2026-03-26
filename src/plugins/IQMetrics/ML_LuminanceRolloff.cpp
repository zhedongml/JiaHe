#include "pch.h"
#include "ML_LuminanceRolloff.h"
#include "ML_Efficiency.h"
#include"ml_rectangleDetection.h"

using namespace MLImageDetection;
using namespace MLIQMetrics;
using namespace cv;
MLIQMetrics::MLLuminanceRolloff::MLLuminanceRolloff()
{
}
MLIQMetrics::MLLuminanceRolloff::~MLLuminanceRolloff()
{
}
void MLIQMetrics::MLLuminanceRolloff::setIsSLB(bool flag)
{
	m_isSLB = flag;
}
void MLIQMetrics::MLLuminanceRolloff::setIsDisparityEyebox(bool flag)
{
	m_isDisparityEyebox = flag;
}
RolloffRe MLIQMetrics::MLLuminanceRolloff::getRelativeBrightness(cv::Mat imgRaw)  // --------this--------
{
	RolloffRe rollRe;
	if (imgRaw.empty())
	{
		rollRe.flag = false;
		rollRe.errMsg = "Input image is NULL";
		return rollRe;
	}
	cv::Mat img = IQMetricUtl::instance()->getRotationAndFlipImg(imgRaw, m_isSLB);
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	cv::Mat roi = GetROIMat(img, ROIRect);
	Mat img8 = convertToUint8(roi);
	cv::Mat img_draw = convertTo3Channels(img8);
	double m_FocalLength = IQMetricsParameters::FocalLength;
	double m_pixel_size = IQMetricsParameters::pixel_size * binNum;
	Point2f center;
	cv::Rect rectBound;
	//MLEfficiency eff;
//	cv::RotatedRect rectR = eff.getSolidBorder(img8, rectBound);
	RectangleDetection rd;
	cv::RotatedRect rectR = rd.getRectangleBorder(img8); // 外接旋转矩形
	//if (rectR.size.area() < 1e6 || rectR.size.area() > 2.25e6)
	if (rectR.size.area() < 1e6 || rectR.size.area() > 5.85e6)
	{
		rd.readSolidInfoFromCSV(rectR);
	}
	else
	{
		rd.writeSolidInfoToCSV(rectR);
	}

	//if (rectR.boundingRect().area() > 16e5 && rectR.boundingRect().area() < 40e5)
	//{
	//	cv::Mat dutInfo(cv::Mat::zeros(cv::Size(5, 1), CV_32FC1));
	//	string filePath = "./config/templateImg/solidInfo.csv";
	//	dutInfo.at<float>(0, 0) = rectR.angle;
	//	dutInfo.at<float>(0, 1) = rectR.center.x;
	//	dutInfo.at<float>(0, 2) = rectR.center.y;
	//	dutInfo.at<float>(0, 3) = rectR.size.width;
	//	dutInfo.at<float>(0, 4) = rectR.size.height;
	//	writeMatTOCSV(filePath, dutInfo);
	//}
	//else if (rectR.boundingRect().area() < 16e5 )
	//{
	//	cv::Mat dutInfo(cv::Mat::zeros(cv::Size(5, 1), CV_32FC1));
	//	string filePath = "./config/templateImg/solidInfo.csv";
	//	dutInfo = readCSVToMat(filePath);
	//	rectR.angle = dutInfo.at<float>(0, 0);
	//	rectR.center.x = dutInfo.at<float>(0, 1);
	//	rectR.center.y = dutInfo.at<float>(0, 2);
	//	rectR.size.width = dutInfo.at<float>(0, 3);
	//	rectR.size.height = dutInfo.at<float>(0, 4);
	//}
	cv::Rect rectAfRo;
	cv::Point2f center0((float)(roi.cols / 2), (float)(roi.rows / 2));
	updateRotateImg(roi, rectR.angle);
	updateRotateImg(img_draw, rectR.angle);
	rectAfRo = updateRotateRect(rectR, center0);
	rectAfRo = rd.getSolidExactRect(img8, rectAfRo);  // 精确边界rect

	center = Point2f(rectAfRo.tl()) + cv::Point2f(rectAfRo.width / 2.0, rectAfRo.height / 2.0);
	if (center.x != 0 && center.y != 0)
	{
		cv::circle(img_draw, center, 2, Scalar(255, 0, 255), -1);
		//drawRectOnImage(img_draw, rectAfRo);
		double ratio = IQMetricsParameters::RolloffAreaRatio;
		cv::Rect rectRatio = updateRectByRatio(rectAfRo, sqrt(ratio));
		//drawRectOnImage(img_draw, rectRatio);
		rectangle(img_draw, rectRatio, Scalar(0, 255, 0), 4 / binNum);
		cv::Mat lumiROI = roi(rectRatio).clone();  //90% fov roi
		double p50 = percentile(lumiROI, 50); // 求将所有像素按从小到大排列 中位数的亮度
		double p5 = percentile(lumiROI, 5); // 5%位置的亮度值
		double p95 = percentile(lumiROI, 95);
		string strh = "P5:" + to_string(p5);
		string strv = "P95:" + to_string(p95);
		string strv1 = "P50:" + to_string(p50);
		putTextOnImage(img_draw, strh, center, 20 / binNum);
		putTextOnImage(img_draw, strv1, center + cv::Point2f(0, 300 / binNum), 20 / binNum);
		putTextOnImage(img_draw, strv, center + cv::Point2f(0, 600 / binNum), 20 / binNum);

		rollRe.p50 = p50;
		rollRe.p5 = p5;
		rollRe.p95 = p95;
		rollRe.ralativeBrightnessLow = p5 / p95;
		rollRe.ralativeBrightnessHigh = p95 / p5;
		rollRe.imgdraw = img_draw;
	}
	return rollRe;
}
RolloffRe_NotUsed MLIQMetrics::MLLuminanceRolloff::LuminanceRolloffRotation(const cv::Mat img)
{
	RolloffRe_NotUsed rollRe;
	std::vector<double> re;
	double H_Rolloff = 0;
	double V_Rolloff = 0;
	if (img.empty())
	{
		rollRe.flag = false;
		rollRe.errMsg = "Input image is NULL";
		return rollRe;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	cv::Mat roi = GetROIMat(img, ROIRect);
	Mat img8 = convertToUint8(roi);
	cv::Mat img_draw = convertTo3Channels(img8);
	double m_FocalLength = IQMetricsParameters::FocalLength;
	double m_pixel_size = IQMetricsParameters::pixel_size * binNum;
	double RolloffLength = IQMetricsParameters::RolloffLength; 
	double RolloffLengthH = IQMetricsParameters::RolloffLengthH;
	double RolloffLengthV = IQMetricsParameters::RolloffLengthV;
	double dis = m_FocalLength * tan(RolloffLength * CV_PI / 180) / m_pixel_size * 2;
	double disH = m_FocalLength * tan(RolloffLengthH * CV_PI / 180) / m_pixel_size * 2;
	double disV = m_FocalLength * tan(RolloffLengthV * CV_PI / 180) / m_pixel_size * 2;
	Point2f center;
	cv::Rect rectBound;
	MLEfficiency eff;
	cv::RotatedRect rectR = eff.getSolidBorder(img8, rectBound);
	cv::Rect rectAfRo;
	cv::Point2f center0((float)(roi.cols / 2), (float)(roi.rows / 2));
	updateRotateImg(roi, rectR.angle);
	updateRotateImg(img_draw, rectR.angle);
	rectAfRo = updateRotateRect(rectR, center0);
	center = Point2f(rectAfRo.tl()) + cv::Point2f(rectAfRo.width / 2.0, rectAfRo.height / 2.0);
	if (center.x != 0 && center.y != 0)
	{
		cv::circle(img_draw, center, 2, Scalar(255, 0, 255), -1);
		double RolloffWidth = IQMetricsParameters::RolloffWidth;
		cv::Rect rectH, rectV;
		rectV.x = (round(center.x) - RolloffWidth / 2);
		rectV.y = (round(center.y) - round(disV / 2.0));
		rectV.width = RolloffWidth;
		rectV.height = disV;
		rectH.x = round(center.x) - round(disH / 2.0);
		rectH.y = (round(center.y) - RolloffWidth / 2.0);
		rectH.width = disH;
		rectH.height = RolloffWidth;
		cv::rectangle(img_draw, rectH, Scalar(255, 0, 255), -1);
		cv::rectangle(img_draw, rectV, Scalar(255, 0, 255), -1);
		roi.convertTo(roi, CV_32FC1);
		cv::Mat matH = roi(rectH).clone();
		cv::Mat matV = roi(rectV).clone();
		cv::Mat hM, vM;
		cv::reduce(matH, hM, 0, REDUCE_AVG);
		cv::reduce(matV, vM, 1, REDUCE_AVG);
		double minVal = min(hM.at<float>(0, 0), hM.at<float>(0, hM.total() - 1));
		double cenVal = hM.at<float>(0, (hM.total() / 2));
		H_Rolloff = minVal / cenVal * 100;
		minVal = min(vM.at<float>(0, 0), vM.at<float>(vM.total() - 1, 0));
		cenVal = vM.at<float>((vM.total() / 2), 0);
		V_Rolloff = minVal / cenVal * 100;
		string strh = "RollOffH:" + to_string(H_Rolloff) + "%";
		string strv = "RollOffV:" + to_string(V_Rolloff) + "%";
		putTextOnImage(img_draw, strh, center);
		putTextOnImage(img_draw, strv, center + cv::Point2f(0, 300 / binNum));
		rollRe.imgdraw = img_draw;
	}
	re.push_back(H_Rolloff);
	re.push_back(V_Rolloff);
	rollRe.rollOffH = H_Rolloff;
	rollRe.rollOffV = V_Rolloff;
	return rollRe;
}

void MLIQMetrics::MLLuminanceRolloff::calculateRollOff(cv::Mat& imgdraw, cv::Rect rect, cv::Mat img, double& p5, double& p50, double& p95,int binNum)
{
	rectangle(imgdraw, rect, Scalar(0, 255, 0), 5);
	cv::Mat lumiROI = img(rect).clone();
	 p50 = percentile(lumiROI, 50);
	 p5 = percentile(lumiROI, 5);
	 p95 = percentile(lumiROI, 95);
	string strh = "P5:" + to_string(p5);
	string strv = "P95:" + to_string(p95);
	string strv1 = "P50:" + to_string(p50);
	putTextOnImage(imgdraw, strh, rect.tl(), 20 / binNum);
	putTextOnImage(imgdraw, strv, rect.tl() + cv::Point(0, 300 / binNum), 20 / binNum);
	putTextOnImage(imgdraw, strv1, rect.tl() + cv::Point(0, 600 / binNum), 20 / binNum);
}

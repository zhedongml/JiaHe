#include "pch.h"
#include "ML_Luminance.h"
#include"ML_Efficiency.h"
#include"ml_rectangleDetection.h"
#include<armadillo>
#include"LogPlus.h"
#include"MLSolidDetection.h"
using namespace cv;
using namespace MLIQMetrics;
using namespace MLImageDetection;

MLIQMetrics::MLLuminance::MLLuminance()
{
}

MLIQMetrics::MLLuminance::~MLLuminance()
{
}


void MLIQMetrics::MLLuminance::setColor(string color)
{
	m_color = color;
}

void MLIQMetrics::MLLuminance::setIsSLB(bool flag)
{
	m_isSLB = flag;
}

void MLIQMetrics::MLLuminance::setFOVType(FOVTYPE type)
{
	m_fovType = type;
}

LuminanceGuSuRe MLIQMetrics::MLLuminance::getLuminanceGusu(cv::Mat img)
{
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Solid Luminance calculation start");
	LuminanceGuSuRe re;
	string info = "------getLuminance------";
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "The input image is null!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		return re;
	}
	cv::Rect rect;
	int resizeNum = 4/binNum;
	cv::Mat imgRe;
	cv::resize(img8, imgRe, img8.size() / resizeNum);
	RectangleDetection rd;
	cv::RotatedRect rectR = rd.getRectangleBorder(imgRe);
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Solid Border detection successfully");
	cv::Rect rectRAfterRotation;
	cv::Point2f center((float)(imgRe.cols / 2), (float)(imgRe.rows / 2));
	rectRAfterRotation = updateRotateRect(rectR, center);
	rectRAfterRotation = rd.getSolidExactRect(imgRe, rectRAfterRotation);
	updateRectByRatio1(rectRAfterRotation, resizeNum);
	updateRotateImg(imgdraw, rectR.angle);
	updateRotateImg(img, rectR.angle);
	updateRotateImg(img8, rectR.angle);
	double zoneAR = IQMetricsParameters::zoneARadius;
	double pixel = IQMetricsParameters::pixel_size*binNum;
	double focallength = IQMetricsParameters::FocalLength;
	int radius = tan(zoneAR / 180 * CV_PI)*focallength/pixel;
	cv::Mat mask=getZoneAMask(rectRAfterRotation.size(),radius);
	cv::Mat roi = img(rectRAfterRotation);
	double meanA = cv::mean(roi, mask)(0);
	cv::Mat maskB;
	cv::bitwise_not(mask, maskB);
	double meanB = cv::mean(roi, maskB)(0);
	re.mean_zoneA = meanA;
	re.mean_zoneB = meanB;
	cv::Point2f cen=getRectCenter(rectRAfterRotation);
	drawRectOnImage(imgdraw, rectRAfterRotation);
	circle(imgdraw, cen, radius, Scalar(0, 0, 255), 24 / binNum);
	putTextOnImage(imgdraw, "meanA:" + numToString(meanA), cen, 24 / binNum);
	putTextOnImage(imgdraw, "meanB:" + to_string(meanB), cen+cv::Point2f(0, 300 / binNum), 24 / binNum);
	re.imgdraw = imgdraw;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Solid Luminance calculation successfully");
	return re;

}

LuminanceRe MLIQMetrics::MLLuminance::getLuminance(cv::Mat img)
{
	LuminanceRe re;
	string info = "------getLuminance------";
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "The input image is null!";
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	cv::Rect rectROI = IQMetricsParameters::ROIRect;
	cv::Mat imgROI = GetROIMat(img, rectROI);
	cv::Mat img8 = convertToUint8(imgROI);
	cv::Mat imgdraw = convertTo3Channels(img8);
	cv::Rect rect;
	MLSolidDetection solid;
	solid.setBinNum(binNum);
	solid.setFOVType(m_fovType);
	SolidDetectionRe solidRe=solid.getSolidLocation(img8);
	cv::Point2f center((float)(img.cols / 2), (float)(img.rows / 2));
	updateRotateImg(imgdraw, solidRe.rotationAngle);
	updateRotateImg(img8, solidRe.rotationAngle);
	updateRotateImg(imgROI, solidRe.rotationAngle);
	cv::Rect rectRAfterRotation = solid.getSolidExactRect(img8, solidRe.rectAf);
	float ratio = IQMetricsParameters::LuminaceActive;
	cv::Rect ROIrect = updateRectByRatio(rectRAfterRotation, ratio);
	cv::Mat roi = imgROI(ROIrect).clone();
	cv::Scalar m0, std;
	cv::meanStdDev(roi, m0, std);
	re.rectMean = m0(0);
	re.rectCov = std(0) / m0(0);
	// drawRectOnImage(imgdraw, rectRAfterRotation,4);
	drawRectOnImage(imgdraw, ROIrect);
	putTextOnImage(imgdraw, "rectMean:" + numToString(m0(0)), ROIrect.tl(), 16 / binNum);
	putTextOnImage(imgdraw, "rectCov:" + to_string(re.rectCov), ROIrect.tl() + cv::Point(0, 300 / binNum), 16 / binNum);
	re.imgdraw = imgdraw;
	std::mutex mtx;
	mtx.lock();
	saveSLBLuminane(re.rectMean);
	mtx.unlock();
	return re;
}

cv::Mat MLIQMetrics::MLLuminance::getZoneAMask(cv::Size size, int radius)
{
	int c0x = size.width/2;
	int c0y = size.height/2;
	arma::vec xvec = arma::linspace(0, size.width-1, size.width);
	arma::vec yvec = arma::linspace(0, size.height-1, size.height);
	arma::mat xmat = arma::repmat(xvec.t(), yvec.n_elem, 1);
	arma::mat ymat = arma::repmat(yvec.t(), xvec.n_elem, 1);
	ymat = ymat.t();
	xmat = xmat - c0x;
	ymat = ymat -c0y;
	arma::mat dis(size.width, size.height, arma::fill::zeros);
	dis = arma::sqrt(xmat % xmat + ymat % ymat);
	arma::mat mask = arma::zeros(size.width, size.height);
	arma::umat mask1 = (dis < radius);
	mask.elem(arma::find(mask1)).fill(1);
	cv::Mat maskMat(mask.n_rows, mask.n_cols, CV_64FC1, mask.memptr());
	maskMat.convertTo(maskMat, CV_8UC1);
	cv::rotate(maskMat, maskMat, ROTATE_90_CLOCKWISE);
	return maskMat;
}

void MLIQMetrics::MLLuminance::saveSLBLuminane(double lumi)
{
	
	if (m_isSLB)
	{
		string fovstr = IQMetricUtl::instance()->fovTypeToString(m_fovType);
		string filepath= "./config/ALGConfig/slbLumi_"+ fovstr+"_"+m_color + ".csv";
		cv::Mat lumimat(1,CV_32FC1,lumi);
		writeMatTOCSV(filepath, lumimat);
	}
}

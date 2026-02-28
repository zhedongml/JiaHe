#include "pch.h"
#include "ML_LuminanceRolloff.h"
#include "ML_Efficiency.h"
#include "pch.h"
#include"MLSolidDetection.h"
#include"LogPlus.h"
using namespace MLImageDetection;
using namespace MLIQMetrics;
using namespace cv;
MLIQMetrics::MLLuminanceRolloff::MLLuminanceRolloff()
{
}
MLIQMetrics::MLLuminanceRolloff::~MLLuminanceRolloff()
{
}
void MLIQMetrics::MLLuminanceRolloff::setIsUpdateSLB(bool flag)
{
	m_IsUpdateSLB = flag;
}
void MLIQMetrics::MLLuminanceRolloff::setFOVType(FOVTYPE type)
{
	m_fovType = type;
}
void MLIQMetrics::MLLuminanceRolloff::setIsSLB(bool flag)
{
	m_isSLB = flag;
}
void MLIQMetrics::MLLuminanceRolloff::setColor(string color)
{
	m_color = color;
}
RolloffJiaXingRe MLIQMetrics::MLLuminanceRolloff::getRelativeBrightness(const cv::Mat img)
{

	RolloffJiaXingRe rollRe;
	if (img.empty())
	{
		rollRe.flag = false;
		rollRe.errMsg = "Input image is NULL";
		return rollRe;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	cv::Rect rectROI = IQMetricsParameters::ROIRect;
	updateRectByRatio1(rectROI, 1.0 / binNum);
	if (m_fovType == BIGFOV)
		rectROI = cv::Rect(0, 0, -1, -1);
	cv::Mat imgROI = GetROIMat(img, rectROI);
	Mat img8 = convertToUint8(imgROI);
	cv::Mat img_draw = convertTo3Channels(img8);
	MLSolidDetection solid;
	solid.setBinNum(binNum);
	solid.setFOVType(m_fovType);
	SolidDetectionRe solidRe = solid.getSolidLocation(img8);
	//updateRotateImg(img, solidRe.rotationAngle);
	updateRotateImg(img_draw, solidRe.rotationAngle);
	updateRotateImg(img8, solidRe.rotationAngle);
	cv::Rect rectR = solid.getSolidExactRect(img8, solidRe.rectAf);
	//drawRectOnImage(img_draw, rectAfRo);
	double ratio = IQMetricsParameters::RolloffAreaRatio;
	cv::Rect rectRatio = updateRectByRatio(rectR, sqrt(ratio));
	//drawRectOnImage(img_draw, rectRatio);
	rectangle(img_draw, rectRatio, Scalar(0, 255, 0), 4);
	cv::Mat lumiROI = imgROI(rectRatio).clone();
	double p50 = percentile(lumiROI, 50);
	double p5 = percentile(lumiROI, 5);
	double p95 = percentile(lumiROI, 95);
	string strh = "P5:" + to_string(p5);
	string strv = "P95:" + to_string(p95);
	string strv1 = "P50:" + to_string(p50);
	putTextOnImage(img_draw, strh, rectRatio.tl(), 20 / binNum);
	putTextOnImage(img_draw, strv, rectRatio.tl() + cv::Point(0, 300 / binNum), 20 / binNum);
	putTextOnImage(img_draw, strv1, rectRatio.tl() + cv::Point(0, 600 / binNum), 20 / binNum);
	cv::Scalar m, std0;
	cv::meanStdDev(lumiROI,m,std0);
	double CV = m(0) / std0(0);
	rollRe.meanLumi = cv::mean(lumiROI)(0);
	rollRe.CV = CV;
	rollRe.p50 = p50;
	rollRe.p5 = p5;
	rollRe.p95 = p95;
	rollRe.ralativeBrightnessLow = 95 / p95;
	rollRe.ralativeBrightnessHigh = p95 / p5;
	rollRe.imgdraw = img_draw;
	std::mutex mtx;
	mtx.lock();
	saveSLBLuminane(rollRe.meanLumi);
	mtx.unlock();
	return rollRe;
}
RolloffRe MLIQMetrics::MLLuminanceRolloff::LuminanceRolloffRotation(cv::Mat img)
{
	RolloffRe rollRe;
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
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Rolloff calculation successfully");
	return rollRe;
}

void MLIQMetrics::MLLuminanceRolloff::saveSLBLuminane(double lumi)
{
	if (m_isSLB&m_IsUpdateSLB)
	{
		string fovstr = IQMetricUtl::instance()->fovTypeToString(m_fovType);
		string filepath = "./config/ALGConfig/" + fovstr + "_slbLumi_" + m_color + ".csv";
		//cv::Mat lumimat(1, CV_32FC1, lumi);
		cv::Mat lumimat(cv::Size(1, 1), CV_32FC1, lumi);
		writeMatTOCSV(filepath, lumimat);
	}
}

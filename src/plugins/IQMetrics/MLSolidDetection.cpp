#include "pch.h"
#include"LogPlus.h"
#include "MLSolidDetection.h"
#include"ml_image_public.h"
#include"ml_rectangleDetection.h"
using namespace MLImageDetection;
using namespace MLIQMetrics;
using namespace cv;
//std::mutex mtx;
MLIQMetrics::MLSolidDetection::MLSolidDetection()
{
}

MLIQMetrics::MLSolidDetection::~MLSolidDetection()
{
}

void MLIQMetrics::MLSolidDetection::setBinNum(int num)
{
	m_binNum = num;
}

void MLIQMetrics::MLSolidDetection::setFOVType(FOVTYPE type)
{
	m_FOVType = type;
}

SolidDetectionRe MLIQMetrics::MLSolidDetection::getSolidLocation(cv::Mat img)
{
	SolidDetectionRe  re;

	if (isSolidDetectionFlagBig&& m_FOVType == BIGFOV)
	{
		  re= readSolidRect(BIGFOV);
		return re;
	}
	else if (isSolidDetectionFlagSmall && m_FOVType == SMALLFOV)
	{
		re = readSolidRect(SMALLFOV);
		return re;
	}
	else
	{
		re = getSolidDtectionRe(img);
		return re;

	}

	return SolidDetectionRe();
}

SolidDetectionRe MLIQMetrics::MLSolidDetection::getSolidDtectionRe(cv::Mat img8)
{
	MLimagePublic pl;
	SolidDetectionRe re;
	int binNum = m_binNum; //= IQMetricUtl::instance()->getBinNum(img8.size());
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = "the image size is not right, please check the input image";
		return re;
	}
	//cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	//pl.updateRectByRatio1(ROIRect, 1.0 / binNum);
	//img8 = pl.GetROIMat(img8, ROIRect);
	
	cv::Rect rect;
	int resizeNum = 4 / binNum;
	cv::Mat imgRe;
	cv::resize(img8, imgRe, img8.size() / resizeNum);
	RectangleDetection rd;
	rd.setBinNum(4);
	cv::RotatedRect rectR = rd.getRectangleBorder(imgRe);
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Solid Border detection successfully");
	cv::Rect rectAf;
	cv::Point2f center((float)(imgRe.cols / 2), (float)(imgRe.rows / 2));
	rectAf = pl.updateRotateRect(rectR, center);
	pl.updateRectByRatio1(rectAf, resizeNum);
	re.rectAf = rectAf;
	re.rotationAngle = rectR.angle;
	std::mutex mtx;
	mtx.lock();
	saveSolidRect(m_FOVType, re);
	mtx.unlock();
	//rd.setBinNum(binNum);
	//rectAf = rd.getSolidExactRect(img8, rectAf);
    return re;
}

cv::Rect MLIQMetrics::MLSolidDetection::getSolidExactRect(cv::Mat img8, cv::Rect rect)
{
	RectangleDetection rd;
	rd.setBinNum(m_binNum);
	return rd.getSolidExactRect(img8, rect);
}

void MLIQMetrics::MLSolidDetection::setDetectionFlag(bool flag, FOVTYPE type)
{
	if (type == BIGFOV)
		isSolidDetectionFlagBig = flag;
	else if (type == SMALLFOV)
		isSolidDetectionFlagSmall = flag;
}

void MLIQMetrics::MLSolidDetection::saveSolidRect(FOVTYPE type, SolidDetectionRe re)
{
	MLimagePublic pl;
	cv::Mat soildmat(1, 5, CV_32FC1, Scalar(0));
	soildmat.at<float>(0, 0) = re.rectAf.x;
	soildmat.at<float>(0, 1) = re.rectAf.y;
	soildmat.at<float>(0, 2) = re.rectAf.width;
	soildmat.at<float>(0, 3) = re.rectAf.height;
	soildmat.at<float>(0, 4) = re.rotationAngle;
	string fovstr = IQMetricUtl::instance()->fovTypeToString(m_FOVType);
	string filepath = "./config/ALGConfig/"+fovstr+"_solidLoc.csv";
	pl.writeMatTOCSV(filepath, soildmat);
	if (type == BIGFOV)
	{
		isSolidDetectionFlagBig = true;
	}
	else if (type == SMALLFOV)
	{
		isSolidDetectionFlagSmall = true;
	}
}

SolidDetectionRe MLIQMetrics::MLSolidDetection::readSolidRect(FOVTYPE type)
{
	SolidDetectionRe re;
	cv::Mat solidmat;
	MLimagePublic pl;
	string fovstr = IQMetricUtl::instance()->fovTypeToString(m_FOVType);
	string filepath = "./config/ALGConfig/" + fovstr + "_solidLoc.csv";
	solidmat = pl.readCSVToMat(filepath);
	re.rectAf.x = solidmat.at<float>(0, 0);
	re.rectAf.y = solidmat.at<float>(0, 1);
	re.rectAf.width = solidmat.at<float>(0, 2);
	re.rectAf.height = solidmat.at<float>(0, 3);
	re.rotationAngle = solidmat.at<float>(0, 4);
	return re;
}

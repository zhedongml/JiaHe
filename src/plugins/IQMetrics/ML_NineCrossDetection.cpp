#include "pch.h"
#include "ML_NineCrossDetection.h"
#include"ml_image_public.h"
#include"LogPlus.h"
#include"CrossCenter.h"
#include <mutex>
using namespace MLImageDetection;
using namespace MLIQMetrics;
using namespace cv;
std::mutex mtx;
MLIQMetrics::MLNineCrossDetection::MLNineCrossDetection()
{
}

MLIQMetrics::MLNineCrossDetection::~MLNineCrossDetection()
{
}

void MLIQMetrics::MLNineCrossDetection::setBinNum(int num)
{
	m_binNum = num;
}

void MLIQMetrics::MLNineCrossDetection::setFOVType(FOVTYPE type)
{
	m_fovType = type;
}

void MLIQMetrics::MLNineCrossDetection::setDetectionFlag(bool flag, FOVTYPE type)
{
	if (type == BIGFOV)
		isNineCrossDetectionFlagBig = flag;
	else if (type == SMALLFOV)
		isNineCrossDetectionFlagSmall = flag;
}

bool MLIQMetrics::MLNineCrossDetection::getDtectionFlag(FOVTYPE type)
{
	if (type == BIGFOV)
		return isNineCrossDetectionFlagBig;
	else if (type == SMALLFOV)
		return isNineCrossDetectionFlagBig;
	return false;
}

map<string, cv::Point2f> MLIQMetrics::MLNineCrossDetection::getNineCrosshairLocation(cv::Mat img, FOVTYPE crossType)
{
	map<string, cv::Point2f> cenMap;
	MLimagePublic pl;
	if (isNineCrossDetectionFlagBig && crossType==BIGFOV)
	{
		cenMap=readCrossLocation(img, BIGFOV);
		return cenMap;
	}
	else if (isNineCrossDetectionFlagSmall && crossType == SMALLFOV)
	{
		cenMap = readCrossLocation(img, SMALLFOV);
		return cenMap;
	}
	else
	{
		cenMap = getNineCrosshairCenter(img, crossType);
		return cenMap;
	}
}

map<string, cv::Point2f> MLIQMetrics::MLNineCrossDetection::getNineCrosshairCenter(cv::Mat img, FOVTYPE crossType)
{
	map<string, cv::Point2f> cenMap;
	if (img.empty())
		return cenMap;
	MLimagePublic pl;
	int binNum =m_binNum ;// IQMetricUtl::instance()->getBinNum(img.size());
	int resizeNum = 4 / binNum;
	cv::Mat img8 = pl.convertToUint8(img);
	cv::Mat imgResize;
	cv::resize(img8, imgResize, img8.size() / resizeNum);
	cv::Mat imgdraw = pl.convertTo3Channels(img8);
	cv::GaussianBlur(imgResize, imgResize, cv::Size(5, 5), 0, 0);
	cv::Mat imgth;
	cv::threshold(imgResize, imgth, 0, 255, THRESH_OTSU);
	cv::Point2f c0(imgth.cols / 2, imgth.rows / 2);
	cv::Rect rect;
	pl.getClosestContourRect(imgth, c0, rect);
	pl.updateRectByRatio1(rect, resizeNum);
	pl.drawRectOnImage(imgdraw, rect);
	cv::Mat roi = img8(rect).clone();
	CrossCenter cc;
	cv::Point2f center = cc.find_centerLINES(roi);
	if (center.x > 1e-6 && center.y > 1e-6)
	{
		center = center + cv::Point2f(rect.tl());
		double theta = -cc.GetVerticalLineTheta();
		double theta0 = cc.GetHorizontalLineTheta();
		theta = theta0;
		cv::Point2f d1 = center;
		vector<double> xdeg, ydeg;
		if (crossType == SMALLFOV)
		{
			xdeg = xdegNine;
			ydeg = ydegNine;
		}
		else if (crossType == BIGFOV)
		{
			xdeg = xdegNineBig;
			ydeg = ydegNineBig;
		}

		vector<cv::Point2f> ptsVec;
		for (int i = 0; i < ydeg.size(); i++)
		{
			ptsVec.push_back(d1 + Point2f(xdeg[i]/binNum , ydeg[i]/binNum));
		}
		double angle = theta * 180.0 / CV_PI;
		vector<cv::Point2f> ptsR = pl.rotatePoints(ptsVec, d1, -angle);
		//drawPointsOnImage(imgdraw, ptsVec, 5, Scalar(0, 0, 255));
		pl.drawPointsOnImage(imgdraw, ptsR, 5, Scalar(0, 255, 0));
		int len;// = m_ROILen / binNum;
		if (crossType == BIGFOV)
			len = m_ROILenBig / binNum;
		else if (crossType == SMALLFOV)
			len = m_ROILenSmall / binNum;
		for (int i = 0; i < ptsR.size(); i++)
		{
			cv::Point2f c0 = ptsR[i];
			cv::Rect rect(c0.x - len / 2, c0.y - len / 2, len, len);
			cenMap.insert(std::make_pair(to_string(i + 1), ptsR[i] ));
			m_cenRectMap.insert(std::make_pair(to_string(i + 1),rect));
			pl.putTextOnImage(imgdraw, to_string(i + 1), ptsR[i], 16 / 4);
			pl.drawRectOnImage(imgdraw, rect);
		}
		//std::mutex mtx;   
		mtx.lock();   
		saveCrossLocation(ptsR, crossType);
		mtx.unlock();

		return cenMap;
	}
	else
	{
		return map<string, cv::Point2f>();
	}

}

map<string, cv::Rect> MLIQMetrics::MLNineCrossDetection::getNineCrossRect()
{
	return m_cenRectMap;
}

map<string, cv::Point2f> MLIQMetrics::MLNineCrossDetection::getNineCrossLocationPreLoc(cv::Mat img, map<string, cv::Point2f> cenMap)
{
	map<string, cv::Point2f> cenMapUpdate;
	if (img.empty())
		return cenMap;
	MLimagePublic pl;
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	int resizeNum = 4 / binNum;
	cv::Mat img8 = pl.convertToUint8(img);
	cv::Mat imgResize;
	cv::Mat imgdraw = pl.convertTo3Channels(img8);
	int len;
	if (m_fovType == BIGFOV)
		len = m_ROILenBig / binNum;
	else if (m_fovType == SMALLFOV)
		len = m_ROILenSmall / binNum;
	for (auto it = cenMap.begin(); it != cenMap.end(); ++it)
	{
		string str = it->first;
		cv::Point2f c0 = it->second;
		cv::Rect rect(c0.x - len / 2, c0.y - len / 2, len, len);
		cv::Rect rectAnd = rect & cv::Rect(0, 0, img.cols, img.rows);
		cv::Mat roi = img(rectAnd);
		CrossCenter cc;
		//TODO:change to gaussian method
		cv::Point2f c01 = cc.find_centerLINES(roi);
		if (c01.x > 1e-6 && c01.y > 1e-6)
			c01 = c01 + cv::Point2f(rectAnd.tl());
		else
			c01 = c0;
		pl.drawPointOnImage(imgdraw, c01);
		cenMapUpdate.insert(std::make_pair(str, c01));
	}
	return cenMapUpdate;
}

void MLIQMetrics::MLNineCrossDetection::saveCrossLocation(vector<cv::Point2f> ptsR, FOVTYPE type)
{
	MLimagePublic pl;
	if (type == BIGFOV)
	{

		string filepath = "./config/ALGConfig/BIGFOV_crossLoc.csv";
		pl.writePointsFile(filepath, ptsR);
		isNineCrossDetectionFlagBig = true;
	}
	else if (type == SMALLFOV)
	{
		string filepath = "./config/ALGConfig/SMALLFOV_crossLoc.csv";
		pl.writePointsFile(filepath, ptsR);
		isNineCrossDetectionFlagSmall = true;
	}
}

map<string, cv::Point2f> MLIQMetrics::MLNineCrossDetection::readCrossLocation(cv::Mat img, FOVTYPE type)
{
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	map<string, cv::Point2f>cenMap;
	MLimagePublic pl;
	cv::Mat ptsmat;
	if (type == FOVTYPE::BIGFOV)
	{
		string filepath = "./config/ALGConfig/BIGFOV_crossLoc.csv";
		ptsmat=pl.readCSVToMat(filepath);

	}
	else if (type == FOVTYPE::SMALLFOV)
	{
		string filepath = "./config/ALGConfig/SMALLFOV_crossLoc.csv";
		ptsmat = pl.readCSVToMat(filepath);
	}
	int len;// = m_ROILen / binNum;
	if (type == BIGFOV)
		len = m_ROILenBig / binNum;
	else if (type == SMALLFOV)
		len = m_ROILenSmall / binNum;
	for (int i = 0; i < ptsmat.rows; i++)
	{
		cv::Point2f c0;
		c0.x = ptsmat.at<float>(i, 0);
		c0.y = ptsmat.at<float>(i, 1);
		cv::Rect rect(c0.x - len / 2, c0.y - len / 2, len, len);
		m_cenRectMap.insert(std::make_pair(to_string(i + 1), rect));
		cenMap.insert(std::make_pair(to_string(i + 1), c0));
	}
	return cenMap;
}

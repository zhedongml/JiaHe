#include "pch.h"
#include "ML_CrossMTF.h"
#include"ml_multiCrossHairDetection.h"
#include "CrossCenter.h"
#include"pipeline.h"
#include"LogPlus.h"
#include"ML_NineCrossDetection.h"
using namespace MLImageDetection;
using namespace MLIQMetrics;
using namespace cv;
MLIQMetrics::MLCrossMTF::MLCrossMTF()
{
}

MLIQMetrics::MLCrossMTF::~MLCrossMTF()
{
}

void MLIQMetrics::MLCrossMTF::setFOVType(FOVTYPE type)
{
	m_fovType = type;
}

MLIQMetrics::CenterCrossHairMTFROIRe MLIQMetrics::MLCrossMTF::getCenterCrossMTFROICenter(cv::Mat img, int offset)
{
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "cross MTF ROI  center calculation start");

	CenterCrossHairMTFROIRe re;
	string info = "------getCenterCrossMTFROICenter------";
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}

	m_binNum = binNum;
	img = GetROIMat(img, IQMetricsParameters::ROIRect);// img(IQMetricsParameters::ROIRect);
	MultiCrossHairDetection md;
	vector<cv::Point2f> ptsVec;
	md.setBinNum(binNum);
	MultiCrossHairRe cenRe = md.getMuliCrossHairCenter(img, IQMetricsParameters::crossBinNum / binNum, false);
	if (!cenRe.flag)
	{
		re.flag = false;
		re.errMsg = info + cenRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "cross detection successfully");
	map<string, vector<double>>mtfMap;
	string str = "5";
	cv::Rect rect = cenRe.rectMap[str];
	cv::Mat roi = img(rect).clone();
	CrossCenter cc(roi, offset);
	cv::Point2f cen = cc.find_centerLINES(roi);
	if (cen.x < 1e-6 || cen.y < 1e-6)
	{
		re.flag = false;
		re.errMsg = info + "center cross hair detection fail";
		return re;
	}
	vector<cv::Point> cenVecROI = cc.get_roi_center();
	vector<cv::Point> cenVec;
	for (int i = 0; i < cenVecROI.size(); i++)
	{
		cv::Point c0 = cenVecROI[i] + rect.tl();
		circle(cenRe.img_draw, c0, 10, Scalar(255, 0, 255), -1);
		cenVec.push_back(c0);
	}
	re.cenVec = cenVec;
	re.imgdraw = cenRe.img_draw;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "getCenterCrossMTFROICenter calculation successfully");
	return re;
}

CrossMTFRe MLIQMetrics::MLCrossMTF::getCrossMTF(cv::Mat img)
{
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "cross MTF calculation start");

	CrossMTFRe re;
	string info = "------getCrossMTF------";
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}

	m_binNum = binNum;
	img = GetROIMat(img, IQMetricsParameters::ROIRect);// img(IQMetricsParameters::ROIRect);
	MultiCrossHairDetection md;
	vector<cv::Point2f> ptsVec;
	md.setBinNum(binNum);
	MultiCrossHairRe cenRe = md.getMuliCrossHairCenter(img, IQMetricsParameters::crossBinNum / binNum, false);
	if (!cenRe.flag)
	{
		re.flag = false;
		re.errMsg = info + cenRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "cross detection successfully");

	map<string, vector<double>>mtfMap;
	int mtfLen = 0;
	for (auto it = cenRe.rectMap.begin(); it != cenRe.rectMap.end(); ++it)
	{
		string str = it->first;
		cv::Rect rect = it->second;
		mtfRectRe mtfre = calMtfRect(img, rect, cenRe.img_draw);
		re.mtfMap.insert(std::make_pair(str, mtfre.mtfVecFreq1));
		re.mtfMapFreq2.insert(std::make_pair(str, mtfre.mtfVecFreq2));
		re.mtfResult.insert(std::make_pair(str, mtfre));
		if (mtfre.mtflen > mtfLen)
			mtfLen = mtfre.mtflen;
	}

	cv::Mat mtfMat(cv::Size(72, mtfLen), CV_32FC1, Scalar(-1));
	int n = 0;
	for (auto it = re.mtfResult.begin(); it != re.mtfResult.end(); ++it)
	{
		string str = it->first;
		mtfRectRe mtfre = it->second;
		for (int i = 0; i < mtfre.mtfCurve.size(); i++)
		{
			double* freq0 = mtfre.freqCurve[i];
			double* mtf0 = mtfre.mtfCurve[i];
			cv::Mat freqMat0(mtfLen, 1, CV_64FC1, freq0);
			cv::Mat mtfMat0(mtfLen, 1, CV_64FC1, mtf0);
			freqMat0.copyTo(mtfMat(cv::Range(0, mtfLen), cv::Range(n * 8 + i * 2, n * 8 + i * 2 + 1)));
			mtfMat0.copyTo(mtfMat(cv::Range(0, mtfLen), cv::Range(n * 8 + i * 2 + 1, n * 8 + i * 2 + 2)));
		}
		n = n + 1;
	}
	re.mtfMat = mtfMat;
	re.imgdraw = cenRe.img_draw;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "crossMTF calculation successfully");
	return re;
}

CrossMTFRe MLIQMetrics::MLCrossMTF::getCrossMTF(cv::Mat img, FOVTYPE crossType)
{
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "cross MTF calculation start");
	CrossMTFRe re;
	string info = "------getCrossMTF------";
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	m_binNum = binNum;
	// if (binNum <= 0)
	//{
	//    re.flag = false;
	//    re.errMsg = info + "the image size is not right, please check the input image";
	//    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
	//    return re;
	//}
	//img = GetROIMat(img, IQMetricsParameters::ROIRect); // img(IQMetricsParameters::ROIRect);
	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);
	MLNineCrossDetection cross;
	cross.setBinNum(binNum);
	map<string, cv::Point2f> cenMap= cross.getNineCrosshairLocation(img8,crossType);
	map<string, cv::Rect> cenRectMap = cross.getNineCrossRect();
	map<string, vector<double>> mtfMap;
	vector <double>mtfhVec1, mtfvVec1, mtfhVec2, mtfvVec2;
	int mtfLen = 0;
	for (auto it = cenRectMap.begin(); it != cenRectMap.end(); ++it)
	{
		string str = it->first;
		cv::Rect rect = it->second;
		mtfRectRe mtfre = calMtfRect(img, rect,imgdraw);

		mtfhVec1.push_back(mtfre.mtfVecFreq1[0]);
		mtfhVec1.push_back(mtfre.mtfVecFreq1[1]);
		mtfhVec2.push_back(mtfre.mtfVecFreq2[0]);
		mtfhVec2.push_back(mtfre.mtfVecFreq2[1]);
		mtfvVec1.push_back(mtfre.mtfVecFreq1[2]);
		mtfvVec1.push_back(mtfre.mtfVecFreq1[3]);
		mtfvVec2.push_back(mtfre.mtfVecFreq2[2]);
		mtfvVec2.push_back(mtfre.mtfVecFreq2[3]);

		re.mtfMap.insert(std::make_pair(str, mtfre.mtfVecFreq1));
		re.mtfMapFreq2.insert(std::make_pair(str, mtfre.mtfVecFreq2));
		re.mtfResult.insert(std::make_pair(str, mtfre));
		if (mtfre.mtflen > mtfLen)
			mtfLen = mtfre.mtflen;
	}
	double meanH = std::accumulate(mtfhVec1.begin(), mtfhVec1.end(), 0.0) / mtfhVec1.size();
	double meanH2 = std::accumulate(mtfhVec2.begin(), mtfhVec2.end(), 0.0) / mtfhVec2.size();
	double minH = *min_element(mtfhVec1.begin(), mtfhVec1.end());
	double minH2 = *min_element(mtfhVec2.begin(), mtfhVec2.end());
	double meanV = std::accumulate(mtfvVec1.begin(), mtfvVec1.end(), 0.0) / mtfvVec1.size();
	double meanV2 = std::accumulate(mtfvVec2.begin(), mtfvVec2.end(), 0.0) / mtfvVec2.size();
	double minV = *min_element(mtfvVec1.begin(), mtfvVec1.end());
	double minV2 = *min_element(mtfvVec2.begin(), mtfvVec2.end());
	re.meanH = meanH;
	re.meanH2 = meanH2;
	re.minH = minH;
	re.minH2 = minH2;
	re.meanV = meanV;
	re.meanV2 = meanV2;
	re.minV = minV;
	re.minV2 = minV2;
	re.mtfMapH = convertVecToMat(mtfhVec1);
	re.mtfMapH2 = convertVecToMat(mtfhVec2);
	re.mtfMapV = convertVecToMat(mtfvVec1);
	re.mtfMapV2 = convertVecToMat(mtfvVec2);
	re.imgdraw = imgdraw;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "crossMTF calculation successfully");
	return re;
}

mtfRectRe MLIQMetrics::MLCrossMTF::getSingleCrossMTF(cv::Mat img)
{

	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, " single cross MTF calculation start");
	mtfRectRe re;
	string info = "------getCrossMTF------";
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}

	m_binNum = binNum;
	img = GetROIMat(img, IQMetricsParameters::ROIRect);// img(IQMetricsParameters::ROIRect);

	CrossCenter cc(img, 260);
	cc.find_centerLINES(img);
	vector<cv::Point> cenVec = cc.get_roi_center();

	if (cenVec.size() != 4)
	{
		re.flag = false;
		re.errMsg = info + "cross hair detection fail";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "cross detection successfully");

	map<string, vector<double>>mtfMap;
	int roiW = 420;
	int roiH = 120;
	cv::Mat imgdraw = convertTo3Channels(img);
	for (int i = 0; i < cenVec.size(); i++)
	{
		cv::Point c0 = cenVec[i];
		circle(imgdraw, c0, 10, Scalar(0, 255, 0), -1);
		cv::Rect mtfRect;
		if (i < 2)
		{
			mtfRect.x = c0.x - roiW / 2;
			mtfRect.y = c0.y - roiH / 2;
			mtfRect.width = roiW;
			mtfRect.height = roiH;
		}
		else
		{
			mtfRect.x = c0.x - roiH / 2;
			mtfRect.y = c0.y - roiW / 2;
			mtfRect.width = roiH;
			mtfRect.height = roiW;
		}
		cv::Mat mtfROI = img(mtfRect).clone();
		drawRectOnImage(imgdraw, mtfRect);
		putTextOnImage(imgdraw, strVec[i], mtfRect.tl(), 24 / m_binNum);
		PipeLine* mtfPipeline = new PipeLine();
		mtfPipeline->SetPixelValue(3.45e-3);
		mtfPipeline->set_freq_unit(FREQ_UNIT::lp_deg, 60);
		double freq = IQMetricsParameters::mtfFreq;
		int ret = mtfPipeline->culc_mtf(mtfROI, CROSS);
		double mtf0 = -1;
		double* mtf, * f;
		int len = mtfPipeline->get_bin_len();
		re.mtflen = len;
		if (ret >= 0)
		{
			mtf0 = mtfPipeline->getMtfByFreq(freq);
			mtf = mtfPipeline->get_mtf();
			f = mtfPipeline->get_freq();
		}
		re.mtfVecFreq1.push_back(mtf0);
		re.mtfCurve.push_back(mtf);
		re.freqCurve.push_back(f);
	}
	re.imgdraw = imgdraw;

	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "crossMTF calculation successfully");
	return re;
}

mtfRectRe MLIQMetrics::MLCrossMTF::calMtfRect(cv::Mat img, cv::Rect rect, cv::Mat& imgdraw)
{
	mtfRectRe re;
	cv::Mat roi = img(rect).clone();
	int offset, roiW, roiH;
	if (m_fovType == SMALLFOV)
	{
		 offset = IQMetricsParameters::ROI_center_offset / m_binNum;
		 roiW = IQMetricsParameters::crossMTFWidth / m_binNum;
		 roiH = IQMetricsParameters::crossMTFHeight / m_binNum;
	}
	else
	{
		offset = IQMetricsParameters::ROI_center_offset_big / m_binNum;
		roiW = IQMetricsParameters::crossMTFWidthBig / m_binNum;
		roiH = IQMetricsParameters::crossMTFHeightBig / m_binNum;
	}

	CrossCenter cc(roi, offset);
	cv::Point2f c0=cc.find_centerLINES(roi);
	if (c0.x < 1e-6 || c0.y < 1e-6)
		c0 = cc.find_centerGaussian(roi,false);
	vector<cv::Point> cenVec = cc.get_roi_center();

	double pixel = IQMetricsParameters::pixel_size * m_binNum;
	double focallength = IQMetricsParameters::FocalLength;
	double freq = IQMetricsParameters::mtfFreq;
	double freq2 = IQMetricsParameters::mtfFreq2;

	for (int i = 0; i < cenVec.size(); i++)
	{
		cv::Point c0 = cenVec[i] + rect.tl();
		circle(imgdraw, c0, 10, Scalar(0, 255, 0), -1);
		cv::Rect mtfRect;
		if (i < 2)
		{
			mtfRect.x = c0.x - roiW / 2;
			mtfRect.y = c0.y - roiH / 2;
			mtfRect.width = roiW;
			mtfRect.height = roiH;
		}
		else
		{
			mtfRect.x = c0.x - roiH / 2;
			mtfRect.y = c0.y - roiW / 2;
			mtfRect.width = roiH;
			mtfRect.height = roiW;
		}
		cv::Mat mtfROI = img(mtfRect).clone();
		drawRectOnImage(imgdraw, mtfRect);
		//putTextOnImage(imgdraw, strVec[i], mtfRect.tl(), 20 / m_binNum);
		PipeLine* mtfPipeline = new PipeLine();
		mtfPipeline->SetPixelValue(pixel);
		mtfPipeline->set_freq_unit(FREQ_UNIT::lp_deg, focallength);
		int ret = mtfPipeline->culc_mtf(mtfROI, CROSS);
		double mtf0 = -1;
		double mtf1 = -1;
		double* mtf, * f;
		int mtfLen = mtfPipeline->get_bin_len();;
		if (ret >= 0)
		{
			mtf0 = mtfPipeline->getMtfByFreq(freq);
			mtf1 = mtfPipeline->getMtfByFreq(freq2);
			mtf = mtfPipeline->get_mtf();
			f = mtfPipeline->get_freq();
		}
		re.mtfVecFreq1.push_back(mtf0);
		re.mtfVecFreq2.push_back(mtf1);
		re.mtfCurve.push_back(mtf);
		re.freqCurve.push_back(f);
		re.mtflen = mtfPipeline->get_bin_len();
		delete mtfPipeline;
	}
	if (cenVec.size() == 0)
	{
		re.mtfVecFreq1.push_back(-1);
		re.mtfVecFreq1.push_back(-1);
		re.mtfVecFreq1.push_back(-1);
		re.mtfVecFreq1.push_back(-1);
	}
	return re;
}

cv::Mat MLIQMetrics::MLCrossMTF::convertVecToMat(const vector<double> mtfVec)
{
	//cv::Mat mtfmat(cv::Size(3, 3), CV_32FC1, cv::Scalar(-1));
	vector<double>mtfAvgVec;
	for (int i = 0; i +1< mtfVec.size(); i = i + 2)
	{
		double avg = (mtfVec[i] + mtfVec[i + 1]) / 2.0;
		mtfAvgVec.push_back(avg);
	}
	cv::Mat mtfmat=cv::Mat(3,3,CV_64FC1, mtfAvgVec.data()).clone();
	mtfmat.convertTo(mtfmat, CV_32FC1);
	return mtfmat;
}


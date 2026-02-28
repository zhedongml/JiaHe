#include "pch.h"
#include "ML_FOVOffset.h"
#include"CrossCenter.h"
#include"ml_multiCrossHairDetection.h"
#include"ml_gridDetect.h"
#include"ML_NineCrossDetection.h"
#include"LogPlus.h"
using namespace cv;
using namespace MLIQMetrics;
using namespace MLImageDetection;

MLIQMetrics::MLFOVOffset::MLFOVOffset()
{
}

MLIQMetrics::MLFOVOffset::~MLFOVOffset()
{
}


void MLIQMetrics::MLFOVOffset::setIsUpdateSLB(bool flag)
{
	m_IsUpdateSLB = flag;
}

void MLIQMetrics::MLFOVOffset::setIsSLB(bool flag)
{
	m_IsSLB = flag;
}

void MLIQMetrics::MLFOVOffset::setFOVType(FOVTYPE type)
{
	m_FOVType = type;
}

void MLIQMetrics::MLFOVOffset::setColor(string color)
{
	m_color = color;
}

FovOffsetRe MLIQMetrics::MLFOVOffset::getBoresightGrid(cv::Mat img)
{
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Boresight calculation start");

	string  info = "------getBoresightGrid------";
	FovOffsetRe re;

	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is null";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	m_binNum = binNum;
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		return re;
	}
	double pixel2Arcmin = IQMetricUtl::instance()->getPix2Arcmin(img.size());
	cv::Point2f opticalCenter = IQMetricsParameters::opticalCenter / binNum;
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	img = GetROIMat(img, ROIRect);
	opticalCenter = opticalCenter - Point2f(ROIRect.tl());
	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);

	int resizeNum = IQMetricsParameters::ResizeNum / binNum;
	cv::Mat imgResize;
	cv::resize(img8, imgResize, img8.size() / resizeNum);

	MLGridDetect grid;
	grid.setAccurateDetectionFlag(false);
	grid.SetbinNum(resizeNum);
	GridRe gridRe = grid.getGridContour(imgResize);
	if (gridRe.flag == false)
	{
		re.flag = false;
		re.errMsg = info + gridRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Cross detection sucessfully");
	cv::Point2f realcenter;
	int rows = gridRe.xLocMat.rows-1;
	int cols= gridRe.xLocMat.cols-1;
	realcenter.x = gridRe.xLocMat.at<float>(rows / 2, cols / 2)*resizeNum;
	realcenter.y = gridRe.yLocMat.at<float>(rows / 2, cols / 2) * resizeNum;
	if (accurateFlag)
	{
		realcenter=getExactLoc(realcenter, img8);
	}

	double deltaPx = realcenter.x - opticalCenter.x;
	double deltaPy = realcenter.y - opticalCenter.y;
	re.V = deltaPy * pixel2Arcmin;
	re.H = deltaPx * pixel2Arcmin;
	double deltaP = sqrt(pow(deltaPx, 2) + pow(deltaPy, 2));
	re.D = sqrt(re.H*re.H+re.V*re.V);
	re.deltxPixel = deltaPx;
	re.deltyPixel = deltaPy;

	if (m_IsSLB == true)
	{
		vector<double> rovec;
		rovec.push_back(re.H);
		rovec.push_back(re.V);
		cv::Mat romat(rovec);
		writeMatTOCSV("./config/ALGConfig/offset.csv", romat);
	}
	else if (m_IsSLB == false)
	{
		cv::Mat romat = readCSVToMat("./config/ALGConfig/offset.csv");
		re.H = re.H - romat.at<float>(0, 0)-3.02*60;
		re.V = re.V - romat.at<float>(1, 0)+6.03*60;
		re.D = sqrt(re.H * re.H + re.V * re.V);
	}

	updateImgdraw(imgdraw, realcenter, binNum);
	//updateImgdraw(imgdraw, opticalCenter, binNum);
	string strOpt = numToString(opticalCenter.x) + "," + numToString(opticalCenter.y);
	circle(imgdraw, opticalCenter, 24 / binNum, Scalar(255, 0, 0), -1);
	updateImgdraw(imgdraw, opticalCenter + cv::Point2f(0, -400 / binNum), strOpt, binNum);
	string strx = "Deltx(pixel):" + to_string(deltaPx);
	string stry = "Delty(pixel):" + to_string(deltaPy);
	string strxArcmin = "Delty(Arcmin):" + to_string(re.H);
	string stryArcmin = "Delty(Arcmin):" + to_string(re.V);
	updateImgdraw(imgdraw, realcenter + cv::Point2f(0, 400 / binNum), strx, binNum);
	updateImgdraw(imgdraw, realcenter + cv::Point2f(0, 800 / binNum), stry, binNum);
	updateImgdraw(imgdraw, realcenter + cv::Point2f(0, 1200 / binNum), strxArcmin, binNum);
	updateImgdraw(imgdraw, realcenter + cv::Point2f(0, 1600 / binNum), stryArcmin, binNum);
	re.imgdraw = imgdraw.clone();
	re.crossCenter.push_back(realcenter);
	re.crossCenter.push_back(opticalCenter);
	return re;
}

FovOffsetRe MLIQMetrics::MLFOVOffset::getBoresightGuSu(cv::Mat img)
{
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Boresight calculation start");

	string  info = "------getBoresightGuSu------";
	FovOffsetRe re;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is null";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		return re;
	}
	double pixel2Arcmin = IQMetricUtl::instance()->getPix2Arcmin(img.size());
	cv::Point2f opticalCenter = IQMetricsParameters::opticalCenter / binNum;
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	img = GetROIMat(img, ROIRect);
	opticalCenter = opticalCenter - Point2f(ROIRect.tl());
	cv::Mat img8 = convertToUint8(img);
	cv::Mat img_draw = convertTo3Channels(img8);
	MultiCrossHairDetection md;
	md.setBinNum(binNum);
	MultiCrossHairRe crossRe = md.getMuliCrossHairCenter(img8, IQMetricsParameters::crossBinNum / binNum, false);
	
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "crosshair detection successfully");

	if (crossRe.flag == false)
	{
		re.flag = false;
		re.errMsg = info + crossRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	
	if (crossRe.flag)
	{
		//cv::Mat img_draw = crossRe.img_draw;
		cv::Point2f start = crossRe.centerMap["5"];
		//cv::Rect rect0(start.x-m_crossROI/binNum/2,start.y- m_crossROI /binNum/2, m_crossROI, m_crossROI);
		cv::Rect rect0;
		rect0.x = start.x - m_crossROI / binNum / 2;
		rect0.y = start.y - m_crossROI / binNum / 2;
		rect0.width = m_crossROI / binNum;
		rect0.height = m_crossROI / binNum;
		cv::Mat cenMat = img8(rect0).clone();
		CrossCenter cc;
		cv::Point2f center;
		center = cc.find_centerLINES(cenMat);

		// cv::Point2f center=cc.find_centerLINES(cenMat);
		cv::Point2f realcenter;
		if (center.x > 1e-6 && center.y > 1e-6)
		{
			realcenter.x = center.x + ROIRect.x + rect0.x;
			realcenter.y = center.y + ROIRect.y + rect0.y;
			double deltaPx = realcenter.x - opticalCenter.x;
			double deltaPy = realcenter.y - opticalCenter.y;
			re.V = deltaPy * pixel2Arcmin;
			re.H = deltaPx * pixel2Arcmin;
			double deltaP = sqrt(pow(deltaPx, 2) + pow(deltaPy, 2));
			re.D = deltaP * pixel2Arcmin;
			re.deltxPixel = deltaPx;
			re.deltyPixel = deltaPy;
			updateImgdraw(img_draw, realcenter, binNum);
			updateImgdraw(img_draw, opticalCenter, binNum);
			string strx = "Deltx(pixel):" + to_string(deltaPx);
			string stry = "Delty(pixel):" + to_string(deltaPy);
			string strxArcmin = "Delty(Arcmin):" + to_string(re.H);
			string stryArcmin = "Delty(Arcmin):" + to_string(re.V);
			updateImgdraw(img_draw, realcenter + cv::Point2f(0, 200 / binNum), strx, binNum);
			updateImgdraw(img_draw, realcenter + cv::Point2f(0, 400 / binNum), stry, binNum);
			updateImgdraw(img_draw, realcenter + cv::Point2f(0, 600 / binNum), strxArcmin, binNum);
			updateImgdraw(img_draw, realcenter + cv::Point2f(0, 800 / binNum), stryArcmin, binNum);
			re.imgdraw = img_draw.clone();
			re.crossCenter.push_back(realcenter);
			re.crossCenter.push_back(opticalCenter);
		}
		else
		{
			re.flag = false;
			re.errMsg = info + "crosshair detection fail";
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
			return re;
		}
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Boresight calculation successfully");

	return re;
}
FovOffsetRe MLIQMetrics::MLFOVOffset::getBoresightNineCross(const cv::Mat img)
{
	FovOffsetRe re;
	if (img.data != NULL)
	{

		int binNum = IQMetricUtl::instance()->getBinNum(img.size());
		double pixel2deg = IQMetricUtl::instance()->getPix2Degree(img.size());
		cv::Rect ROIRect = IQMetricsParameters::ROIRect;
		updateRectByRatio1(ROIRect, 1.0 / binNum);
		if (m_FOVType == BIGFOV)
			ROIRect = cv::Rect(0, 0, -1, -1);
		cv::Mat roi1 = getRectROIImg(img, ROIRect);
		cv::Mat img8 = convertToUint8(roi1);
		cv::Mat img_draw = convertTo3Channels(img8);
		cv::Point2f opticalCenter = IQMetricsParameters::opticalCenter / binNum;
		opticalCenter = opticalCenter - cv::Point2f(ROIRect.tl())/binNum;

		MLNineCrossDetection cross;
		cross.setBinNum(binNum);
		map<string, cv::Point2f> cenMap=cross.getNineCrosshairLocation(roi1, m_FOVType);
		map<string, cv::Rect> cenRectMap = cross.getNineCrossRect();
		if (cenMap.size()==9)
		{
			cv::Rect rect = cenRectMap["5"];
			cv::Mat cenMat = roi1(rect).clone();
			CrossCenter cc;
			cv::Point2f center;
			//center = cc.find_centerGaussian(cenMat,false);
			if(m_IsSLB)
			  center=cc.find_centerLINES(cenMat);
			else
				center = cc.find_centerGaussian(cenMat, false);
			cv::Point2f realcenter;
			if (center.x > 1e-6 && center.y > 1e-6)
			{
				realcenter.x = center.x  + rect.x;
				realcenter.y = center.y + rect.y;
				double deltaPx = realcenter.x - opticalCenter.x ;
				double deltaPy = realcenter.y - opticalCenter.y ;
				re.V = deltaPy * pixel2deg;
				re.H = deltaPx * pixel2deg;
				double deltaP = sqrt(pow(deltaPx, 2) + pow(deltaPy, 2));
				re.D = deltaP * pixel2deg;
				re.deltxPixel = deltaPx;
				re.deltyPixel = deltaPy;
				cv::circle(img_draw, realcenter, 16 / binNum, cv::Scalar(255, 0, 0), -1);
				cv::circle(img_draw, opticalCenter , 16 / binNum, cv::Scalar(255, 0, 255), -1);
				//string xstr = to_string(realcenter.x);
				//string ystr = to_string(realcenter.y);
				//string text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);
				//cv::putText(img_draw, text, realcenter, FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 255), 2);
				updateImgdraw(img_draw, realcenter, binNum);
				updateImgdraw(img_draw, opticalCenter, binNum);
				string strx = "Deltx(pixel):" + to_string(deltaPx);
				string stry = "Delty(pixel):" + to_string(deltaPy);
				string strxArcmin = "Deltx(Deg):" + to_string(re.H);
				string stryArcmin = "Delty(Deg):" + to_string(re.V);
				updateImgdraw(img_draw, realcenter + cv::Point2f(0, 200 / binNum), strx, binNum);
				updateImgdraw(img_draw, realcenter + cv::Point2f(0, 400 / binNum), stry, binNum);
				updateImgdraw(img_draw, realcenter + cv::Point2f(0, 600 / binNum), strxArcmin, binNum);
				updateImgdraw(img_draw, realcenter + cv::Point2f(0, 800 / binNum), stryArcmin, binNum);				
				upateFOVOffset(re,m_IsSLB);
				re.imgdraw = img_draw.clone();
				re.crossCenter.push_back(realcenter);
				re.crossCenter.push_back(opticalCenter);
			}
		}
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "FOV offset calculation successfully");

	return re;
}
MLIQMetrics::FovOffsetRe MLIQMetrics::MLFOVOffset::BoresightNoBorder(cv::Mat img)
{

	FovOffsetRe re;
	if (img.data != NULL)
	{
		cv::Mat img8 = convertToUint8(img);
		cv::Mat img_draw = convertTo3Channels(img8);
		int binNum = IQMetricUtl::instance()->getBinNum(img.size());
		cv::Rect ROIRect = IQMetricsParameters::ROIRect;
		updateRectByRatio1(ROIRect, 1.0 / binNum);
		cv::Mat roi1 = getRectROIImg(img8, ROIRect);
		cv::Point2f opticalCenter = IQMetricsParameters::opticalCenter / binNum;
		double pixel2deg = IQMetricUtl::instance()->getPix2Degree(img.size());
		MultiCrossHairDetection md;
	
		CenterCrossHairRe centerRe = md.getCenterCrossHairCenter(roi1);
		if (centerRe.flag)
		{
			cv::Mat cenMat = roi1(centerRe.rectCenter).clone();
			CrossCenter cc;
			cv::Point2f center;
			center = cc.find_centerLINES(cenMat);
			// cv::Point2f center=cc.find_centerLINES(cenMat);
			cv::Point2f realcenter;
			if (center.x > 1e-6 && center.y > 1e-6)
			{
				realcenter.x = center.x + ROIRect.x + centerRe.rectCenter.x;
				realcenter.y = center.y + ROIRect.y + centerRe.rectCenter.y;
				double deltaPx = realcenter.x - opticalCenter.x / binNum;
				double deltaPy = realcenter.y - opticalCenter.y / binNum;
				re.V = deltaPy * pixel2deg;
				re.H = deltaPx * pixel2deg;
				double deltaP = sqrt(pow(deltaPx, 2) + pow(deltaPy, 2));
				re.D = deltaP * pixel2deg;
				re.deltxPixel = deltaPx;
				re.deltyPixel = deltaPy;

				cv::circle(img_draw, realcenter, 16 / binNum, cv::Scalar(255, 0, 0), -1);
				cv::circle(img_draw, opticalCenter / binNum, 16 / binNum, cv::Scalar(255, 0, 255), -1);

				string xstr = to_string(realcenter.x);
				string ystr = to_string(realcenter.y);
				string text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);
				cv::putText(img_draw, text, realcenter, FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 255), 2);

				re.imgdraw = img_draw.clone();
				re.crossCenter.push_back(realcenter);
				re.crossCenter.push_back(opticalCenter);
			}
		}
	}
	return re;
}

MLIQMetrics::FovOffsetRe MLIQMetrics::MLFOVOffset::BoresightNoBorder(cv::Mat img, int roationAngle, MirrorALG mirror)
{
	FovOffsetRe re;
	if (img.data != NULL)
	{
		// celeConfig.GetNewPara(img);
		cv::Mat img8 = convertToUint8(img);
		cv::Mat img_draw = convertTo3Channels(img8);
		cv::Rect ROIRect = IQMetricsParameters::ROIRect;
		cv::Mat roi1 = getRectROIImg(img, ROIRect).clone();
		int binNum = IQMetricUtl::instance()->getBinNum(img.size());
		cv::Point2f opticalCenter = IQMetricsParameters::opticalCenter;
		double pixel2deg = IQMetricUtl::instance()->getPix2Degree(img.size());
		MultiCrossHairDetection md;
		CenterCrossHairRe centerRe = md.getCenterCrossHairCenter(roi1);
		if (centerRe.flag)
		{
			cv::Mat cenMat = roi1(centerRe.rectCenter).clone();
			CrossCenter cc;
			cv::Point2f center;
			//center = cc.find_centerGaussianEn(cenMat);
			center = cc.find_centerLINES(cenMat);
			//if (isSLB)
			//	center = cc.find_centerLINES(cenMat);
			//else
			//	center = cc.find_centerGaussian(cenMat, false);

			cv::Point2f realcenter;
			if (center.x > 1e-6 && center.y > 1e-6)
			{
				realcenter.x = center.x + ROIRect.x + centerRe.rectCenter.x;
				realcenter.y = center.y + ROIRect.y + centerRe.rectCenter.y;

				cv::Point2f basePoint =
					updateOpticalCenter(opticalCenter / binNum, img.size(), roationAngle, mirror);
				double deltaPx = realcenter.x - basePoint.x;
				double deltaPy = realcenter.y - basePoint.y;
				re.V = deltaPy * pixel2deg;
				re.H = deltaPx * pixel2deg;
				double deltaP = sqrt(pow(deltaPx, 2) + pow(deltaPy, 2));
				re.D = deltaP * pixel2deg;
				re.deltxPixel = deltaPx;
				re.deltyPixel = deltaPy;
				cv::circle(img_draw, realcenter, 4 / binNum, cv::Scalar(255, 0, 0), -1);
				cv::circle(img_draw, basePoint, 20 / binNum, cv::Scalar(255, 0, 255), -1);
				re.imgdraw = img_draw;
			}
		}
	}
	return re;
}

FovOffsetRe MLIQMetrics::MLFOVOffset::getMultiCrossBoresight(cv::Mat img, int roationAngle, int eyeLoc, MLImageDetection::MirrorALG mirror)
{
	FovOffsetRe re;
	if (img.data != NULL)
	{
		// celeConfig.GetNewPara(img);
		cv::Mat img8 = convertToUint8(img);
		cv::Mat img_draw = convertTo3Channels(img8);
		cv::Rect ROIRect = IQMetricsParameters::ROIRect;
		cv::Mat roi1 = getRectROIImg(img8, ROIRect);
		int binNum = IQMetricUtl::instance()->getBinNum(img.size());
		cv::Point2f opticalCenter = IQMetricsParameters::opticalCenter;
		double pixel2deg = IQMetricUtl::instance()->getPix2Degree(img.size());
		MultiCrossHairDetection md;
		// CenterCrossHairRe centerRe = md.getCenterCrossHairCenterByContour(roi1,binNum);
		MultiCrossHairRe centerRe = md.getMuliCrossHairCenterByDistance(roi1, eyeLoc, binNum);

		if (centerRe.flag)
		{
			// cv::Mat cenMat = roi1(centerRe.rectCenter).clone();
			cv::Mat cenMat = roi1(centerRe.rectMap["9"]).clone();

			CrossCenter cc;
			cv::Point2f center;

			center = cc.find_centerLINES(cenMat);


			cv::Point2f realcenter;
			if (center.x > 1e-6 && center.y > 1e-6)
			{
				realcenter.x = center.x + ROIRect.x + centerRe.rectMap["9"].x;
				realcenter.y = center.y + ROIRect.y + centerRe.rectMap["9"].y;
				cv::Point2f basePoint = updateOpticalCenter(opticalCenter / binNum, img.size(), roationAngle, mirror);
				double deltaPx = realcenter.x - basePoint.x;
				double deltaPy = realcenter.y - basePoint.y;
				re.V = deltaPy * pixel2deg;
				re.H = deltaPx * pixel2deg;
				double deltaP = sqrt(pow(deltaPx, 2) + pow(deltaPy, 2));
				re.D = deltaP * pixel2deg;
				re.deltxPixel = deltaPx;
				re.deltyPixel = deltaPy;
				updateImgdraw(img_draw, realcenter, binNum);
				updateImgdraw(img_draw, basePoint, binNum);
				putTextOnImage(img_draw, "deltX:(pixel)" + numToString(deltaPx), basePoint + cv::Point2f(0, 300 / binNum), 24 / binNum);
				putTextOnImage(img_draw, "deltY:(pixel)" + numToString(deltaPy), basePoint + cv::Point2f(0, 600 / binNum), 24 / binNum);
				putTextOnImage(img_draw, "deltX:(degree)" + numToString(re.H),
					basePoint + cv::Point2f(0, 900 / binNum), 24 / binNum);
				putTextOnImage(img_draw, "deltY:(degree)" + numToString(re.V),
					basePoint + cv::Point2f(0, 1200 / binNum), 24 / binNum);
				re.imgdraw = img_draw;
			}
		}
	}
	return re;
}

void MLIQMetrics::MLFOVOffset::updateImgdraw(cv::Mat& imgdraw, cv::Point2f h1, int binNum)
{
	circle(imgdraw, h1, 24 / binNum, Scalar(0, 0, 255), -1);
	string xstr = to_string(h1.x);
	string ystr = to_string(h1.y);
	string text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);
	//putTextOnImage(imgdraw, text, h1, 24 / binNum);
	cv::putText(imgdraw, text, h1, FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(0, 255, 0), 8 / binNum);

	//cv::putText(imgdraw, text, h1, FONT_HERSHEY_PLAIN, 24 / binNum, Scalar(0, 255, 0), 8 / binNum);
}

void MLIQMetrics::MLFOVOffset::updateImgdraw(cv::Mat& imgdraw, cv::Point2f pts1, string str, int binNum)
{
	cv::putText(imgdraw, str, pts1, FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(0, 255, 0), 8 / binNum);

}

cv::Point2f MLIQMetrics::MLFOVOffset::getExactLoc(cv::Point2f cen, cv::Mat gray)
{
	cv::Rect rect;
	int len = m_gridROI / m_binNum;
	rect.x = cen.x - len/ 2;
	rect.y = cen.y - len / 2;
	rect.width = len;
	rect.height = len;
	cv::Mat roi = gray(rect).clone();
	CrossCenter cc;
	cv::Point2f c0 = cc.find_centerLINES(roi);
	if (c0.x > 0 && c0.y > 0)
		return c0 + Point2f(rect.tl());
	else
		return cen;
}

void MLIQMetrics::MLFOVOffset::upateFOVOffset(FovOffsetRe& re, bool m_IsSLB)
{
	string fovstr = IQMetricUtl::instance()->fovTypeToString(m_FOVType);
	string filepath = "./config/ALGConfig/" +fovstr+"_"+ m_color + "_offset.csv";
	if (m_IsSLB == true)
	{
		vector<double> rovec;
		rovec.push_back(re.H);
		rovec.push_back(re.V);
		cv::Mat romat(rovec);
		if (m_IsUpdateSLB)
		{
			std::mutex mtx;
			mtx.lock();
			writeMatTOCSV(filepath, romat);
			mtx.unlock();
		}
	}
	else if (m_IsSLB == false)
	{
	
		cv::Mat romat = readCSVToMat(filepath);
		re.H = re.H - romat.at<float>(0, 0) ;
		re.V = re.V - romat.at<float>(1, 0);
		re.D = sqrt(re.H * re.H + re.V * re.V);
	}
}

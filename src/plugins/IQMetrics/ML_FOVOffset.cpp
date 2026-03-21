#include "pch.h"
#include "ML_FOVOffset.h"
#include"CrossCenter.h"
#include"ml_multiCrossHairDetection.h"
#include"ml_gridDetect.h"
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


void MLIQMetrics::MLFOVOffset::setIsSLB(bool flag)
{
	m_IsSLB = flag;
}

void MLIQMetrics::MLFOVOffset::setColor(string color)
{
	m_color = color;
}

void MLIQMetrics::MLFOVOffset::setIsUpdateSLB(bool flag)
{
	m_updateSLB = flag;
}

FovOffsetRe MLIQMetrics::MLFOVOffset::getBoresightGrid(const cv::Mat imgRaw)  
{
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Boresight calculation start");

	string  info = "------getBoresightGrid------";
	FovOffsetRe re;

	if (imgRaw.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is null";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(imgRaw.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	m_binNum = binNum;
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		return re;
	}
	double pixel2Arcmin = IQMetricUtl::instance()->getPix2Arcmin(imgRaw.size());
	cv::Point2f opticalCenter = IQMetricsParameters::opticalCenter / binNum;
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	cv::Mat img = GetROIMat(imgRaw, ROIRect);
	opticalCenter = opticalCenter - Point2f(ROIRect.tl());    // 全局坐标系切换到ROI内的坐标系，因为realcenter是在roi内求的
	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);

	int resizeNum = IQMetricsParameters::GridResizeNum / binNum;
	cv::Mat imgResize;
	cv::resize(img8, imgResize, img8.size() / resizeNum);

	MLGridDetect grid;
	grid.setAccurateDetectionFlag(false);
	grid.SetbinNum(resizeNum);
	//GridRe gridRe = grid.getGridContour(imgResize);
	GridRe gridRe = grid.getGridCenter(imgResize);
	if (gridRe.flag)
	{
		writeFOVOffsetGridCenter(gridRe);
	}
	else
		readFOVOffsetGridCenter(gridRe);

	if (gridRe.flag == false)
	{
		re.flag = false;
		re.errMsg = info + gridRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Cross detection sucessfully");
	cv::Point2f realcenter=gridRe.center*resizeNum;

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
	upateFOVOffset(re,m_IsSLB);
	updateImgdraw(imgdraw, realcenter, binNum,Scalar(255,0,255));
	//updateImgdraw(imgdraw, opticalCenter, binNum);
	string strOpt = numToString(opticalCenter.x) + "," + numToString(opticalCenter.y);
	circle(imgdraw, opticalCenter, 24 / binNum, Scalar(0, 255, 0), -1);
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
	re.crossCenter.clear();
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
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
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
			updateImgdraw(img_draw, realcenter, binNum,Scalar(255,0,255));
			updateImgdraw(img_draw, opticalCenter, binNum, Scalar(255, 0, 0));
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
				int deltaPx = realcenter.x - opticalCenter.x / binNum;
				int deltaPy = realcenter.y - opticalCenter.y / binNum;
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
		int binNum = IQMetricUtl::instance()->getBinNum(img.size());
		cv::Rect ROIRect = IQMetricsParameters::ROIRect;
		cv::Mat roi1 = getRectROIImg(img, ROIRect).clone();
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
				int deltaPx = realcenter.x - basePoint.x;
				int deltaPy = realcenter.y - basePoint.y;
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
		int binNum = IQMetricUtl::instance()->getBinNum(img.size());
		cv::Rect ROIRect = IQMetricsParameters::ROIRect;
		cv::Mat roi1 = getRectROIImg(img8, ROIRect);
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
				int deltaPx = realcenter.x - basePoint.x;
				int deltaPy = realcenter.y - basePoint.y;
				re.V = deltaPy * pixel2deg;
				re.H = deltaPx * pixel2deg;
				double deltaP = sqrt(pow(deltaPx, 2) + pow(deltaPy, 2));
				re.D = deltaP * pixel2deg;
				re.deltxPixel = deltaPx;
				re.deltyPixel = deltaPy;
				updateImgdraw(img_draw, realcenter, binNum,Scalar(255,0,255));
				updateImgdraw(img_draw, basePoint, binNum, Scalar(255, 0, 0));
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

void MLIQMetrics::MLFOVOffset::updateImgdraw(cv::Mat& imgdraw, cv::Point2f h1, int binNum,cv::Scalar color)
{
	circle(imgdraw, h1, 24 / binNum, color, -1);
	string xstr = to_string(h1.x);
	string ystr = to_string(h1.y);
	string text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);
	//putTextOnImage(imgdraw, text, h1, 24 / binNum);
	cv::putText(imgdraw, text, h1, FONT_HERSHEY_PLAIN, 16 / binNum, color, 8 / binNum);
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

void MLIQMetrics::MLFOVOffset::upateFOVOffset(FovOffsetRe& re, bool IsSLB)
{
	string filepath = "./config/AlgConfig/slbInfo/offset_" + m_color + ".csv";

	if (IsSLB == true)
	{
		vector<double> rovec;
		rovec.push_back(re.H);
		rovec.push_back(re.V);
		cv::Mat romat(rovec);	
			std::mutex mtx;
			mtx.lock();
			writeMatTOCSV(filepath, romat);
			mtx.unlock();		
	}
	else if (IsSLB == false)
	{

		cv::Mat romat = readCSVToMat(filepath);
		re.H = re.H - romat.at<float>(0, 0);
		re.V = re.V - romat.at<float>(1, 0);
		re.D = sqrt(re.H * re.H + re.V * re.V);
	}
}

void MLIQMetrics::MLFOVOffset::writeFOVOffsetGridCenter(GridRe re)
{
	string filepath = "./config/AlgConfig/slbInfo/FOVoffsetCen.csv";
	vector<double>cenVec;
	cenVec.push_back(re.center.x);
	cenVec.push_back(re.center.y);
	cv::Mat romat(cenVec);
	std::mutex mtx;
	mtx.lock();
	writeMatTOCSV(filepath, romat);	
	mtx.unlock();

}

void MLIQMetrics::MLFOVOffset::readFOVOffsetGridCenter(GridRe& re)
{
	string filepath = "./config/AlgConfig/slbInfo/FOVoffsetCen.csv";
	cv::Mat cenmat = readCSVToMat(filepath);
	re.center.x = cenmat.at<float>(0, 0);
	re.center.y = cenmat.at<float>(1, 0);
	re.flag = true;
}

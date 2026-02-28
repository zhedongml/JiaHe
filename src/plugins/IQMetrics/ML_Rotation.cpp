#include "pch.h"
#include "ML_Rotation.h"
#include"ml_multiCrossHairDetection.h"
#include"LogPlus.h"
#include"ml_gridDetect.h"
#include"CrossCenter.h"
#include"ML_NineCrossDetection.h"
using namespace MLImageDetection;
using namespace MLIQMetrics;
using namespace cv;
MLIQMetrics::MLRotation::MLRotation()
{
}

MLIQMetrics::MLRotation::~MLRotation()
{
}

void MLIQMetrics::MLRotation::setIsUpdateSLB(bool flag)
{
	m_IsUpdateSLB = flag;
}

void MLIQMetrics::MLRotation::setIsSLB(bool flag)
{
	m_IsSLB = flag;
}

void MLIQMetrics::MLRotation::setFOVType(FOVTYPE type)
{
	m_FOVType = type;
}

void MLIQMetrics::MLRotation::setColor(string color)
{
	m_color = color;
}

RotationRe MLIQMetrics::MLRotation::getGridRotation(cv::Mat img)
{

	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Cross Rotation calculation start");

	string info = "------getCrossRotationGusu------";
	RotationRe re;
	double rotation = -1;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "input image is null";
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
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	img = GetROIMat(img, ROIRect);
	cv::Mat img8 = convertToUint8(img);
	cv::Mat img_draw = convertTo3Channels(img8);

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
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "image detection sucessfully");

	if (gridRe.flag)
	{
		int rows = gridRe.xLocMat.rows - 1;
		int cols = gridRe.xLocMat.cols - 1;
		cv::Point2f cenLeft, cenRight;
		cenLeft.x = gridRe.xLocMat.at<float>(rows / 2, 0) * resizeNum;
		cenLeft.y = gridRe.yLocMat.at<float>(rows / 2, 0) * resizeNum;
		cenRight.x = gridRe.xLocMat.at<float>(rows / 2, cols) * resizeNum;
		cenRight.y = gridRe.yLocMat.at<float>(rows / 2, cols) * resizeNum;
		if (accurateFlag)
		{
			updateGridPoint(cenLeft, img8);
			updateGridPoint(cenRight, img8);
		}

		vector<cv::Point2f> rotaionPts;
		if (cenLeft.x > 1e-6 && cenLeft.y > 1e-6 && cenRight.x > 1e-6 && cenRight.y > 1e-6)
		{
			rotaionPts.clear();
			rotaionPts.push_back(cenLeft);
			rotaionPts.push_back(cenRight);
			re.rotaionPts = rotaionPts;
			updateImgdraw(img_draw, cenLeft, binNum);
			updateImgdraw(img_draw, cenRight, binNum);
			double deltaPixelX = cenLeft.x - cenRight.x;
			double deltaPixelY = cenLeft.y - cenRight.y;
			rotation = atan(deltaPixelY / deltaPixelX) * 180.0 / CV_PI;
			if (m_IsSLB)
			{
				writeMatTOCSV("./config/ALGConfig/rotation.csv", cv::Mat(1, 1, CV_32FC1, rotation));
			}
			else
			{
				cv::Mat romat = readCSVToMat("./config/ALGConfig/rotation.csv");
				rotation = rotation - romat.at<float>(0, 0);
			}
			string stry = "Delty(pixel):" + to_string(deltaPixelY);
			string strx = "Deltx(pixel):" + to_string(deltaPixelX);
			string strR = "RoationAngle(degree):" + to_string(rotation);
			updateImgdraw(img_draw, cenLeft + Point2f(0, 300 / binNum), stry, binNum);
			updateImgdraw(img_draw, cenLeft + Point2f(0, 600 / binNum), strx, binNum);
			updateImgdraw(img_draw, cenLeft + Point2f(0, 900 / binNum), strR, binNum);
			re.rotaionPts.push_back(cenLeft);
			re.rotaionPts.push_back(cenRight);
			re.imgdraw = img_draw;
			re.angle = rotation;
		}
		else
		{
			re.flag = false;
			re.errMsg = info + " image detection fail";
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
			return re;
		}
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, " Rotation calculation successfully");
	return re;


}

RotationRe MLIQMetrics::MLRotation::getCrossRotationGusu(cv::Mat img)
{
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Cross Rotation calculation start");

	string info = "------getCrossRotationGusu------";
	RotationRe re;
	double rotation = -1;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "input image is null";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());

	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		return re;
	}
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	img = GetROIMat(img, ROIRect);
	cv::Mat img8 = convertToUint8(img);
	cv::Mat img_draw = convertTo3Channels(img8);
	MultiCrossHairDetection md;
	md.setBinNum(binNum);
	MultiCrossHairRe cenRe = md.getMuliCrossHairCenter(img8, IQMetricsParameters::crossBinNum / binNum, false);
	if (cenRe.flag == false)
	{
		re.flag = false;
		re.errMsg = info + cenRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Cross detection sucessfully");

	if (cenRe.flag)
	{
		//cv::Mat img_draw = cenRe.img_draw;
		cv::Point2f cL = cenRe.centerMap["4"];
		cv::Rect rectL(cL.x - m_crossROI / binNum / 2, cL.y - m_crossROI / binNum / 2, m_crossROI / binNum, m_crossROI / binNum);
		cv::Mat lMat = img8(rectL);
		cv::Point2f cR = cenRe.centerMap["6"];
		cv::Rect rectR(cR.x - m_crossROI / binNum / 2, cR.y - m_crossROI / binNum / 2, m_crossROI / binNum, m_crossROI / binNum);
		cv::Mat rMat = img8(rectR);
		CrossCenter cc;
		cv::Point2f lcen, rcen;
		lcen = cc.find_centerLINES(lMat);
		rcen = cc.find_centerLINES(rMat);
		//if (isSLB)
		//{
		//	lcen = cc.find_centerLINES(lMat);
		//	rcen = cc.find_centerLINES(rMat);
		//}
		//else
		//{
		//	lcen = cc.find_centerGaussian(lMat, false);
		//	rcen = cc.find_centerGaussian(rMat, false);
		//}
		vector<cv::Point2f> rotaionPts;
		if (lcen.x > 1e-6 && lcen.y > 1e-6 && rcen.x > 1e-6 && rcen.y > 1e-6)
		{
			lcen = lcen + Point2f(rectL.tl());
			rcen = rcen + Point2f(rectR.tl());
			rotaionPts.clear();
			rotaionPts.push_back(lcen);
			rotaionPts.push_back(rcen);
			re.rotaionPts = rotaionPts;
			updateImgdraw(img_draw, lcen, binNum);
			updateImgdraw(img_draw, rcen, binNum);
			double deltaPixelX = lcen.x - rcen.x;
			double deltaPixelY = lcen.y - rcen.y;
			rotation = atan(deltaPixelY / deltaPixelX) * 180.0 / CV_PI;
			string stry = "Delty(pixel):" + to_string(deltaPixelY);
			string strx = "Deltx(pixel):" + to_string(deltaPixelX);
			string strR = "RoationAngle(degree):" + to_string(rotation);
			updateImgdraw(img_draw, lcen + Point2f(0, 300 / binNum), stry, binNum);
			updateImgdraw(img_draw, lcen + Point2f(0, 600 / binNum), strx, binNum);
			updateImgdraw(img_draw, lcen + Point2f(0, 900 / binNum), strR, binNum);
			re.rotaionPts.push_back(lcen);
			re.rotaionPts.push_back(rcen);

			re.imgdraw = img_draw;
			re.angle = rotation;
			return re;
		}
		else
		{
			re.flag = false;
			re.errMsg = info + "crosshair detection fail";
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
			return re;
		}
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Cross Rotation calculation successfully");
	return re;
}

RotationRe MLIQMetrics::MLRotation::getNineCrossRotation(const cv::Mat img)
{

	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Cross Rotation calculation start");
	string info = "------getCrossRotationGusu------";
	RotationRe re;
	double rotation = -1;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "input image is null";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	m_binNum = binNum;
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		return re;
	}
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	if (m_FOVType == BIGFOV)
		ROIRect = cv::Rect(0, 0, -1, -1);
	cv::Mat imgROI = GetROIMat(img, ROIRect);
	cv::Mat img8 = convertToUint8(imgROI);
	cv::Mat img_draw = convertTo3Channels(img8);
	MLNineCrossDetection cross;
	cross.setBinNum(binNum);
	map<string, cv::Point2f>cenMap = cross.getNineCrosshairLocation(img8, m_FOVType);
	map<string, cv::Rect>cenRectMap = cross.getNineCrossRect();
	if (cenMap.size() != 9)
	{
		re.flag = false;
		re.errMsg = info + "nine crosshair detection fail";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "image detection sucessfully");
	cv::Rect rectL = cenRectMap["4"];
	cv::Rect rectR = cenRectMap["6"];
	cv::Point2f cenLeft, cenRight;
	CrossCenter cc;
	//cv::Point2f cenLeft = cc.find_centerGaussian(img8(rectL),false);
	//cv::Point2f cenRight = cc.find_centerGaussian(img8(rectR), false);
	if (m_IsSLB)
	{
		 cenLeft = cc.find_centerLINES(img8(rectL));
		 cenRight = cc.find_centerLINES(img8(rectR));
	}
	else
	{
		cenLeft = cc.find_centerGaussian(img8(rectL),false);
		cenRight = cc.find_centerGaussian(img8(rectR),false);
	}

	vector<cv::Point2f> rotaionPts;
	if (cenLeft.x > 1e-6 && cenLeft.y > 1e-6 && cenRight.x > 1e-6 && cenRight.y > 1e-6)
	{
		cenLeft = cenLeft + cv::Point2f(rectL.tl());
		cenRight = cenRight + cv::Point2f(rectR.tl());
		rotaionPts.clear();
		rotaionPts.push_back(cenLeft);
		rotaionPts.push_back(cenRight);
		re.rotaionPts = rotaionPts;
		updateImgdraw(img_draw, cenLeft, binNum);
		updateImgdraw(img_draw, cenRight, binNum);
		double deltaPixelX = cenLeft.x - cenRight.x;
		double deltaPixelY = cenLeft.y - cenRight.y;
		rotation = atan(deltaPixelY / deltaPixelX) * 180.0 / CV_PI;
		string stry = "Delty(pixel):" + to_string(deltaPixelY);
		string strx = "Deltx(pixel):" + to_string(deltaPixelX);
		string strR = "RoationAngle(degree):" + to_string(rotation);
		updateImgdraw(img_draw, cenLeft + Point2f(0, 300 / binNum), stry, binNum);
		updateImgdraw(img_draw, cenLeft + Point2f(0, 600 / binNum), strx, binNum);
		updateImgdraw(img_draw, cenLeft + Point2f(0, 900 / binNum), strR, binNum);
		//updateRotationRe(rotation, m_IsSLB);
		if (m_IsSLB == false&&m_FOVType==SMALLFOV)
			rotation = rotation - IQMetricsParameters::DUTRotationAngle;
		re.rotaionPts.push_back(cenLeft);
		re.rotaionPts.push_back(cenRight);
		re.imgdraw = img_draw;
		re.angle = rotation;
	}
	else
	{
		re.flag = false;
		re.errMsg = info + " image detection fail";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}

	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, " Rotation calculation successfully");
	return re;




}

RotationRe MLIQMetrics::MLRotation::getMultiCrossRotation(cv::Mat img, int eyeLoc)
{
	RotationRe re;
	double rotation = -1;
	if (img.data != NULL)
	{
		cv::Rect rec, rect;
		cv::Mat img8 = convertToUint8(img);
		cv::Mat img_draw = convertTo3Channels(img8);
		int binNum = IQMetricUtl::instance()->getBinNum(img.size());

		MultiCrossHairDetection md;
		vector<cv::Point2f> ptsVec;
		// MultiCrossHairRe cenRe = md.getMuliCrossHairCenterByDistance(img8, binNum);
		MultiCrossHairRe cenRe = md.getMuliCrossHairCenterByDistance(img8, eyeLoc, binNum);

		if (cenRe.flag)
		{
			cv::Mat lMat = img8(cenRe.rectMap["1"]).clone();
			cv::Mat rMat = img8(cenRe.rectMap["2"]).clone();
			CrossCenter cc;
			cv::Point2f lcen, rcen;

			lcen = cc.find_centerLINES(lMat);
			rcen = cc.find_centerLINES(rMat);


			vector<cv::Point2f> rotaionPts;
			int binNum = IQMetricUtl::instance()->getBinNum(img.size());
			if (lcen.x > 1e-6 && lcen.y > 1e-6 && rcen.x > 1e-6 && rcen.y > 1e-6)
			{
				lcen = lcen + Point2f(cenRe.rectMap["1"].x, cenRe.rectMap["1"].y);
				rcen = rcen + Point2f(cenRe.rectMap["2"].x, cenRe.rectMap["2"].y);
				drawRectOnImage(img_draw, cenRe.rectMap["1"]);
				drawRectOnImage(img_draw, cenRe.rectMap["2"]);

				rotaionPts.clear();
				rotaionPts.push_back(lcen);
				rotaionPts.push_back(rcen);
				re.rotaionPts = rotaionPts;
				updateImgdraw(img_draw, lcen, binNum);
				updateImgdraw(img_draw, rcen, binNum);
				double deltaPixelX = lcen.x - rcen.x;
				double deltaPixelY = lcen.y - rcen.y;
				rotation = atan(deltaPixelY / deltaPixelX) * 180.0 / CV_PI;
				putTextOnImage(img_draw, "Rotation:(degree)" + to_string(rotation), lcen, 24 / binNum);
			}
		}

		re.imgdraw = img_draw;
	}
	re.angle = rotation;
	return re;
}

void MLIQMetrics::MLRotation::updateGridPoint(cv::Point2f& cen, cv::Mat img)
{
	cv::Rect rect;
	int len = m_gridROI / m_binNum;
	rect.x = cen.x - len / 2;
	rect.y = cen.y - len / 2;
	rect.width = len;
	rect.height = len;
	cv::Mat roi = img(rect).clone();
	CrossCenter cc;
	cv::Point2f c0 = cc.find_centerLINES(roi);
	if (c0.x > 0 && c0.y > 0)
		cen = c0 + Point2f(rect.tl());
}

void MLIQMetrics::MLRotation::updateImgdraw(cv::Mat& imgdraw, cv::Point2f lcen, int binNum)
{
	//circle(imgdraw, lcen, 16 / binNum, Scalar(0, 0, 255), -1);
	circle(imgdraw, lcen, 1, Scalar(0, 0, 255), -1);

	string xstr = to_string(lcen.x);
	string ystr = to_string(lcen.y);
	string text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);
	cv::putText(imgdraw, text, lcen + Point2f(0, -10), FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(0, 255, 0), 16 / binNum);
}

void MLIQMetrics::MLRotation::updateImgdraw(cv::Mat& imgdraw, cv::Point2f pts1, string str, int binNum)
{
	cv::putText(imgdraw, str, pts1, FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(0, 255, 0), 16 / binNum);
}

void MLIQMetrics::MLRotation::updateRotationRe(double& rotation, bool m_IsSLB)
{
	string fovstr = IQMetricUtl::instance()->fovTypeToString(m_FOVType);
	string filepath = "./config/ALGConfig/" + fovstr + "_" + m_color + "_rotation.csv";
	if (m_IsSLB)
	{
		if(m_IsUpdateSLB)
		writeMatTOCSV(filepath, cv::Mat(1, 1, CV_32FC1, rotation));
	}
	else
	{
		cv::Mat romat = readCSVToMat(filepath);
		rotation = rotation - romat.at<float>(0, 0);
	}
}

#include "pch.h"
#include "MLContrastRatio.h"
#include "MLCherkerboardDetect.h"
#include"LogPlus.h"
#include<numeric>
using namespace MLImageDetection;
using namespace cv;
using namespace MLIQMetrics;

MLContrastRatio::MLContrastRatio()
{
}
MLContrastRatio::~MLContrastRatio()
{
}

std::vector<double> MLContrastRatio::CalculateROIValue(cv::Mat img, vector<cv::Rect> roiVec)
{
	vector<double> crVec;
	if (img.data != NULL)
	{
		for (int i = 0; i < roiVec.size(); i++)
		{
			double cr = 1e-9;
			bool is_rect = (0 <= roiVec[i].x && 0 < roiVec[i].width && roiVec[i].x + roiVec[i].width < img.cols &&
				0 <= roiVec[i].y && 0 < roiVec[i].height && roiVec[i].y + roiVec[i].height < img.rows);
			if (is_rect)
			{
				cv::Mat posROI = img(roiVec[i]);
				if (posROI.data != NULL)
				{
					cv::Scalar mean = cv::mean(posROI);
					cr = mean(0);
				}
				// else
				//{
				//	cout << roiVec[i] << endl;
				if (cr < 0)
				{
					cr = 1e-9;
				}
			}
			crVec.push_back(cr);
		}
	}
	return crVec;
}

std::vector<double> MLContrastRatio::CalculateCR(vector<double> posValue, vector<double> negValue)
{
	vector<double> crVec;
	if (posValue.size() == negValue.size())
	{
		for (int i = 0; i < posValue.size(); i++)
		{
			if (posValue[i] >= negValue[i] & negValue[i] > 1e-6)
			{
				crVec.push_back(posValue[i] / negValue[i]);
			}
			else if (negValue[i] >= posValue[i] & posValue[i] > 1e-6)
			{
				crVec.push_back(negValue[i] / posValue[i]);
			}
			else
			{
				crVec.push_back(1e-9);
			}
		}
	}
	return crVec;
}

double MLIQMetrics::MLContrastRatio::CalculateSingleCR(vector<double> posValue)
{
	double m0 = accumulate(posValue.begin(), posValue.end(), 0.0) / posValue.size();
	double sumP = 0;
	double sumN = 0;
	for (int i = 0; i < posValue.size(); i++)
	{
		if (posValue[i] > m0)
			sumP = sumP + posValue[i];
		else
			sumN = sumN + posValue[i];
	}

	return sumP / (sumN + 1e-6);
}

cv::Rect MLIQMetrics::MLContrastRatio::updateRectByBinNum(cv::Rect rect, int binNum)
{
	cv::Rect rectBin;
	rectBin.x = rect.x / binNum;
	rectBin.y = rect.y / binNum;
	rectBin.width = rect.width / binNum;
	rectBin.height = rect.height / binNum;
	return rectBin;
}

cv::Mat MLIQMetrics::MLContrastRatio::updateMatValue(vector<double> crVec, cv::Size boardSize)
{
	cv::Mat crMat(boardSize, CV_32FC1, Scalar(0));
	int i = 0;
	for (int n = 0; n < boardSize.height; n++)
	{
		for (int m = 0; m < boardSize.width; m++)
		{
			crMat.at<float>(n, m) = crVec[i];
			i++;
		}
	}
	return crMat;
}

void MLIQMetrics::MLContrastRatio::updateImgdraw(vector<cv::Rect> rectVec, vector<double> posV, vector<double> negV, vector<double> crV, cv::Mat& img_drawP, cv::Mat& img_drawN)
{
	for (int i = 0; i < rectVec.size(); i++)
	{

		//if (containRect(rect, checkerRe.rectVec[i]))
		{
			cv::Point posP, posN;
			posP = rectVec[i].tl() - cv::Point(60 / m_binNum, 0);// cv::Point(rectVec[i].x, rectVec[i].y);
			posN = rectVec[i].tl() - cv::Point(60 / m_binNum, 0); // cv::Point(rectVec[i].x, rectVec[i].y);
			double cr = crV[i];
			//if (cr < 2 || cr > 500)
			//	continue;
			//  drawN++;
			cv::rectangle(img_drawP, rectVec[i], cv::Scalar(0, 0, 255), 10 / m_binNum);
			cv::rectangle(img_drawN, rectVec[i], cv::Scalar(0, 0, 255), 10 / m_binNum);
			// circle(img_drawP, centerVecP[i], 2, Scalar(0, 0, 255), -1);
			string strP = numToString(posV[i]);//to_string(posV[i]);
			string strN = numToString(negV[i]); //to_string(negV[i]);
			string strCR = numToString(crV[i]); //to_string(cr);
			//putText(img_drawP, strP, posP, FONT_HERSHEY_PLAIN, 4 / m_binNum,
			//	Scalar(0, 255, 0), 4 / m_binNum);
			//putText(img_drawN, strN, posN, FONT_HERSHEY_PLAIN, 4 / m_binNum,
			//	Scalar(0, 255, 0), 4 / m_binNum);
			//putText(img_drawP, strCR, posN + cv::Point(0, 50/m_binNum), FONT_HERSHEY_PLAIN,
			//	4 / m_binNum, Scalar(0, 255, 255), 4 / m_binNum);
			//putText(img_drawN, strCR, posP + cv::Point(0, 50/m_binNum), FONT_HERSHEY_PLAIN,
			//	4 / m_binNum, Scalar(0, 255, 255), 4 / m_binNum);
		}
	}

}

void MLIQMetrics::MLContrastRatio::updateImgdraw(cv::Rect rect, cv::Mat& img_drawP, cv::Mat& img_drawN)
{
	cv::rectangle(img_drawP, rect, Scalar(0, 0, 255), 5);
	cv::rectangle(img_drawN, rect, Scalar(0, 0, 255), 5);
}

void MLIQMetrics::MLContrastRatio::updateImgdraw(vector<cv::Rect> rectVec, vector<double> posV, cv::Mat& imgdraw)
{
	if (rectVec.size() != posV.size())
		return;
	for (int i = 0; i < rectVec.size(); i++)
	{
		cv::rectangle(imgdraw, rectVec[i], cv::Scalar(0, 0, 255), 5);
		string strP = numToString(posV[i]);//to_string(posV[i]);
		cv::Point posP = rectVec[i].tl() - cv::Point(60 / m_binNum, 0);
		putText(imgdraw, strP, posP, FONT_HERSHEY_PLAIN, 20 / m_binNum,
			Scalar(0, 255, 0), 20 / m_binNum);
	}
}

vector<double> MLIQMetrics::MLContrastRatio::getROICR(cv::Rect rect, vector<cv::Rect> rectVec, vector<double> crVec)
{
	vector<double>crROIVec;
	for (int i = 0; i < rectVec.size(); i++)
	{
		if (containRect(rect, rectVec[i]))
		{
			crROIVec.push_back(crVec[i]);
		}
	}
	return crROIVec;
}

vector<double> MLIQMetrics::MLContrastRatio::getROICR(vector<cv::Rect> rectVec, cv::Point2f cen, vector<double> crVec, vector<double>& crVecZoneB)
{
	double zoneAR = IQMetricsParameters::zoneARadius;
	double pixel = IQMetricsParameters::pixel_size * m_binNum;
	double focallengh = IQMetricsParameters::FocalLength;
	vector<double> crVecZoneA;
	for (int i = 0; i < rectVec.size(); i++)
	{

		double dis = Getdistance(rectVec[i].tl(), cen);
		double fov = atan(dis * pixel / focallengh) * 180 / CV_PI;
		if (fov < zoneAR)
			crVecZoneA.push_back(crVec[i]);
		else
			crVecZoneB.push_back(crVec[i]);
	}
	return crVecZoneA;
}

vector<double> MLIQMetrics::MLContrastRatio::getROICR(vector<cv::Rect> rectVec, cv::Rect rect, vector<double> crVec, vector<double>& crVecOutMask)
{
	vector<double> crVecInMask;
	for (int i = 0; i < rectVec.size(); i++)
	{
		if (rect.contains(rectVec[i].tl())&&rect.contains(rectVec[i].br()))
			crVecInMask.push_back(crVec[i]);
		else
			crVecOutMask.push_back(crVec[i]);
			
	}
	return crVecInMask;
}

double MLIQMetrics::MLContrastRatio::getHarmonicMean(vector<double> crVec)
{
	double sum = 0;
	for (int i = 0; i < crVec.size(); i++)
	{
		double tmp;
		if (crVec[i] <= 0)
			tmp = 1 / 1e-6;
		else
			tmp = 1.0 / crVec[i];
		sum = sum + tmp;
	}
	double meanH = crVec.size() / sum;
	return meanH;
}
void MLIQMetrics::MLContrastRatio::setColor(string color)
{
	m_color = color;
}
void MLIQMetrics::MLContrastRatio::setEyeboxLocation(string eyebox)
{
	m_eyebox = eyebox;
}
void MLIQMetrics::MLContrastRatio::readCRMatFromCSV(cv::Mat& crmat, string color, string id)
{
	string dir = get_exe_dir() + m_slbCRDir + color + "_" + id + "_slbCR.csv";
	crmat = readCSVToMat(dir);
}

void MLIQMetrics::MLContrastRatio::readCRMatFromCSV(cv::Mat& crmat, string color, string id, FOVTYPE type)
{
	string fovstr = IQMetricUtl::instance()->fovTypeToString(type);
	string dir = "./config/ALGConfig/" + fovstr+"_"+ color + "_slbCR.csv";
	crmat = readCSVToMat(dir);
}

void MLIQMetrics::MLContrastRatio::saveSLBCRMatToCSV(const cv::Mat crmat, string color, string id, FOVTYPE type)
{
	string fovstr = IQMetricUtl::instance()->fovTypeToString(type);
	string dir = "./config/ALGConfig/" + fovstr + "_" + color + "_slbCR.csv";
	writeMatTOCSV(dir,crmat);
}

void MLIQMetrics::MLContrastRatio::readCRMatFromCSV(cv::Mat& crmat, string color)
{
	string path = "./config/ALGConfig/" + color + "_SLBCR.csv";
	crmat = readCSVToMat(path);
}

void MLIQMetrics::MLContrastRatio::readROICRFromCSV(vector<double>& crmat, string color, string id)
{
	string dir = get_exe_dir() + m_slbCRDir + color + "_" + id + "_slbROICR.csv";
	crmat = readCSVToMat(dir);
}

cv::Mat MLIQMetrics::MLContrastRatio::updateDutCRBySlbCR(cv::Mat dutcr, cv::Mat slbcr)
{
	if (slbcr.data == NULL)
		return dutcr;
	if (slbcr.size() != dutcr.size())
		return dutcr;
	cv::Mat crMat(dutcr.size(), dutcr.type(), Scalar(-1));
	for (int i = 0; i < dutcr.rows; i++)
	{
		for (int j = 0; j < dutcr.cols; j++)
		{
			float cr = 1.0 / (dutcr.at<float>(i, j)) - 1.0 / (slbcr.at<float>(i, j));
			crMat.at<float>(i, j) = 1.0 / (cr + 1e-6);
		}
	}
	return crMat;
}

vector<double> MLIQMetrics::MLContrastRatio::updateDutCRBySlbCR(vector<double> dutcr, vector<double> slbcr)
{
	vector<double> dutCali;
	if (slbcr.size() < 1)
		return dutcr;
	if (dutcr.size() != slbcr.size())
		return dutcr;
	for (int i = 0; i < slbcr.size(); i++)
	{
		float cr = 1.0 / (dutcr[i]) - 1.0 / (slbcr[i]);
		dutCali.push_back(1.0 / (cr + 1e-6));
	}
	return dutCali;
}

cv::RotatedRect MLIQMetrics::MLContrastRatio::getCherkerBorder(cv::Mat img, cv::Rect& rect)
{
	cv::RotatedRect rectR;
	if (img.data != NULL)
	{
		cv::Mat img8 = convertToUint8(img);
		cv::Mat imgdraw = convertTo3Channels(img8);
		cv::Mat preImg = preProcess(img8);
		cv::Mat srcbinary;
		threshold(preImg, srcbinary, 0, 255, THRESH_OTSU);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(40, 40), Point(-1, -1));
		morphologyEx(srcbinary, srcbinary, MORPH_DILATE, kernel, Point(-1, -1));

		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(srcbinary, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		vector<double> area_sort;
		vector<int> list;
		cv::Mat roi;
		vector<double> areaVec;
		for (int i = 0; i < contours.size(); ++i)
		{
			double area = contourArea(contours[i], false);
			int binNum = IQMetricUtl::instance()->getBinNum(img.size());
			double powbin = pow(binNum, 2);
			rect = cv::boundingRect(contours[i]);
			double maxLen = max(rect.width, rect.height);
			double minLen = min(rect.width, rect.height);
			double ratio = minLen / maxLen;
			if (area > 2e5 /*&& ratio > 0.8 && ratio < 1.2*/)
			{

				std::vector<cv::Point> approx;
				double epsilon = 0.02 * cv::arcLength(contours[i], true); // 近似精度
				cv::approxPolyDP(contours[i], approx, epsilon, true);
				// 绘制近似多边形
				cv::polylines(imgdraw, approx, true, cv::Scalar(0, 255, 0), 2);

				rectR = cv::minAreaRect(contours[i]);
				// center = rectR.center;
				cv::rectangle(imgdraw, rect, Scalar(255, 0, 255), 5);
				break;
			}
		}
	}
	return rectR;
}

cv::Mat MLIQMetrics::MLContrastRatio::preProcess(cv::Mat gray)
{
	//cv::medianBlur(gray, gray, 3);
	//cv::Mat  gray8 = convertToUint8(gray);
	cv::Mat blur;
	cv::GaussianBlur(gray, blur, Size(3, 3), 1, 0);
	Ptr<CLAHE> clahe = createCLAHE(2.0, Size(5, 5));
	clahe->apply(blur, blur);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	//morphologyEx(gray, gray, MORPH_CLOSE, kernel, Point(-1, -1));
	morphologyEx(blur, blur, MORPH_GRADIENT, kernel, Point(-1, -1));
	return blur;
}

void MLIQMetrics::MLContrastRatio::setFOVType(FOVTYPE type)
{
	m_fovType = type;
}

void MLIQMetrics::MLContrastRatio::setIsUpdateSLB(bool flag)
{
	m_IsUpdateSLB = flag;
}

ContrastRatioRe MLIQMetrics::MLContrastRatio::getContrastRatio(const cv::Mat imgP, const cv::Mat imgN, bool isSLB, MaskType type)
{
	string info = "------getContrastRatio------";
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "getContrastRatio calculate start");
	ContrastRatioRe re;
	if (imgP.data != NULL && imgN.data != NULL)
	{
		int binNum = IQMetricUtl::instance()->getBinNum(imgP.size());
		cv::Rect ROIRect = IQMetricsParameters::ROIRect;
		updateRectByRatio1(ROIRect, 1.0 / binNum);
		if (m_fovType == BIGFOV)
			ROIRect = cv::Rect(0, 0, -1, -1);
		cv::Mat imgPROI = GetROIMat(imgP,ROIRect);
		cv::Mat imgNROI = GetROIMat(imgN, ROIRect);
		cv::Rect rect;
		cv::RotatedRect rectR = getCherkerBorder(imgPROI, rect);
		updateRotateImg(imgPROI, rectR.angle);
		updateRotateImg(imgNROI, rectR.angle);
		cv::Mat imgP8 = convertToUint8(imgPROI);
		cv::Mat imgN8 = convertToUint8(imgNROI);
		cv::Rect rectAfterRo = updateRotateRect(rectR, rectR.center);
		rectAfterRo = updateRectByRatio(rectAfterRo, 1.4);
		cv::Rect rectAll(0, 0, imgPROI.cols, imgPROI.rows);
		imgPROI= imgPROI(rectAfterRo & rectAll);
		imgNROI = imgNROI(rectAfterRo & rectAll);
		imgP8 = imgP8(rectAfterRo & rectAll);
		imgN8 = imgN8(rectAfterRo & rectAll);
		cv::Mat img_drawP = convertTo3Channels(imgP8);
		cv::Mat img_drawN = convertTo3Channels(imgN8);
		MLCherkerboardDetect cb;
		double crRation = IQMetricsParameters::crArea;
		if (binNum <= 0)
		{
			re.flag = false;
			re.errMsg = info + "the image size is not right, please check the input image";
			return re;
		}
		m_binNum = binNum;
		cb.SetCRAreaPercent(crRation);
		cb.SetChessboardUpdateFlag(false);
		cb.SetChecssboardPointsClusters(200 / binNum);
		cb.SetChessboardxyClassification(350 / binNum);
		cb.SetChessboardUpdateFlag(true);
		//CheckerboardRe checkerRe = cb.detectChessboardTemplate1(imgP8, crRation, binNum);
		CheckerboardRe checkerRe=cb.detectChessboardTemplate1(imgP8, crRation);
		//if(checkerRe.flag==false)
		//	checkerRe = cb.detectChessboardTemplate1(imgP8, crRation, binNum);
		//checkerRe = cd.detectChessboardCorner1(imgP8, crRation, 4);
	   // checkerRe = cd.detectChessboardContour(imgP8, crRation, 4);
		if (checkerRe.flag == false)
		{
			re.flag = false;
			re.errMsg = info + checkerRe.errMsg;
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
			return re;
		}
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "checker detection successfully");

		// draw
		//drawPointsOnImage(img_drawN, checkerRe.pts);
		//drawPointsOnImage(img_drawP, checkerRe.pts);
		vector<double> posValue, negValue;
		posValue = CalculateROIValue(imgPROI, checkerRe.rectVec);
		negValue = CalculateROIValue(imgNROI, checkerRe.rectVec);
		vector<double> crVec = CalculateCR(posValue, negValue);
		cv::Size boardSize = checkerRe.boardSize - cv::Size(1, 1);
		cv::Mat crAll = updateMatValue(crVec, boardSize);
		cv::Mat grayLevelP = updateMatValue(posValue, boardSize);
		cv::Mat grayLevelN = updateMatValue(negValue, boardSize);
		updateImgdraw(checkerRe.rectVec, posValue, negValue, crVec, img_drawP, img_drawN);
		double minCR = 0, maxCR = 0, meanCR = 0;
		if (isSLB == false)
		{
			cv::Mat slbcrmat;
			//readCRMatFromCSV(slbcrmat, m_color, m_eyebox);
			readCRMatFromCSV(slbcrmat, m_color, m_eyebox,m_fovType);
			cv::Mat dutCali = updateDutCRBySlbCR(crAll, slbcrmat);
			crAll = dutCali;
		}
		else if(isSLB&& m_IsUpdateSLB)
		{
			std::mutex mtx;
			mtx.lock();
			saveSLBCRMatToCSV(crAll, m_color, m_eyebox, m_fovType);
			mtx.unlock();
		}
		vector<double>crInMask;
		vector<double>crOutMask;
		if (type == CIRCLE)
		{
			double zoneAR = IQMetricsParameters::CheckerCIRCLEMaskRadius;
			double pixel = IQMetricsParameters::pixel_size;
			double focallengh = IQMetricsParameters::FocalLength;
			double r = focallengh * tan(zoneAR / 180.0 * CV_PI) / (pixel * binNum);
			cv::Point2f center = checkerRe.center;
			circle(img_drawN, center, r, Scalar(0, 255, 0), 16 / binNum);
			circle(img_drawP, center, r, Scalar(0, 255, 0), 16 / binNum);
			crInMask = getROICR(checkerRe.rectVec, center, crVec, crOutMask);

			 minCR = *min_element(crInMask.begin(), crInMask.end());
			 maxCR = *max_element(crInMask.begin(), crInMask.end());
			double sum = accumulate(crInMask.begin(), crInMask.end(), 0.0);
			 meanCR= sum / crInMask.size();
		}
		else if (type == RECT)
		{
			double hor = IQMetricsParameters::CheckerRECTMaskHor;
			double ver = IQMetricsParameters::CheckerRECTMaskVer;
			double pixel = IQMetricsParameters::pixel_size*binNum;
			double focallengh = IQMetricsParameters::FocalLength;
			cv::Point2f center = checkerRe.center;
			double w = tan(hor / 2 * 180 / CV_PI) * focallengh / pixel*2;
			double h = tan(ver / 2 * 180 / CV_PI) * focallengh / pixel * 2;
			cv::Rect rect0(center.x - w / 2, center.y - h / 2, w, h);
			cv::rectangle(img_drawN, rect0, Scalar(0, 255, 0), 4 / binNum);
			cv::rectangle(img_drawP, rect0, Scalar(0, 255, 0), 4 / binNum);
			minCR = *min_element(crInMask.begin(), crInMask.end());
			maxCR = *max_element(crInMask.begin(), crInMask.end());
			double sum = accumulate(crInMask.begin(), crInMask.end(), 0.0);
			meanCR = sum / crInMask.size();
		}
		else if (type == NOMASK)
		{
			meanCR = cv::mean(crAll)(0);
			cv::minMaxLoc(crAll, &minCR, &maxCR);
		}
		
		re.minCR = minCR;
		re.maxCR = maxCR;
		re.meanCR = meanCR;
		re.crMat = crAll;
		re.greyLevelN = grayLevelN;
		re.greyLevelP = grayLevelP;
		//re.boardSize = checkerRe.boardSize;
		re.imgdrawN = img_drawN;
		re.imgdrawP = img_drawP;
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "checker CR calculation successfully");
		return re;
	}
	else
	{
		re.flag = false;
		re.errMsg = info + "The Input img is NULL!!!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
}


ContrastRatioReGuSu MLIQMetrics::MLContrastRatio::getContrastRatioGuSu(cv::Mat imgP, cv::Mat imgN)
{
	string info = "------getContrastRatioGuSu------";
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "getContrastRatioGuSu calculate start");
	ContrastRatioReGuSu re;
	ContrastRatioRe crRe = getContrastRatio(imgP, imgN,false,CIRCLE);
	if (crRe.flag)
	{
		re.crMat = crRe.crMat;
		re.greyLevelN = crRe.greyLevelN;
		re.greyLevelP = crRe.greyLevelP;
		re.imgdrawN = crRe.imgdrawN;
		re.imgdrawP = crRe.imgdrawP;
		re.minZoneA = crRe.minCR;
		re.maxZoneA = crRe.maxCR;
		re.meanZoneA = crRe.meanCR;
		double meanZoneB = accumulate(crRe.crOutMask.begin(), crRe.crOutMask.end(), 0.0) / crRe.crOutMask.size();
		re.meanZoneB = meanZoneB;
	}
	else
	{
		re.flag = false;
		re.errMsg = crRe.errMsg;
		return re;
	}
	return re;
}

SingleCheckerCRRe MLIQMetrics::MLContrastRatio::getSingleCheckerCR(cv::Mat img)
{

	SingleCheckerCRRe re;
	if (img.data != NULL)
	{
		//TODO::
		int binNum = IQMetricUtl::instance()->getBinNum(img.size());// m_binNum;
		m_binNum = binNum;
		cv::Rect ROIRect = IQMetricsParameters::ROIRect;
		img = GetROIMat(img, ROIRect);
		cv::Mat img8 = convertToUint8(img);
		cv::Mat img_draw = convertTo3Channels(img8);

		// TODO: get the rotaion angle
		cv::Rect rect;
		//cv::RotatedRect rectR = getCherkerBorder(imgP8, rect);
		//updateRotateImg(imgP, rectR.angle);
		//updateRotateImg(imgN, rectR.angle);
		//updateRotateImg(img_drawN, rectR.angle);
		//updateRotateImg(img_drawP, rectR.angle);

		MLCherkerboardDetect cd;
		double crRation = IQMetricsParameters::crArea;

		CheckerboardRe checkerRe = cd.detectChessboardTemplate1(img_draw, crRation, binNum);
		//checkerRe = cd.detectChessboardCorner1(imgP8, crRation, 4);
	   // checkerRe = cd.detectChessboardContour(imgP8, crRation, 4);
		if (checkerRe.flag)
		{
			vector<double> posValue, negValue;
			posValue = CalculateROIValue(img, checkerRe.rectVec);
			double cr = CalculateSingleCR(posValue);
			cv::Size boardSize = checkerRe.boardSize - cv::Size(1, 1);
			cv::Mat graylevelmat = updateMatValue(posValue, boardSize);
			updateImgdraw(checkerRe.rectVec, posValue, img_draw);
			putTextOnImage(img_draw, "CR:" + numToString(cr), cv::Point2f(200, 400), 24 / binNum);
			re.grayMat = graylevelmat;
			//re.ROIcr = crVecROI;
			re.cr = cr;
			re.boardSize = checkerRe.boardSize;
			re.imgdraw = img_draw;
		}
		return re;
	}
	else
	{
		re.flag = false;
		re.errMsg = "The Input img is NULL!!!";
		return re;

	}
}

#include "pch.h"
#include "ML_Distortion.h"
#include "ml_gridDetect.h"
#include"MLCherkerboardDetect.h"
#include "pch.h"
#include <numeric>
#include"MLContrastRatio.h"
#include"LogPlus.h"
#include"CrossCenter.h"
using namespace cv;
using namespace MLIQMetrics;
using namespace MLImageDetection;
MLIQMetrics::MLDistortion::MLDistortion()
{
}

MLIQMetrics::MLDistortion::~MLDistortion()
{
}
void MLIQMetrics::MLDistortion::setIsUpdateSLB(bool flag)
{
	m_IsUpdateSLB = flag;
}
void MLIQMetrics::MLDistortion::setIsSLB(bool flag)
{
	m_isSLB = flag;
}
void MLIQMetrics::MLDistortion::setFOVType(FOVTYPE type)
{
	m_fovType = type;
}
void MLIQMetrics::MLDistortion::setColor(string color)
{
	m_color = color;
}
DistortionRe MLIQMetrics::MLDistortion::GridDistortion(cv::Mat img, cv::Rect rect)
{

	DistortionRe re;
	if (img.data != NULL)
	{
		int binNum = IQMetricUtl::instance()->getBinNum(img.size());
		cv::Rect ROIRect = IQMetricsParameters::ROIRect;
		updateRectByRatio1(ROIRect, 1.0 / binNum);
		cv::Mat roi = GetROIMat(img, ROIRect);
		cv::Mat roi1 = convertToUint8(roi);
		cv::Mat img_draw = convertTo3Channels(roi1);
		cv::RotatedRect rectR;
		MLGridDetect grid;
		roi1 = grid.rotateGridImg(roi1);
		grid.SetbinNum(binNum);
		GridRe gridRe = grid.getGridContour(roi1);
		if (gridRe.flag == false)
		{
			gridRe = grid.getGridTemplateGaussian(roi1);
		}
		if (gridRe.flag == false)
		{
			gridRe = grid.getGridCorners(roi1);
		}

		if (gridRe.flag == true)
		{
			cv::Point2f center;
			vector<cv::Point2f> corners;
			std::vector<double> disCorner;
			vector<cv::Point2f>corns = gridRe.corners;
			cv::Mat ordList = gridRe.indexMap;
			center = getCenter(corns, ordList);
			circle(img_draw, center, 10, cv::Scalar(255, 0, 255), -1);
			cv::Point2f BL = corns[ordList.at<short>(ordList.rows - 1, 0)];
			corners.push_back(BL);
			cv::Point2f BR = corns[ordList.at<short>(ordList.rows - 1, ordList.cols - 1)];
			corners.push_back(BR);
			cv::Point2f UL = corns[ordList.at<short>(0, 0)];
			corners.push_back(UL);
			cv::Point2f UR = corns[ordList.at<short>(0, ordList.cols - 1)];
			corners.push_back(UR);
			double maxdis = 0;
			for (int i = 0; i < corners.size(); i++)
			{
				circle(img_draw, corners[i], 16 / binNum, cv::Scalar(0, 255, 0), -1);
				double val = getCornerDistortionVal(corners[i], center, binNum);
				disCorner.push_back(abs(val));
				putTextOnImage(img_draw, to_string(val) + "%", corners[i], 20 / binNum);
				if (abs(val) > abs(maxdis))
					maxdis = val;

			}
			double disAvg = -1;
			disAvg = accumulate(disCorner.begin(), disCorner.end(), 0.0) / disCorner.size();
			disCorner.push_back(disAvg);
			cv::Mat xPos(ordList.rows, ordList.cols, CV_32FC1);
			cv::Mat yPos(ordList.rows, ordList.cols, CV_32FC1);
			int m = 0;
			cv::Mat xPosDis(ordList.rows, ordList.cols, CV_32FC1);
			cv::Mat yPosDis(ordList.rows, ordList.cols, CV_32FC1);

			for (int i = 0; i < ordList.rows; i++)
			{
				for (int j = 0; j < ordList.cols; j++)
				{
					// TODO:
					int in = ordList.at<short>(i, j);
					double x = corns[in].x;
					double y = corns[in].y;
					double deltx = abs(x - center.x);
					double delty = abs(y - center.y);
					xPosDis.at<float>(i, j) = deltx;
					yPosDis.at<float>(i, j) = delty;
					double m_FocalLength = IQMetricsParameters::FocalLength;
					double m_pixel_size = IQMetricsParameters::pixel_size * binNum;
					xPos.at<float>(i, j) = atan(deltx * m_pixel_size / m_FocalLength) * 180 / CV_PI;
					yPos.at<float>(i, j) = atan(delty * m_pixel_size / m_FocalLength) * 180 / CV_PI;
					cv::Point2f pt = cv::Point2f(x, y);
					circle(img_draw, pt, 5, cv::Scalar(255, 0, 255), -1);
				}
			}
			//double maxdis = *max_element(disCorner.begin(), disCorner.end());
			re.disCorner.clear();
			re.disCorner = disCorner;
			re.disAvg = disAvg;
			re.disMax = maxdis;
			re.xPos = gridRe.xLocMat + ROIRect.x;
			re.yPos = gridRe.yLocMat + ROIRect.y;
			re.imgdraw = img_draw;
		}
	}
	return re;


}
DistortionRe MLIQMetrics::MLDistortion::GridDistortionFourCorner(const cv::Mat img, cv::Rect rect)
{
	DistortionRe re;
	if (img.data != NULL)
	{
		cv::Mat disMat(cv::Size(1, 4), CV_32FC1, Scalar(0));
		int binNum = IQMetricUtl::instance()->getBinNum(img.size());
		m_binNum = binNum;
		double m_FocalLength = IQMetricsParameters::FocalLength;
		double m_pixel_size = IQMetricsParameters::pixel_size;
		double pix2deg = IQMetricUtl::instance()->getPix2Degree(img.size());
		cv::Rect ROIRect = IQMetricsParameters::ROIRect;
		updateRectByRatio1(ROIRect, 1.0 / binNum);
		if (m_fovType == BIGFOV)
			ROIRect = cv::Rect(0, 0, -1, -1);
		cv::Mat roi = GetROIMat(img, ROIRect);
		cv::Mat roi1 = convertToUint8(roi);
		// cv::Mat roi1 = convert12bitTo8bit(roi);
		cv::Rect rectAfterRotation;

		cv::RotatedRect rectR;
		cv::Rect rectGrid = getGridRect(roi1, rectR, binNum);
		cv::Point2f center0((float)(roi1.cols / 2.0), (float)(roi1.rows / 2.0));
		updateRotateImg(roi1, rectR.angle);
		rectAfterRotation = updateRotateRect(rectR, center0);

		cv::Mat img_draw = convertTo3Channels(roi1);
		//drawRectOnImage(img_draw, rectAfterRotation,8/binNum);
		int len = m_len / binNum;
		int width = rectAfterRotation.width;
		int height = rectAfterRotation.height;
		int startX = rectAfterRotation.x;
		int startY = rectAfterRotation.y;
		cv::Rect rectTL(startX - len / 2, startY - len / 2, len, len);
		cv::Rect rectTR(startX + width - len / 2, startY - len / 2, len, len);
		cv::Rect rectBL(startX - len / 2, startY + height - len / 2, len, len);
		cv::Rect rectBR(startX + width - len / 2, startY + height - len / 2, len, len);
		cv::Rect rectAll(0, 0, roi.cols, roi.rows);
		rectTL = rectTL & rectAll;
		rectTR = rectTR & rectAll;
		rectBR = rectBR & rectAll;
		rectBL = rectBL & rectAll;

		drawRectOnImage(img_draw, rectTL, 8 / binNum);
		drawRectOnImage(img_draw, rectTR, 8 / binNum);
		drawRectOnImage(img_draw, rectBL, 8 / binNum);
		drawRectOnImage(img_draw, rectBR, 8 / binNum);
		cv::Point2f ptBL = getCornerByHist(roi1(rectBL).clone(), binNum)/*+cv::Point2f(3,-3)*/;
		cv::Point2f ptBR = getCornerByHist(roi1(rectBR).clone(), binNum) /*- cv::Point2f(3, 3)*/;
		cv::Point2f ptTL = getCornerByHist(roi1(rectTL).clone(), binNum) /*+ cv::Point2f(3, 3)*/;
		cv::Point2f ptTR = getCornerByHist(roi1(rectTR).clone(), binNum) /*- cv::Point2f(3, -3)*/;

		cv::Point2f center = Point2f(rectAfterRotation.tl()) + Point2f(width / 2.0, height / 2.0);
		vector<cv::Point2f> corners;
		std::vector<double> disCorner;
		corners.push_back(ptBL + Point2f(rectBL.tl()));
		corners.push_back(ptBR + Point2f(rectBR.tl()));
		corners.push_back(ptTL + Point2f(rectTL.tl()));
		corners.push_back(ptTR + Point2f(rectTR.tl()));
		center = getPointsCenter(corners);
		if (accurateFlag)
		{
			center = getAccucateCenter(center, roi1);
		}
		circle(img_draw, center, 16 / binNum, cv::Scalar(255, 0, 255), -1);
		double maxdis = 0;
		for (int i = 0; i < corners.size(); i++)
		{
			circle(img_draw, corners[i], 16 / binNum, cv::Scalar(255, 0, 255), -1);

			double val = getCornerDistortionVal(corners[i], center, binNum);
			// double val = getCornerDistortionVal(corners[i], center, pix2deg);
			putTextOnImage(img_draw, to_string(val) + "%", corners[i], 24 / binNum);
			disMat.at<float>(i, 0) = val;
			disCorner.push_back(abs(val));
			if (abs(val) > abs(maxdis))
				maxdis = val;
		}
		double disAvg = -1;
		updateDistortionBySLB(disCorner, m_isSLB);
		disAvg = accumulate(disCorner.begin(), disCorner.end(), 0.0) / disCorner.size();
		maxdis = 0;
		for (int i = 0; i < disCorner.size(); i++)
		{
			if (abs(disCorner[i]) > maxdis)
				maxdis = disCorner[i];
		}
		re.disCorner.clear();
		re.disCorner = disCorner;
		re.disAvg = disAvg;
		re.disMax = maxdis;
		re.imgdraw = img_draw;
	}
	return re;
}
DistortionRe MLIQMetrics::MLDistortion::GridDistortionFourCornerBigFOV(const cv::Mat img)
{
		DistortionRe re;
	if (img.data != NULL)
	{
		cv::Mat disMat(cv::Size(1, 4), CV_32FC1, Scalar(0));
		int binNum = IQMetricUtl::instance()->getBinNum(img.size());
		m_binNum = binNum;
		double m_FocalLength = IQMetricsParameters::FocalLength;
		double m_pixel_size = IQMetricsParameters::pixel_size;
		double pix2deg = IQMetricUtl::instance()->getPix2Degree(img.size());
		cv::Mat roi1 = convertToUint8(img);
		cv::Mat img_draw = convertTo3Channels(roi1);
		//drawRectOnImage(img_draw, rectAfterRotation,8/binNum);
		//int len = m_len / binNum;
		int w = 500 / binNum;
		int h = 300 / binNum;
		cv::Rect rectTL(0, 0, w ,h);
		cv::Rect rectTR(roi1.cols-w, 0, w, h);
		cv::Rect rectBL(0,roi1.rows-h,w, h);
		cv::Rect rectBR(roi1.cols - w, roi1.rows - h, w, h);
		cv::Rect rectAll(0, 0, roi1.cols, roi1.rows);
		drawRectOnImage(img_draw, rectTL, 8 / binNum);
		drawRectOnImage(img_draw, rectTR, 8 / binNum);
		drawRectOnImage(img_draw, rectBL, 8 / binNum);
		drawRectOnImage(img_draw, rectBR, 8 / binNum);
		cv::Point2f ptBL = getCornerByHist(roi1(rectBL).clone(), binNum)/*+cv::Point2f(3,-3)*/;
		cv::Point2f ptBR = getCornerByHist(roi1(rectBR).clone(), binNum) /*- cv::Point2f(3, 3)*/;
		cv::Point2f ptTL = getCornerByHist(roi1(rectTL).clone(), binNum) /*+ cv::Point2f(3, 3)*/;
		cv::Point2f ptTR = getCornerByHist(roi1(rectTR).clone(), binNum) /*- cv::Point2f(3, -3)*/;

		vector<cv::Point2f> corners;
		std::vector<double> disCorner;
		corners.push_back(ptBL + Point2f(rectBL.tl()));
		corners.push_back(ptBR + Point2f(rectBR.tl()));
		corners.push_back(ptTL + Point2f(rectTL.tl()));
		corners.push_back(ptTR + Point2f(rectTR.tl()));
		cv::Point2f center = getPointsCenter(corners);
		if (accurateFlag)
		{
			center = getAccucateCenter(center, roi1);
		}
		circle(img_draw, center, 16 / binNum, cv::Scalar(255, 0, 255), -1);
		double maxdis = 0;
		for (int i = 0; i < corners.size(); i++)
		{
			circle(img_draw, corners[i], 16 / binNum, cv::Scalar(255, 0, 255), -1);

			double val = getCornerDistortionVal(corners[i], center, binNum);
			// double val = getCornerDistortionVal(corners[i], center, pix2deg);
			if(i==0)
			putTextOnImage(img_draw, to_string(val) + "%", corners[i], 24 / binNum);
			else if(i==1)
				putTextOnImage(img_draw, to_string(val) + "%", corners[i]-cv::Point2f(1800/binNum,0), 24 / binNum);
			else if (i == 2)
				putTextOnImage(img_draw, to_string(val) + "%", corners[i] - cv::Point2f(0 / binNum, -300/binNum), 24 / binNum);
			else if (i == 3)
				putTextOnImage(img_draw, to_string(val) + "%", corners[i] - cv::Point2f(1800 / binNum, -300/binNum), 24 / binNum);
			disMat.at<float>(i, 0) = val;
			disCorner.push_back(abs(val));
			if (abs(val) > abs(maxdis))
				maxdis = val;
		}
		double disAvg = -1;
		updateDistortionBySLB(disCorner, m_isSLB);
		disAvg = accumulate(disCorner.begin(), disCorner.end(), 0.0) / disCorner.size();
		maxdis = 0;
		for (int i = 0; i < disCorner.size(); i++)
		{
			if (abs(disCorner[i]) > maxdis)
				maxdis = disCorner[i];
		}
		re.disCorner.clear();
		re.disCorner = disCorner;
		re.disAvg = disAvg;
		re.disMax = maxdis;
		re.imgdraw = img_draw;
	}
	return re;
}
DistortionRe MLIQMetrics::MLDistortion::CheckerDistortion(cv::Mat img)
{
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "CheckerDistortion calculation start");
	string info = "------CheckerDistortion------";
	DistortionRe re;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "input is null";
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

	MLContrastRatio cr;
	cv::Rect rect;
	cv::Mat img8 = convertToUint8(img);
	cv::RotatedRect rectR = cr.getCherkerBorder(img8, rect);
	updateRotateImg(img8, rectR.angle);
	cv::Point2f cen(img8.cols / 2, img8.rows / 2);
	cv::Rect rectRo = updateRotateRect(rectR, cen);
	// img = GetROIMat(img, rect);

	MLCherkerboardDetect cb;
	// patameters to changed
	cb.SetChecssboardPointsClusters(300 / binNum);
	cb.SetChessboardxyClassification(350 / binNum);
	cb.SetChessboardUpdateFlag(false);
	CheckerboardRe checkerRe = cb.detectChessboardTemplate1(img8, 0.5, binNum);
	if (checkerRe.flag == false)
	{
		re.flag = false;
		re.errMsg = checkerRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Checker detection sucessfully");

	cv::Point2f ptA, ptB, ptC, ptD, center;
	int row = checkerRe.xLocMat.rows;
	int col = checkerRe.xLocMat.cols;
	ptA.x = checkerRe.xLocMat.at<float>(1, 1);
	ptA.y = checkerRe.yLocMat.at<float>(1, 1);
	ptC.x = checkerRe.xLocMat.at<float>(row - 2, 1);
	ptC.y = checkerRe.yLocMat.at<float>(row - 2, 1);
	ptB.x = checkerRe.xLocMat.at<float>(1, col - 2);
	ptB.y = checkerRe.yLocMat.at<float>(1, col - 2);
	ptD.x = checkerRe.xLocMat.at<float>(row - 2, col - 2);
	ptD.y = checkerRe.yLocMat.at<float>(row - 2, col - 2);
	cv::Point2f c1, c2, c3, c4;
	c1.x = checkerRe.xLocMat.at<float>(row / 2, col / 2 - 1);
	c1.y = checkerRe.yLocMat.at<float>(row / 2, col / 2 - 1);
	c2.x = checkerRe.xLocMat.at<float>(row / 2, col / 2);
	c2.y = checkerRe.yLocMat.at<float>(row / 2, col / 2);
	c3.x = checkerRe.xLocMat.at<float>(row / 2 - 1, col / 2 - 1);
	c3.y = checkerRe.yLocMat.at<float>(row / 2 - 1, col / 2 - 1);
	c4.x = checkerRe.xLocMat.at<float>(row / 2 - 1, col / 2);
	c4.y = checkerRe.yLocMat.at<float>(row / 2 - 1, col / 2);

	//center = (c1 + c2 + c3 + c4) / 4.0;
	center = c4;
	cv::Mat imgdraw = checkerRe.img_draw;

	putTextOnImage(imgdraw, "A", ptA, 24 / binNum);
	putTextOnImage(imgdraw, "B", ptB, 24 / binNum);
	putTextOnImage(imgdraw, "C", ptC, 24 / binNum);
	putTextOnImage(imgdraw, "D", ptD, 24 / binNum);
	putTextOnImage(imgdraw, "O", center, 24 / binNum);
	circle(imgdraw, center, 10, Scalar(255, 0, 255), -1);
	//circle(imgdraw, c1, 24 / binNum, Scalar(255, 0, 255), -1);
	//circle(imgdraw, c2, 24 / binNum, Scalar(255, 0, 255), -1);
	//circle(imgdraw, c3, 24 / binNum, Scalar(255, 0, 255), -1);
	//circle(imgdraw, c4, 24 / binNum, Scalar(255, 0, 255), -1);
	circle(imgdraw, ptA, 24 / binNum, Scalar(255, 0, 255), -1);
	circle(imgdraw, ptB, 24 / binNum, Scalar(255, 0, 255), -1);
	circle(imgdraw, ptC, 24 / binNum, Scalar(255, 0, 255), -1);
	circle(imgdraw, ptD, 24 / binNum, Scalar(255, 0, 255), -1);

	double pixel = IQMetricsParameters::pixel_size * binNum;
	double focallnegh = IQMetricsParameters::FocalLength;
	double disA = Getdistance(ptA, center);
	double disB = Getdistance(ptB, center);
	double disC = Getdistance(ptC, center);
	double disD = Getdistance(ptD, center);
	disA = atan(disA * pixel * binNum / focallnegh) * 180 / CV_PI;
	disB = atan(disB * pixel * binNum / focallnegh) * 180 / CV_PI;
	disC = atan(disC * pixel * binNum / focallnegh) * 180 / CV_PI;
	disD = atan(disD * pixel * binNum / focallnegh) * 180 / CV_PI;

	double disAvg = (disA + disB + disC + disD) / 4.0;
	double Adist = 100 * (disAvg - disA) / disA;
	double Bdist = 100 * (disAvg - disB) / disB;
	double Cdist = 100 * (disAvg - disC) / disC;
	double Ddist = 100 * (disAvg - disD) / disD;

	putTextOnImage(imgdraw, to_string(Adist) + "%", ptA + Point2f(0, 100), 24 / binNum);
	putTextOnImage(imgdraw, to_string(Bdist) + "%", ptB + Point2f(0, 100), 24 / binNum);
	putTextOnImage(imgdraw, to_string(Cdist) + "%", ptC + Point2f(0, 100), 24 / binNum);
	putTextOnImage(imgdraw, to_string(Ddist) + "%", ptD + Point2f(0, 100), 24 / binNum);
	updateImgdraw(ptA, imgdraw, binNum);
	updateImgdraw(ptB, imgdraw, binNum);
	updateImgdraw(ptC, imgdraw, binNum);
	updateImgdraw(ptD, imgdraw, binNum);
	updateImgdraw(center, imgdraw, binNum);
	vector<double>disVec = { Adist, Bdist, Cdist, Ddist };
	double maxDist = *max_element(disVec.begin(), disVec.end());
	//re.Adist = Adist;
	//re.Bdist = Bdist;
	//re.Cdist = Cdist;
	//re.Ddist = Ddist;
	//re.maxDist = maxDist;
	re.xPos = checkerRe.xLocMat;
	re.yPos = checkerRe.yLocMat;
	re.imgdraw = imgdraw;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "CheckerDistortion calculation successfully");
	return re;
}
void MLIQMetrics::MLDistortion::setRotationAngle(double angle)
{
	m_rotationAngle = angle;
}

void MLIQMetrics::MLDistortion::updateImgdraw(cv::Point2f cen, cv::Mat& imgdraw, int binNum)
{
	string str = numToString(cen.x) + "," + numToString(cen.y);
	putTextOnImage(imgdraw, str, cen + Point2f(-800 / binNum, 300), 24 / binNum);
}

cv::Rect MLIQMetrics::MLDistortion::getGridRect(cv::Mat roi, cv::RotatedRect& rectR, int binNum)
{
	cv::Rect rect;
	if (roi.data != NULL)
	{
		cv::Mat img_draw = convertTo3Channels(roi);
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::Mat img_process, srcbinary;
		cv::GaussianBlur(roi, roi, cv::Size(3, 3), 0, 0);
		cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));
		//  cv::morphologyEx(roi, roi, cv::MORPH_CLOSE, element1);
		  // cv::morphologyEx(img_gray1, img_process, cv::MORPH_GRADIENT, element1);
		threshold(roi, srcbinary, 0, 255, THRESH_TRIANGLE);
		// cv::Canny(srcbinary, img_canny, 50, 150);
		cv::findContours(srcbinary, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		vector<double> area_sort;
		vector<int> list;
		cv::Mat roi;
		RotatedRect rec;
		for (int i = 0; i < contours.size(); ++i)
		{
			double area = contourArea(contours[i], false);
			// cout << area << endl;
			double powbin = 2; // pow(m_para.binNum, 2);
			if (area > 1e6 / pow(binNum, 2))
			{
				rectR = cv::minAreaRect(contours[i]);
				rect = cv::boundingRect(cv::Mat(contours[i]));
				cv::rectangle(img_draw, rect, Scalar(255, 0, 255), 1);
				break;
			}
		}
	}
	return rect;
}

cv::Point2f MLIQMetrics::MLDistortion::getCornerByHist(cv::Mat img, int binNum)
{

	cv::Mat imgdraw = convertTo3Channels(img);
	cv::Mat imgth;
	cv::threshold(img, imgth, 0, 255, THRESH_OTSU);
	cv::Mat rowMat, colMat;
	cv::reduce(imgth, rowMat, 0, REDUCE_MAX);
	cv::reduce(imgth, colMat, 1, REDUCE_MAX);
	cv::Mat kernel1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 1));
	cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 3));
	cv::morphologyEx(rowMat, rowMat, MORPH_GRADIENT, kernel1);
	cv::morphologyEx(colMat, colMat, MORPH_GRADIENT, kernel2);
	cv::Scalar m0, m1, std0, std1;
	double max1, max2;
	cv::Point maxLoc1, maxLoc2;
	cv::minMaxLoc(rowMat, NULL, &max1, NULL, &maxLoc1);
	cv::minMaxLoc(colMat, NULL, &max2, NULL, &maxLoc2);
	cv::Point2f c0;
	c0.x = maxLoc1.x;
	c0.y = maxLoc2.y;
	drawPointOnImage(imgdraw, c0, 1);

	// get the exact loc
	int len = 80 / binNum;
	cv::Rect rect0(c0.x - len / 2, c0.y - len / 2, len, len);
	cv::Rect rectAnd = rect0 & cv::Rect(0, 0, img.rows, img.cols);
	cv::Mat roi = img(rectAnd);
	cv::reduce(roi, rowMat, 0, REDUCE_AVG);
	cv::reduce(roi, colMat, 1, REDUCE_AVG);
	cv::minMaxLoc(rowMat, NULL, &max1, NULL, &maxLoc1);
	cv::minMaxLoc(colMat, NULL, &max2, NULL, &maxLoc2);

	cv::Point2f center;
	center.x = maxLoc1.x + rectAnd.x;
	center.y = maxLoc2.y + rectAnd.y;
	drawPointOnImage(imgdraw, center, 1);

	return center;


}

cv::Point2f MLIQMetrics::MLDistortion::getCorner(cv::Mat img)
{
	cv::Mat imgdraw = convertTo3Channels(img);
	cv::Mat imgth;
	cv::threshold(img, imgth, 0, 255, THRESH_OTSU);
	vector<Point2f> corners;
	int maxcorners = 1;
	double qualityLevel = 0.01;
	double minDistance = 100;
	int blockSize = 5;
	double k = 0.06;
	goodFeaturesToTrack(imgth, corners, maxcorners, qualityLevel, minDistance, Mat(), blockSize, false, k);
	Size winSize = Size(3, 3);
	Size zeroZone = Size(-1, -1);
	TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 40, 0.001);
	cv::cornerSubPix(imgth, corners, winSize, zeroZone, criteria);
	drawPointsOnImage(imgdraw, corners, 1, Scalar(0, 0, 255));
	if (corners.size() > 0)
		return corners[0];

}

double MLIQMetrics::MLDistortion::getCornerDistortionVal(cv::Point2f corner, cv::Point2f center, int binNum)
{

	double val = -1;
	double fovH = 0, fovV = 0;
	double m_FocalLength = IQMetricsParameters::FocalLength;
	double m_pixel_size = IQMetricsParameters::pixel_size * binNum;
	// double pix2deg = IQMetricUtl::instance()->getPix2Degree();
	if (m_fovType == SMALLFOV)
	{
		fovH = IQMetricsParameters::distortonTheHor;
		fovV = IQMetricsParameters::distortonTheVer;
	}
	else if (m_fovType == BIGFOV)
	{
		fovH = IQMetricsParameters::distortonTheHorBig;
		fovV = IQMetricsParameters::distortonTheVerBig;
	}
	double degx = tan(fovH / 2.0 / 180 * CV_PI) * m_FocalLength / m_pixel_size;
	double degy = tan(fovV / 2.0 / 180 * CV_PI) * m_FocalLength / m_pixel_size;
	double disThe = sqrt(degx * degx + degy * degy);
	double d = Getdistance(corner, center);
	double medVar = m_FocalLength * tan(sqrt(pow((fovH / 2), 2) + pow((fovV / 2), 2)) / 180.0 * CV_PI);
	//d = d * m_pixel_size;
	// cout << d << "," << medVar << endl;
	val = 100 * /*abs*/ ((d - disThe) / disThe);
	return val;


}

double MLIQMetrics::MLDistortion::getCornerDistortionVal(cv::Point2f corner, cv::Point2f center, double pix2deg)
{

	double val = -1;
	double fovH, fovV;
	double m_FocalLength = IQMetricsParameters::FocalLength;
	// double pix2deg = IQMetricUtl::instance()->getPix2Degree();

	fovH = IQMetricsParameters::distortonTheHor;
	fovV = IQMetricsParameters::distortonTheVer;

	double d = Getdistance(corner, center);
	// double medVar = m_FocalLength * tan(sqrt(pow((fovH / 2), 2) + pow((fovV / 2), 2)) / 180.0 * CV_PI);
	double medVar = sqrt(pow((fovH / 2), 2) + pow((fovV / 2), 2));

	d = d * pix2deg;
	// cout << d << "," << medVar << endl;
	val = 100 * /*abs*/ ((d - medVar) / medVar);
	return val;


}

cv::Point2f MLIQMetrics::MLDistortion::getAccucateCenter(cv::Point2f cen, cv::Mat img)
{
	cv::Rect rect;
	int len = m_len / m_binNum;
	rect.x = cen.x - len / 2;
	rect.y = cen.y - len / 2;
	rect.width = len;
	rect.height = len;
	cv::Mat roi = img(rect).clone();
	CrossCenter cc;
	cv::Point2f c0 = cc.find_centerLINES(roi);
	if (c0.x > 0 && c0.y > 0)
		return c0 + Point2f(rect.tl());
	else
		return cen;
	return cv::Point2f();
}

cv::Point2f MLIQMetrics::MLDistortion::getCenter(vector<cv::Point2f> corns, cv::Mat indexMap)
{
	cv::Point2f center;
	int col = indexMap.cols;
	int row = indexMap.rows;
	if ((row / 2) % 2 == 1 && (col / 2) % 2 == 1)
	{
		cv::Point2f cor1 = corns[indexMap.at<short>(row / 2 - 1, col / 2 - 1)];
		cv::Point2f cor2 = corns[indexMap.at<short>(row / 2 - 1, col / 2)];
		cv::Point2f cor3 = corns[indexMap.at<short>(row / 2, col / 2 - 1)];
		cv::Point2f cor4 = corns[indexMap.at<short>(row / 2, col / 2)];
		center = (cor1 + cor2 + cor3 + cor4) / 4.0;
	}
	else if ((row / 2) % 2 == 0 && (col / 2) % 2 == 0)
	{
		center = corns[indexMap.at<short>((row) / 2, (col) / 2)];
	}
	else if ((row / 2) % 2 == 0 && (col / 2) % 2 == 1)
	{
		cv::Point2f cor1 = corns[indexMap.at<short>(row / 2, (col - 1) / 2)];
		cv::Point2f cor2 = corns[indexMap.at<short>(row / 2, (col - 1) / 2 + 1)];
		center = (cor1 + cor2) / 2.0;
	}
	else if ((row / 2) % 2 == 1 && (col / 2) % 2 == 0)
	{
		cv::Point2f cor1 = corns[indexMap.at<short>((row - 1) / 2, col / 2)];
		cv::Point2f cor2 = corns[indexMap.at<short>((row - 1) / 2 + 1, col / 2)];
		center = (cor1 + cor2) / 2.0;
	}
	return center;
}

void MLIQMetrics::MLDistortion::updateDistortionBySLB(vector<double>& disvec, bool isSLB)
{
	string fovstr = IQMetricUtl::instance()->fovTypeToString(m_fovType);
	string filepath = "./config/ALGConfig/" + fovstr + "_" + m_color + "_distortion.csv";
	cv::Mat disMat = cv::Mat(disvec.size(), 1, CV_64F, disvec.data()).clone();
	disMat.convertTo(disMat, CV_32FC1);
	if (isSLB)
	{
		if(m_IsUpdateSLB)
		writeMatTOCSV(filepath, disMat);
	}
	else
	{
		cv::Mat dismatSLB = readCSVToMat(filepath);
		cv::Mat dismatCali = disMat - dismatSLB;
		dismatCali.convertTo(dismatCali, CV_64FC1);
		std::vector<double> vec((double*)dismatCali.data, (double*)dismatCali.data + dismatCali.total());
		disvec.clear();
		disvec = vec;
	}
}


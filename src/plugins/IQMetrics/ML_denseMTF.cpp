#include "pch.h"
#include "ML_denseMTF.h"
#include"MLCherkerboardDetect.h"
#include"mtfpipelineplugin.h"
#include"pipeline.h"
#include"MLContrastRatio.h"
#include"LogPlus.h"
#include"ml_gridDetect.h"
#include <omp.h>
#include <shared_mutex>
#include <thread>
using namespace MLImageDetection;
using namespace cv;
using namespace MLIQMetrics;
using namespace std;

MLIQMetrics::MLdenseMTF::MLdenseMTF()
{
}

MLIQMetrics::MLdenseMTF::~MLdenseMTF()
{
}
void MLIQMetrics::MLdenseMTF::setIsSLB(bool flag)
{
	m_IsSLB = flag;
}
void MLIQMetrics::MLdenseMTF::setIsDisparityEyebox(bool flag)
{
	m_isDisparityEyebox = flag;
}
void MLIQMetrics::MLdenseMTF::setPatternCenter(cv::Point2f cen)
{
	m_center = cen;
}
DenseMTFRe MLIQMetrics::MLdenseMTF::getDenseMTFChecker(cv::Mat img)  
{
	string info = "------getDenseMTF------";
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "DenseMTF calculation start");
	DenseMTFRe re;
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
		return re;
	}
	m_binNum = binNum;
	cv::Mat img8 = convertToUint8(img);  
	cv::Mat imgdraw = convertTo3Channels(img8);
	MLContrastRatio cr;
	cv::Rect rect;
	cv::RotatedRect rectR = cr.getCherkerBorder(img8, rect);
	updateRotateImg(img, rectR.angle);
	updateRotateImg(img8, rectR.angle);
	updateRotateImg(imgdraw, rectR.angle);

	//cv::Mat imgResize;
	int reiseNum =1;
	//cv::resize(img8, imgResize, img.size() / reiseNum);
	MLCherkerboardDetect cb;
	cb.SetChecssboardPointsClusters(15);
	cb.SetChessboardxyClassification(20);
	cb.SetChessboardUpdateFlag(false);
	CheckerboardRe checkerRe;
	checkerRe = cb.detectChessboardCorner(img8, 0.25, binNum);
	if (checkerRe.flag == false)
	{
		re.flag = false;
		re.errMsg = info + checkerRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "DenseMTF calculation: corners detection successfully");

	m_center = checkerRe.center * reiseNum;
	m_center = IQMetricsParameters::opticalCenter / binNum;
	cv::Mat xlocMat = checkerRe.xLocMat * reiseNum;
	cv::Mat ylocMat = checkerRe.yLocMat * reiseNum;

	cv::Mat mtfmapH = getMtfMat(xlocMat, ylocMat, imgdraw, img, 0, SLANT);
	cv::Mat mtfmapV = getMtfMat(xlocMat, ylocMat, imgdraw, img, 1, SLANT);
	getDenseMTFData(mtfmapH, m_maskH, re, 0);
	getDenseMTFData(mtfmapV, m_maskV, re, 1);
	re.mtfMapH = mtfmapH;
	re.mtfMapV = mtfmapV;
	re.imgdraw = imgdraw;
	re.xPos = xlocMat;
	re.yPos = ylocMat;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "DenseMTF calculation successfully");
	return re;
}

DenseMTFGridRe MLIQMetrics::MLdenseMTF::getDenseMTFGrid(const cv::Mat imgRaw) // -----this---
{
	string info = "------getDenseMTFGrid------";
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "DenseMTF calculation start");
	DenseMTFGridRe re;
	if (imgRaw.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	cv::Mat img = IQMetricUtl::instance()->getRotationAndFlipImg(imgRaw, m_IsSLB); // 11240*9200
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());  // 缩放 
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		return re;
	}
	m_binNum = binNum;
	//cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	//updateRectByRatio1(ROIRect, 1.0 / binNum);
	//img = GetROIMat(img, ROIRect);

	int binNumCen = IQMetricUtl::instance()->getBinNum(cv::Size(m_center.x * 2, m_center.y * 2)); 
	double ratio = double(binNumCen) / double(binNum);
	cv::Point2f cen = m_center * ratio;
	int len = m_len / binNum;
	cv::Rect rect0(cen.x - len / 2, cen.y - len / 2, len, len);
	//img = GetROIMat(img, rect0);

	MLGridDetect grid;
	grid.setAccurateDetectionFlag(false);
	//img = grid.rotateGridImg(img);
	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);
	cv::Mat imgResize;
	int resizeNum = IQMetricsParameters::GridResizeNum / binNum;  // 4
	int reiseNum = resizeNum / binNum;
	cv::resize(img8, imgResize, img.size() / reiseNum);   // img/4
	grid.SetbinNum(reiseNum);
	grid.SetChessboardUpdateFlag(false);
	GridRe gridRe;
	if(m_IsSLB)
	gridRe = grid.getGridContour(imgResize);
	std::shared_mutex rw_mutex;
	std::mutex mtx;  
	if (m_IsSLB)
	{
		if (gridRe.xLocMat.size() == IQMetricsParameters::GridSize) // 检测到的角点横坐标矩阵和{15,19}
		{
			mtx.lock();
			grid.writeGridInfoToCSV(gridRe);
			mtx.unlock();
		}
		else
		{
			grid.readGridInfoFromCSV(gridRe);
		}
	}
	else
	{
		grid.readGridInfoFromCSV(gridRe);
		string filepath = "./config/ALGConfig/slbInfo/DUTCenter.csv";
		cv::Mat cenmat = readCSVToMat(filepath);
		cv::Point2f center;
		center.x = cenmat.at<float>(0, 0)/4.0;
		center.y = cenmat.at<float>(1, 0)/4.0;
		cv::Point2f offset = center - gridRe.center;
		//gridRe.xLocMat = gridRe.xLocMat -offset.x+15;
		//gridRe.yLocMat = gridRe.yLocMat - offset.y;

	}
	if (gridRe.flag == false)
	{
		re.flag = false;
		re.errMsg = info + gridRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "DenseMTF calculation: corners detection successfully");

	//m_center = IQMetricsParameters::opticalCenter / binNum;
	cv::Mat xlocMat = gridRe.xLocMat * reiseNum;
	cv::Mat ylocMat = gridRe.yLocMat * reiseNum;
	//xlocMat = xlocMat(cv::Range(1,xlocMat.rows-1),cv::Range(1,xlocMat.cols-1));
	//ylocMat = ylocMat(cv::Range(1, ylocMat.rows - 1), cv::Range(1, ylocMat.cols - 1));

	cv::Mat mtfmapH, mtfmapV;
	//getMtfMat(xlocMat, ylocMat, imgdraw, img, CROSS, mtfmapH0, mtfmapV0);
	mtfmapH = getMtfMat(xlocMat, ylocMat, imgdraw, img, 0, CROSS);
	cv::Mat mtfmapH0 = calculateMTFHor(mtfmapH); // 上下相邻行做平均
	cv::Mat mtfmapH2 = calculateMTFHor(m_mtfmapH2);
	mtfmapV = getMtfMat(xlocMat, ylocMat, imgdraw,img, 1, CROSS);
	cv::Mat mtfmapV0 = calculateMTFVer(mtfmapV);
	cv::Mat mtfmapV2= calculateMTFVer(m_mtfmapV2);

	calculateMtfValue(mtfmapH0, re.minH,re.meanH);
	calculateMtfValue(mtfmapV0, re.minV, re.meanV);
	calculateMtfValue(mtfmapH2, re.minH_freq2, re.meanH_freq2);
	calculateMtfValue(mtfmapV2, re.minH_freq2, re.meanH_freq2);

	//cv::Mat mtfH0,mtfV0;
	//mtfmapH0.copyTo(mtfH0, mtfmapH0 > 20);
	//mtfmapV0.copyTo(mtfV0, mtfmapV0 > 20);
	//Scalar meanH = cv::mean(mtfH0); // 6.25频率的meanMTFH
	//Scalar meanV = cv::mean(mtfV0);
	//double minH, minV;
	//cv::minMaxLoc(mtfH0, &minH,NULL,NULL,NULL);
	//cv::minMaxLoc(mtfV0, &minV, NULL, NULL, NULL);
	//re.meanH = meanH(0);
	//re.meanV = meanV(0);
	//re.minH = minH;
	//re.minV = minV;
	if (m_isDisparityEyebox)
	{
		cv::Mat xloc0 = xlocMat(cv::Range(1, xlocMat.rows - 1), cv::Range(1, xlocMat.cols - 1));
		cv::Mat yloc0 = ylocMat(cv::Range(1, xlocMat.rows - 1), cv::Range(1, xlocMat.cols - 1));
		cv::Point2f cen = gridRe.center;
		cv::Rect2f rect2fB = IQMetricsParameters::zoneBRect;
		cv::Rect rectB = IQMetricUtl::instance()->getZoneRect(rect2fB, cen, binNum);
		rectangle(imgdraw, rectB, Scalar(255, 0, 255), 10);
		vector < double >mtfOutMask;
		vector < double>mtfHInMask0 = getMTFRect(mtfmapH0,xloc0, yloc0, rectB,mtfOutMask);
		vector < double>mtfVInMask0 = getMTFRect(mtfmapV0, xloc0, yloc0, rectB, mtfOutMask);
		vector < double>mtfHInMask2 = getMTFRect(mtfmapH2, xloc0, yloc0, rectB, mtfOutMask);
		vector < double>mtfVInMask2 = getMTFRect(mtfmapV2, xloc0, yloc0, rectB, mtfOutMask);
		re.minH = *min_element(mtfHInMask0.begin(), mtfHInMask0.end());
		re.minH_freq2 = *min_element(mtfHInMask2.begin(), mtfHInMask2.end());
		re.minV = *min_element(mtfVInMask0.begin(), mtfVInMask0.end());
		re.minV_freq2 = *min_element(mtfVInMask2.begin(), mtfVInMask2.end());
		re.meanH = accumulate(mtfHInMask0.begin(), mtfHInMask0.end(),0.0)/ mtfHInMask0.size();
		re.meanH_freq2 = accumulate(mtfHInMask2.begin(), mtfHInMask2.end(),0.0) / mtfHInMask0.size();
		re.meanV = accumulate(mtfVInMask0.begin(), mtfVInMask0.end(), 0.0) / mtfHInMask0.size();
		re.meanV_freq2 = accumulate(mtfVInMask2.begin(), mtfVInMask2.end(), 0.0) / mtfHInMask0.size();
	}

	re.mtfMapH = mtfmapH0;
	re.mtfMapV = mtfmapV0;
	re.mtfMapH2 = mtfmapH2;
	re.mtfMapV2 = mtfmapV2;
	re.imgdraw = imgdraw;
	re.xPos = xlocMat;
	re.yPos = ylocMat;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "DenseMTF calculation successfully");
	return re;
}

CenterRects MLIQMetrics::MLdenseMTF::getGridCenterRects(cv::Mat img)
{

	string info = "------getGridCenterRects------";
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "getGridCenterRects calculation start");
	CenterRects re;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	m_binNum = binNum;
	//cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	//updateRectByRatio1(ROIRect, 1.0 / binNum);
	//img = GetROIMat(img, ROIRect);
	MLGridDetect grid;
	grid.setAccurateDetectionFlag(false);
	img = grid.rotateGridImg(img);
	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);
	cv::Mat imgResize;
	int reiseNum = IQMetricsParameters::GridResizeNum / binNum;
	cv::resize(img8, imgResize, img.size() / reiseNum);
	grid.SetbinNum(4);
	grid.SetChessboardUpdateFlag(false);
	GridRe gridRe = grid.getGridContour(imgResize);
	if (gridRe.flag == false)
	{
		re.flag = false;
		re.errMsg = info + gridRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "getGridCenterRects calculation: corners detection successfully");
	cv::Mat xlocMat = gridRe.xLocMat * reiseNum;
	cv::Mat ylocMat = gridRe.yLocMat * reiseNum;

	int row = xlocMat.rows;
	int col = xlocMat.cols;
	cv::Point2f center;
	center.x = xlocMat.at<float>(row / 2, col / 2);
	center.y = ylocMat.at<float>(row / 2, col / 2);

	int w = IQMetricsParameters::gridMTFWidth / m_binNum;
	int h = IQMetricsParameters::gridMTFHeight / m_binNum;
	cv::Rect rectT(center.x - w / 2, center.y - h * 2, w, h);
	cv::Rect rectB(center.x - w / 2, center.y + h, w, h);
	cv::Rect rectL(center.x - h * 2, center.y - w / 2, h, w);
	cv::Rect rectR(center.x + h, center.y - w / 2, h, w);
	drawPointOnImage(imgdraw, center, 16 / binNum);
	drawRectOnImage(imgdraw, rectT,16/binNum);
	drawRectOnImage(imgdraw, rectB, 16 / binNum);
	drawRectOnImage(imgdraw, rectL, 16 / binNum);
	drawRectOnImage(imgdraw, rectR, 16 / binNum);
	re.rectLeft = rectL;
	re.rectRight = rectR;
	re.rectTop = rectT;
	re.rectBottom = rectB;
	re.rectVec.clear();
	re.rectVec.push_back(rectT);
	re.rectVec.push_back(rectB);
	re.rectVec.push_back(rectL);
	re.rectVec.push_back(rectR);
	re.imgdraw = imgdraw;
	return re;
}

DenseMTFGridRe MLIQMetrics::MLdenseMTF::getDenseMTFGrid_CSV(cv::Mat img)
{
	string info = "------getDenseMTFGrid------";
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "DenseMTF calculation start");
	DenseMTFGridRe re;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	//if (binNum <= 0)
	//{
	//	re.flag = false;
	//	re.errMsg = info + "the image size is not right, please check the input image";
	//	return re;
	//}
	//m_binNum = binNum;

	//int binNumCen = IQMetricUtl::instance()->getBinNum(cv::Size(m_center.x * 2, m_center.y * 2));
	//double ratio = double(binNumCen) / double(binNum);
	//cv::Point2f cen = m_center * ratio;
	//int len = m_len / binNum;
	//cv::Rect rect0(cen.x - len / 2, cen.y - len / 2, len, len);
	//img = GetROIMat(img, rect0);

	MLGridDetect grid;
	grid.setAccurateDetectionFlag(false);
	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);
	cv::Mat imgResize;

	grid.SetbinNum(binNum);
	grid.SetChessboardUpdateFlag(false);
	GridRe gridRe = grid.getGridContour(img);
	std::shared_mutex rw_mutex;
	std::mutex mtx;
	if (gridRe.xLocMat.size() == cv::Size(15, 19))
	{
		//std::shared_lock lock(rw_mutex);
		mtx.lock();
		writeMatTOCSV(m_filepathx, gridRe.xLocMat);
		writeMatTOCSV(m_filepathy, gridRe.yLocMat);
		mtx.unlock();

	}
	if (gridRe.xLocMat.size() != cv::Size(15, 19))
	{
		//std::shared_lock lock(rw_mutex);  
		mtx.lock();
		cv::Mat xLocMat = readCSVToMat(m_filepathx);
		cv::Mat yLocMat = readCSVToMat(m_filepathy);
		mtx.unlock();

		gridRe = grid.getGridPreLoc(img, xLocMat, yLocMat);
	}
	if (gridRe.flag == false)
	{
		re.flag = false;
		re.errMsg = info + gridRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	return re;
}

CenterRects MLIQMetrics::MLdenseMTF::getGridRects(cv::Mat img, Point rowCol)
{
	cv::Mat xlocMat = readCSVToMat(m_filepathx);
	cv::Mat ylocMat = readCSVToMat(m_filepathy);

	CenterRects re = getRectMat(xlocMat, ylocMat, 0, rowCol);

	//TODO: 
	//if (rowCol.y == 1) {
	//	re.rectLeft.x += 60;
	//}
	//if (rowCol.y == 13) {
	//	re.rectRight.x -= 60;
	//}

	cv::Mat imgdraw = convertTo3Channels(img);
	cv::rectangle(imgdraw, re.rectTop, Scalar(0, 255, 0), 3);
	cv::rectangle(imgdraw, re.rectBottom, Scalar(0, 255, 0), 3);
	cv::rectangle(imgdraw, re.rectLeft, Scalar(0, 0, 255), 3);
	cv::rectangle(imgdraw, re.rectRight, Scalar(0, 0, 255), 3);
	return re;
}

CenterRects MLIQMetrics::MLdenseMTF::getRectMat(cv::Mat xlocMat, cv::Mat ylocMat, int flag, Point rowCol)
{
	CenterRects re;
	int w = IQMetricsParameters::gridMTFWidth / m_binNum;
	int h = IQMetricsParameters::gridMTFHeight / m_binNum;
	cv::Point2f center(xlocMat.at<float>(rowCol.x, rowCol.y), ylocMat.at<float>(rowCol.x, rowCol.y));
	{
		cv::Rect rectT(center.x - w / 2, center.y - h * 2, w, h);
		cv::Rect rectB(center.x - w / 2, center.y + h, w, h);
		cv::Rect rectL(center.x - h * 2, center.y - w / 2, h, w);
		cv::Rect rectR(center.x + h, center.y - w / 2, h, w);

		re.rectTop = rectT;
		re.rectBottom = rectB;
		re.rectLeft = rectL;
		re.rectRight = rectR;
	}
	return re;
}

cv::Mat MLIQMetrics::MLdenseMTF::getMtfMat(cv::Mat xlocMat, cv::Mat ylocMat, cv::Mat& imgdraw, cv::Mat rawImg, int flag, MTF_TYPE type)
{
	cv::Mat xl, xr, yl, yr;
	Scalar color(0, 0, 255);
	if (flag == 1) // 取每一行左右相邻的点
	{
		xl = xlocMat(cv::Range(0, xlocMat.rows), cv::Range(0, xlocMat.cols - 1));
		xr = xlocMat(cv::Range(0, xlocMat.rows), cv::Range(1, xlocMat.cols));
		yl = ylocMat(cv::Range(0, xlocMat.rows), cv::Range(0, xlocMat.cols - 1));
		yr = ylocMat(cv::Range(0, xlocMat.rows), cv::Range(1, xlocMat.cols));

		//m_maskV = cv::Mat(xl.size(), CV_8UC1, Scalar(0));
	}
	else // 取每一列上下相邻的点
	{
		xl = xlocMat(cv::Range(0, xlocMat.rows - 1), cv::Range(0, xlocMat.cols));
		xr = xlocMat(cv::Range(1, xlocMat.rows), cv::Range(0, xlocMat.cols));
		yl = ylocMat(cv::Range(0, xlocMat.rows - 1), cv::Range(0, xlocMat.cols));
		yr = ylocMat(cv::Range(1, xlocMat.rows), cv::Range(0, xlocMat.cols));
		color = Scalar(0, 255, 0);
		//m_maskH = cv::Mat(xl.size(), CV_8UC1, Scalar(0));
	}
	int roiW = IQMetricsParameters::denseMTFWidth / m_binNum;  // 330/1
	int roiH = IQMetricsParameters::denseMTFHeight / m_binNum; //110/1
	cv::Mat xlocMapROI = xl + (xr - xl) / 2.0; // 计算相邻点中点
	cv::Mat ylocMapROI = yl + (yr - yl) / 2.0;
	cv::Mat mtfmat(xlocMapROI.size(), CV_32FC1, Scalar(-1));
	if (flag == 0)
	{
		m_mtfmapH2 = cv::Mat(xlocMapROI.size(), CV_32FC1, Scalar(-1)); // 初始化

	}
	else
	{
		m_mtfmapV2 = cv::Mat(xlocMapROI.size(), CV_32FC1, Scalar(-1));

	}

	double pixel = IQMetricsParameters::pixel_size * m_binNum;
	double focallengh = IQMetricsParameters::FocalLength;
	double zoneAR = IQMetricsParameters::zoneARadius;
	cv::Point2f c0 = m_center;
//	double r = tan(zoneAR / 180 * CV_PI) * focallengh / pixel;
	//circle(imgdraw, c0, r, Scalar(0, 0, 255), 5);

	omp_lock_t lock;
	omp_init_lock(&lock);
#pragma omp parallel for 
	for (int i = 0; i < xlocMapROI.rows; i++)
	{
		for (int j = 0; j < xlocMapROI.cols; j++)
		{
			double x0 = xlocMapROI.at<float>(i, j);
			double y0 = ylocMapROI.at<float>(i, j);

			if (x0 > 1e-6 && y0 > 1e-6)
			{
				cv::Point2f c0(x0, y0); // 取中点坐标
				//circle(imgdraw, c0, 5, color, -1);
				cv::Rect rectT;
				if (flag == 0)
					rectT = cv::Rect(c0.x - roiW / 2, c0.y - roiH / 2, roiW, roiH); // 创建ROI 横向
				else
					rectT = cv::Rect(c0.x - roiH / 2, c0.y - roiW / 2, roiH, roiW);

				//if((flag==0&&i!=0||i!= xlocMapROI.rows-1)||(flag==1 && i!= 0 || i != xlocMapROI.cols - 1))

				if ((flag == 0 && j != 0 && j != xlocMapROI.cols - 1) || (flag == 1 && i != 0 && i != xlocMapROI.rows - 1)) // 去掉最左右最上下

				{
					cv::rectangle(imgdraw, rectT, color, 3); // 画图

					//if (flag == 1 && i != 0 && i != xlocMapROI.rows - 1)
					//	cv::rectangle(imgdraw, rectT, color, 3);
					//double fov = calculateFOV(c0);
					//int maskvalue = 0;
					//if (fov < zoneAR)
					//	maskvalue = 1;
					//if (flag == 0)
					//	m_maskH.at<uchar>(i, j) = maskvalue;
					//else
					//	m_maskV.at<uchar>(i, j) = maskvalue;
					cv::Mat roi = rawImg(rectT).clone();
					double mtf0 = -1;
					cv::Scalar m0, std0;
					cv::meanStdDev(roi, m0, std0); // 计算roi内的灰度均值和灰度标准差
					vector<double>mtfvec;
					if (std0(0) < 0)
						mtf0 = -1;
					else
					{
						//mtf0 = calculateMtf(roi, type);
                   // #pragma omp critical
						omp_set_lock(&lock);
						
							mtfvec = calculateMtf1(roi, type);  // 输出两个频率的mtf
						
						omp_unset_lock(&lock);

						

					}
					mtfmat.at<float>(i, j) = mtfvec[0];
					if (flag == 0)
						m_mtfmapH2.at<float>(i, j) = mtfvec[1];
					else
						m_mtfmapV2.at<float>(i, j) = mtfvec[1];
				}
				//else
					//mtfmat.at<float>(i, j) = -1;
			}
		}
	}
	omp_destroy_lock(&lock);

	return mtfmat;
}

void MLIQMetrics::MLdenseMTF::getMtfMat(cv::Mat xlocmat, cv::Mat ylocmat, cv::Mat& imgdraw, cv::Mat rawImg, MTF_TYPE type, cv::Mat& mtfH, cv::Mat& mtfV)
{
	mtfH = cv::Mat(xlocmat.size()-Size(2,2), CV_32FC1, Scalar(-1));
	mtfV = cv::Mat(xlocmat.size() - Size(2, 2), CV_32FC1, Scalar(-1));
	for (int i = 1; i < xlocmat.rows-1; i++)
	{
		for (int j = 1; j<xlocmat.cols-1; j++)
		{
			cv::Point2f c0(xlocmat.at<float>(i, j), ylocmat.at<float>(i, j));
			cv::Point2f ct(xlocmat.at<float>(i-1, j), ylocmat.at<float>(i-1, j));
			cv::Point2f cb(xlocmat.at<float>(i+1, j), ylocmat.at<float>(i+1, j));
			cv::Point2f cl(xlocmat.at<float>(i, j-1), ylocmat.at<float>(i, j-1));
			cv::Point2f cr(xlocmat.at<float>(i, j+1), ylocmat.at<float>(i, j+1));
			//circle(imgdraw, c0, 5, Scalar(0, 0, 255), -1);
			//circle(imgdraw, cl, 5, Scalar(0, 255, 255), -1);
			//circle(imgdraw, cr, 5, Scalar(255, 0, 255), -1);
			//circle(imgdraw, ct, 5, Scalar(0, 255, 0), -1);
			//circle(imgdraw, cb, 5, Scalar(255, 0, 0), -1);

			int w = IQMetricsParameters::gridMTFWidth/m_binNum;
			int h = IQMetricsParameters::gridMTFHeight/m_binNum;
			cv::Point2f c0T = (c0 + ct)/2.0;
			cv::Point2f c0B = (c0 + cb) / 2.0;
			cv::Point2f c0L = (c0 + cl) / 2.0;
			cv::Point2f c0R = (c0 + cr) / 2.0;
			//circle(imgdraw, c0T, 5, Scalar(0, 0, 255), -1);
			//circle(imgdraw, c0B, 5, Scalar(0, 0, 255), -1);
			//circle(imgdraw, c0L, 5, Scalar(0, 0, 255), -1);
			//circle(imgdraw, c0R, 5, Scalar(0, 0, 255), -1);

			

		/*	cv::Rect rectT(c0.x-w/2,c0.y-h*2,w,h);
			cv::Rect rectB(c0.x - w / 2, c0.y + h, w, h);
			cv::Rect rectL(c0.x - h*2, c0.y - w / 2, h, w);
			cv::Rect rectR(c0.x + h, c0.y - w / 2, h, w);*/

			cv::Rect rectT(c0T.x - w / 2, c0T.y - h /2, w, h);
			cv::Rect rectB(c0B.x - w / 2, c0B.y - h / 2, w, h);
			cv::Rect rectL(c0L.x - h / 2, c0L.y - w / 2, h, w);
			cv::Rect rectR(c0R.x - h / 2, c0R.y - w / 2, h, w);

			Scalar stdT, stdB, stdL, stdR;
			Scalar meanT;
			cv::meanStdDev(rawImg(rectT), meanT,stdT);
			cv::meanStdDev(rawImg(rectB), meanT, stdB);
			cv::meanStdDev(rawImg(rectL), meanT, stdL);
			cv::meanStdDev(rawImg(rectR), meanT, stdR);
			double mtfT = 0, mtfB=0, mtfL=0, mtfR=0;
			double mtfh = 0,mtfv=0;
			int numh = 0,numv=0;
			if (stdT(0) > 10)
			{
				 mtfT = calculateMtf(rawImg(rectT), CROSS);
				 drawRectOnImage(imgdraw, rectT);
				 mtfh = mtfh + mtfT;
				 numh++;
			}
			if (stdB(0) > 10)
			{
				mtfB = calculateMtf(rawImg(rectB), CROSS);
				drawRectOnImage(imgdraw, rectB);
				mtfh = mtfh + mtfB;
				numh++;
			}
			if (stdL(0) > 10)
			{
				mtfL = calculateMtf(rawImg(rectL), CROSS);
				drawRectOnImage(imgdraw, rectL);
				mtfv = mtfv + mtfL;
				numv++;
			}
			if (stdR(0) > 10)
			{
				mtfR = calculateMtf(rawImg(rectR), CROSS);
				drawRectOnImage(imgdraw, rectR);
				mtfv = mtfv + mtfR;
				numv++;
			}
			mtfh = mtfh / numh;
			mtfv = mtfv / numv;
			mtfH.at<float>(i-1, j-1) = mtfh;
			mtfV.at<float>(i-1, j-1) = mtfv;

		}
	}
}

double MLIQMetrics::MLdenseMTF::calculateMtf(cv::Mat roi, MTF_TYPE type)
{
	double pixle = IQMetricsParameters::pixel_size * m_binNum;
	double focalLenght = IQMetricsParameters::FocalLength;
	double freq = IQMetricsParameters::mtfFreq;
	PipeLine* mtfPipeline = new PipeLine();
	mtfPipeline->SetPixelValue(pixle);
	mtfPipeline->SetBinning(1);
	mtfPipeline->set_freq_unit(FREQ_UNIT::lp_deg, focalLenght);
	int ret = mtfPipeline->culc_mtf(roi, type);
	double mtfH = mtfPipeline->getMtfByFreq(freq);
	
	if (ret >= 0)
	{
		return mtfH;
	}
	delete mtfPipeline;
	return -1;
}

vector<double> MLIQMetrics::MLdenseMTF::calculateMtf1(cv::Mat roi, MTF_TYPE type)
{
	vector<double>mtfvec;
	double pixle = IQMetricsParameters::pixel_size * m_binNum;
	double focalLenght = IQMetricsParameters::FocalLength;
	double freq1 = IQMetricsParameters::mtfFreq;  // 6.25
	double freq2 = IQMetricsParameters::mtfFreq2; // 12.5
	PipeLine* mtfPipeline = new PipeLine();
	mtfPipeline->SetPixelValue(pixle); // 图像大小
	mtfPipeline->SetBinning(1);
	mtfPipeline->set_freq_unit(FREQ_UNIT::lp_deg, focalLenght); 

	int ret = mtfPipeline->culc_mtf(roi, type);  // 计算MTF

	double mtfH = mtfPipeline->getMtfByFreq(freq1);  // 读取数值
	double mtfH2 = mtfPipeline->getMtfByFreq(freq2);
	if (ret >= 0)
	{
		 mtfvec.push_back(mtfH);
		 mtfvec.push_back(mtfH2);
	}
	else
	{
		mtfvec.push_back(-1);
		mtfvec.push_back(-1);
	}
	delete mtfPipeline;
	return mtfvec;
}

double MLIQMetrics::MLdenseMTF::calculateFOV(cv::Point2f pt)
{
	double pixel = IQMetricsParameters::pixel_size * m_binNum;
	double focallengh = IQMetricsParameters::FocalLength;
	double dis = Getdistance(pt, m_center);
	double fov = atan(dis * pixel / focallengh) * 180 / CV_PI;
	return fov;
}

void MLIQMetrics::MLdenseMTF::getDenseMTFData(cv::Mat seqMat, cv::Mat mask, DenseMTFRe& re, int flag)
{
	cv::Mat seqmatMask = seqMat + 1e-6;
	seqmatMask.setTo(0, mask == 0);
	seqmatMask.convertTo(seqmatMask, CV_64FC1);
	arma::mat data(reinterpret_cast<double*>(seqmatMask.data), seqmatMask.cols, seqmatMask.rows);
	arma::vec datavec = arma::vectorise(data);
	arma::uvec index = arma::find(data > 0);
	arma::vec dataMask = data(index);
	if (flag == 0)
	{
		re.p20H = percentile(dataMask, 20);
		re.p75H = percentile(dataMask, 75);
		re.p25H = percentile(dataMask, 25);
		re.p75_25H = re.p75H - re.p25H;
		re.medianH = arma::median(dataMask);
	}
	else
	{
		re.p20V = percentile(dataMask, 20);
		re.p75V = percentile(dataMask, 75);
		re.p25V = percentile(dataMask, 25);
		re.p75_25V = re.p75V - re.p25V;
		re.medianV = arma::median(dataMask);
	}
}

double MLIQMetrics::MLdenseMTF::calculateMatMean(cv::Mat mat)
{
	return cv::mean(mat)(0);
}

double MLIQMetrics::MLdenseMTF::calculateMatMin(cv::Mat mat)
{
	double min;
	cv::minMaxLoc(mat, &min);
	return min;
}

cv::Mat MLIQMetrics::MLdenseMTF::calculateMTFHor(cv::Mat mtfmapH)
{
	cv::Mat mtfmapH01 = mtfmapH(cv::Range(0, mtfmapH.rows), cv::Range(1, mtfmapH.cols - 1));
	cv::Mat mtfmapH_top = mtfmapH(cv::Range(0, mtfmapH.rows - 1), cv::Range(1, mtfmapH.cols - 1));
	cv::Mat mtfmapH_bottom = mtfmapH(cv::Range(1, mtfmapH.rows), cv::Range(1, mtfmapH.cols - 1));
	cv::Mat mtfmapH0 = (mtfmapH_top + mtfmapH_bottom) / 2.0;
	return mtfmapH0;
}

cv::Mat MLIQMetrics::MLdenseMTF::calculateMTFVer(cv::Mat mtfmapV)
{
	cv::Mat mtfmapV01 = mtfmapV(cv::Range(1, mtfmapV.rows - 1), cv::Range(0, mtfmapV.cols));
	cv::Mat mtfmapV_left = mtfmapV(cv::Range(1, mtfmapV.rows - 1), cv::Range(0, mtfmapV.cols - 1));
	cv::Mat mtfmapV_right = mtfmapV(cv::Range(1, mtfmapV.rows - 1), cv::Range(1, mtfmapV.cols));
	cv::Mat mtfmapV0 = (mtfmapV_left + mtfmapV_left) / 2.0;
	return mtfmapV0;
}

vector<double> MLIQMetrics::MLdenseMTF::getMTFRect(cv::Mat mtfMap,cv::Mat xloc, cv::Mat yloc, cv::Rect rect, vector<double>& mtfOutMask)
{
	vector<double> mtfInMask;
	if(mtfMap.size()!=xloc.size())
		return vector<double>();

	for (int i = 0; i < xloc.rows; i++)
	{
		for (int j = 0; j < xloc.cols; j++)
		{
			double x = xloc.at<float>(i, j);
			double y = yloc.at<float>(i, j);
			double mtf0 = mtfMap.at<float>(i, j);
			if (rect.contains(cv::Point2f(x, y)))
				mtfInMask.push_back(mtf0);
			else
				mtfOutMask.push_back(mtf0);
		}
	}
	return mtfInMask;
}

void MLIQMetrics::MLdenseMTF::calculateMtfValue(cv::Mat mtfmapH, double& min, double& mean)
{
	cv::Mat mtfH0;

	cv::Mat mask = mtfmapH > 20;
	cv::Scalar meanVal = cv::mean(mtfmapH, mask);
	double minVal, maxVal;
	cv::minMaxLoc(mtfmapH, &minVal, &maxVal, nullptr, nullptr, mask);
	mean = meanVal(0);
	min= minVal;
}

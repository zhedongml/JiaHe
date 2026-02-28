#include "pch.h"
#include "ML_denseMTF.h"
#include"MLCherkerboardDetect.h"
#include"mtfpipelineplugin.h"
#include"pipeline.h"
#include"MLContrastRatio.h"
#include"LogPlus.h"
#include"ml_gridDetect.h"
#include"ML_gridPatternDetection.h"
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
void MLIQMetrics::MLdenseMTF::setCheckerDegree(int deg)
{
	//m_flag = deg;
}
void MLIQMetrics::MLdenseMTF::setFOVType(FOVTYPE type)
{
	m_fovType = type;
}
DenseMTFRe MLIQMetrics::MLdenseMTF::getDenseMTFChecker(cv::Mat img, int flag)
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
	cv::Rect rectROI = IQMetricsParameters::ROIRect;
	updateRectByRatio1(rectROI, 1.0 / binNum);
	if (m_fovType == BIGFOV)
		rectROI = cv::Rect(0, 0, -1, -1);
	cv::Mat imgROI = GetROIMat(img, rectROI);
	cv::Mat img8 = convertToUint8(imgROI);
	cv::Mat imgdraw = convertTo3Channels(img8);
	MLContrastRatio cr;
	cv::Rect rect;
	if (m_fovType != BIGFOV)
	{
		cv::RotatedRect rectR = cr.getCherkerBorder(img8, rect);
		updateRotateImg(imgROI, rectR.angle);
		updateRotateImg(img8, rectR.angle);
		updateRotateImg(imgdraw, rectR.angle);
	}
	
	cv::Mat imgResize;
	int reiseNum = 4 / binNum;
	cv::resize(img8, imgResize, imgROI.size() / reiseNum);
	MLCherkerboardDetect cb;
	if (flag == 2)
	{
		cb.SetChecssboardPointsClusters(15*4/ 4);
		cb.SetChessboardxyClassification(15*4/ 4);
	}
	else if (flag==1)

	{
		cb.SetChecssboardPointsClusters(200/ 4);
		cb.SetChessboardxyClassification(200/ 4);
	}
	cb.SetChessboardUpdateFlag(true);
	CheckerboardRe checkerRe;
	if(flag==2)
	  checkerRe = cb.detectChessboardCorner(imgResize, 0.25, binNum);
	else 
	 checkerRe = cb.detectChessboardTemplate1(imgResize, 0.25, 4);
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

	cv::Mat mtfmapH = getMtfMat(xlocMat, ylocMat, imgdraw, imgROI, 0, SLANT,flag);
	cv::Mat mtfmapV = getMtfMat(xlocMat, ylocMat, imgdraw, imgROI, 1, SLANT, flag);
	//getDenseMTFData(mtfmapH, m_maskH, re, 0);
	//getDenseMTFData(mtfmapV, m_maskV, re, 1);
	mtfmapH = mtfmapH(cv::Range(0, mtfmapH.rows), cv::Range(1, mtfmapH.cols-1));
	m_mtfmapH2 = m_mtfmapH2(cv::Range(0, m_mtfmapH2.rows), cv::Range(1, m_mtfmapH2.cols - 1));
	mtfmapV = mtfmapV(cv::Range(1, mtfmapV.rows-1), cv::Range(0, mtfmapV.cols));
	m_mtfmapV2 = m_mtfmapV2(cv::Range(1, m_mtfmapV2.rows - 1), cv::Range(0, m_mtfmapV2.cols));

	re.mtfMapH = mtfmapH;
	re.mtfMapV = mtfmapV;
	re.mtfMapH2 = m_mtfmapH2;
	re.mtfMapV2 = m_mtfmapV2;
	re.imgdraw = imgdraw;
	re.xPos = xlocMat;
	re.yPos = ylocMat;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "DenseMTF calculation successfully");
	return re;
}

DenseMTFGridRe MLIQMetrics::MLdenseMTF::getDenseMTFGrid(const cv::Mat img)
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
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		return re;
	}
	m_binNum = binNum;
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	if (m_fovType == BIGFOV)
		ROIRect = cv::Rect(0, 0, -1, -1);
	cv::Mat imgROI = GetROIMat(img, ROIRect);
	MLGridDetect grid;
	grid.setAccurateDetectionFlag(false);
	imgROI = grid.rotateGridImg(imgROI);
	cv::Mat img8 = convertToUint8(imgROI);
	cv::Mat imgdraw = convertTo3Channels(img8);
	cv::Mat imgResize;
	int resizeNum = IQMetricsParameters::ResizeNum / binNum;
	int reiseNum = resizeNum / binNum;
	cv::resize(img8, imgResize, img8.size() / reiseNum);
	grid.SetbinNum(resizeNum);
	grid.SetChessboardUpdateFlag(false);
	GridRe gridRe = grid.getGridContour(imgResize);
	//MLGridPatternDetection grid;
	//grid.setBinNum(binNum);
	//grid.setFOVType(SMALLFOV);
	//GridRe gridRe = grid.getGridLoc(img);
	//int reiseNum = 1;

	if (gridRe.flag == false)
	{
		re.flag = false;
		re.errMsg = info + gridRe.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "DenseMTF calculation: corners detection successfully");
	m_center = IQMetricsParameters::opticalCenter / binNum;
	cv::Mat xlocMat = gridRe.xLocMat * reiseNum;
	cv::Mat ylocMat = gridRe.yLocMat * reiseNum;
	//xlocMat = xlocMat(cv::Range(1,xlocMat.rows-1),cv::Range(1,xlocMat.cols-1));
	//ylocMat = ylocMat(cv::Range(1, ylocMat.rows - 1), cv::Range(1, ylocMat.cols - 1));

	cv::Mat mtfmapH, mtfmapV;
	//getMtfMat(xlocMat, ylocMat, imgdraw, img, CROSS, mtfmapH0, mtfmapV0);
	mtfmapH = getMtfMat(xlocMat, ylocMat, imgdraw, imgROI, 0, CROSS);
	cv::Mat mtfmapH0 = calculateMTFHor(mtfmapH);
	cv::Mat mtfmapH2 = calculateMTFHor(m_mtfmapH2);

	mtfmapV = getMtfMat(xlocMat, ylocMat, imgdraw, imgROI, 1, CROSS);
	cv::Mat mtfmapV0 = calculateMTFVer(mtfmapV);
	cv::Mat mtfmapV2= calculateMTFVer(m_mtfmapV2);


	Scalar meanH = cv::mean(mtfmapH0); 
	Scalar meanV = cv::mean(mtfmapV0);
	double minH, minV;
	cv::minMaxLoc(mtfmapH0, &minH,NULL,NULL,NULL);
	cv::minMaxLoc(mtfmapV0, &minV, NULL, NULL, NULL);
	re.meanH = meanH(0);
	re.meanV = meanV(0);
	re.minH = minH;
	re.minV = minV;
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
	int reiseNum = 4 / binNum;
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

cv::Mat MLIQMetrics::MLdenseMTF::getMtfMat(cv::Mat xlocMat, cv::Mat ylocMat, cv::Mat& imgdraw, cv::Mat rawImg, int flag, MTF_TYPE type, int typeFlag)
{
	cv::Mat xl, xr, yl, yr;
	Scalar color(0, 0, 255);
	if (flag == 1)
	{
		xl = xlocMat(cv::Range(0, xlocMat.rows), cv::Range(0, xlocMat.cols - 1));
		xr = xlocMat(cv::Range(0, xlocMat.rows), cv::Range(1, xlocMat.cols));
		yl = ylocMat(cv::Range(0, xlocMat.rows), cv::Range(0, xlocMat.cols - 1));
		yr = ylocMat(cv::Range(0, xlocMat.rows), cv::Range(1, xlocMat.cols));

		//m_maskV = cv::Mat(xl.size(), CV_8UC1, Scalar(0));
	}
	else
	{
		xl = xlocMat(cv::Range(0, xlocMat.rows - 1), cv::Range(0, xlocMat.cols));
		xr = xlocMat(cv::Range(1, xlocMat.rows), cv::Range(0, xlocMat.cols));
		yl = ylocMat(cv::Range(0, xlocMat.rows - 1), cv::Range(0, xlocMat.cols));
		yr = ylocMat(cv::Range(1, xlocMat.rows), cv::Range(0, xlocMat.cols));
		color = Scalar(0, 255, 0);
		//m_maskH = cv::Mat(xl.size(), CV_8UC1, Scalar(0));
	}
	int roiW = IQMetricsParameters::gridMTFWidth / m_binNum;
	int roiH = IQMetricsParameters::gridMTFHeight / m_binNum;
	if (typeFlag == 1)
	{
		roiW = IQMetricsParameters::checkerMTFWidth_2 / m_binNum;
		roiH = IQMetricsParameters::checkerMTFHeight_2 / m_binNum;
	}
	else if (typeFlag == 2)
	{
		roiW = IQMetricsParameters::checkerMTFWidth_05 / m_binNum;
		roiH = IQMetricsParameters::checkerMTFHeight_05 / m_binNum;
	}

	cv::Mat xlocMapROI = xl + (xr - xl) / 2.0;
	cv::Mat ylocMapROI = yl + (yr - yl) / 2.0;
	cv::Mat mtfmat(xlocMapROI.size(), CV_32FC1, Scalar(-1));
	if (flag == 0)
	{
		m_mtfmapH2 = cv::Mat(xlocMapROI.size(), CV_32FC1, Scalar(-1));

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

#pragma omp parallel for
	for (int i = 0; i < xlocMapROI.rows; i++)
	{
		for (int j = 0; j < xlocMapROI.cols; j++)
		{
			double x0 = xlocMapROI.at<float>(i, j);
			double y0 = ylocMapROI.at<float>(i, j);

			if (x0 > 1e-6 && y0 > 1e-6)
			{
				cv::Point2f c0(x0, y0);
				//circle(imgdraw, c0, 5, color, -1);
				cv::Rect rectT;
				if (flag == 0)
					rectT = cv::Rect(c0.x - roiW / 2, c0.y - roiH / 2, roiW, roiH);
				else
					rectT = cv::Rect(c0.x - roiH / 2, c0.y - roiW / 2, roiH, roiW);

				//if((flag==0&&i!=0||i!= xlocMapROI.rows-1)||(flag==1 && i!= 0 || i != xlocMapROI.cols - 1))

				if ((flag == 0 && j != 0 && j != xlocMapROI.cols - 1) || (flag == 1 && i != 0 && i != xlocMapROI.rows - 1))

				{
					cv::rectangle(imgdraw, rectT, color, 3);
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
					cv::Mat roi = rawImg(rectT&cv::Rect(0,0,rawImg.cols,rawImg.rows));
					double mtf0 = -1;
					cv::Scalar m0, std0;
					cv::meanStdDev(roi, m0, std0);
					vector<double>mtfvec;
					if (std0(0) < 0)
						mtf0 = -1;
					else
					{
						//mtf0 = calculateMtf(roi, type);
						mtfvec = calculateMtf1(roi, type);
						//mtfvec = calculateMtf2(roi,type, rectT.tl());

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
	return -1;
}

vector<double> MLIQMetrics::MLdenseMTF::calculateMtf1(cv::Mat roi, MTF_TYPE type)
{
	vector<double>mtfvec;
	double pixle = IQMetricsParameters::pixel_size * m_binNum;
	double focalLenght = IQMetricsParameters::FocalLength;
	double freq1 = IQMetricsParameters::mtfFreq;
	double freq2 = IQMetricsParameters::mtfFreq2;
	PipeLine* mtfPipeline = new PipeLine();
	mtfPipeline->SetPixelValue(pixle);
	mtfPipeline->SetBinning(1);
	mtfPipeline->set_freq_unit(FREQ_UNIT::lp_deg, focalLenght);
	int ret = mtfPipeline->culc_mtf(roi, type);
	double mtfH = mtfPipeline->getMtfByFreq(freq1);
	double mtfH2 = mtfPipeline->getMtfByFreq(freq2);
	delete mtfPipeline;
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
	return mtfvec;
}

vector<double> MLIQMetrics::MLdenseMTF::calculateMtf2(cv::Mat roi, MTF_TYPE type, cv::Point2f c1)
{
	cv::Point2f opc = IQMetricsParameters::opticalCenter;

	double pixle = IQMetricsParameters::pixel_size * m_binNum;
	double focalLenght = IQMetricsParameters::FocalLength;
	double freq1 = IQMetricsParameters::mtfFreq;
	double freq2 = IQMetricsParameters::mtfFreq2;

	double dis = Getdistance(opc, c1);
	double theta = atan(dis * pixle / focalLenght);
	focalLenght = focalLenght / cos(theta);
		vector<double>mtfvec;
	PipeLine* mtfPipeline = new PipeLine();
	mtfPipeline->SetPixelValue(pixle);
	mtfPipeline->SetBinning(1);
	mtfPipeline->set_freq_unit(FREQ_UNIT::lp_deg, focalLenght);
	int ret = mtfPipeline->culc_mtf(roi, type);
	double mtfH = mtfPipeline->getMtfByFreq(freq1);
	double mtfH2 = mtfPipeline->getMtfByFreq(freq2);
	delete mtfPipeline;
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

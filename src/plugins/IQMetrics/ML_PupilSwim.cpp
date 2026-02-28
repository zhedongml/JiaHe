#include "pch.h"
#include "ML_PupilSwim.h"
#include"MLCherkerboardDetect.h"
#include "MLContrastRatio.h"
#include"LogPlus.h"
#include"CrossCenter.h"
using namespace cv;
using namespace std;
using namespace MLImageDetection;
using namespace MLIQMetrics;
MLIQMetrics::MLPupilSwim::MLPupilSwim()
{
}

MLIQMetrics::MLPupilSwim::~MLPupilSwim()
{
}

void MLIQMetrics::MLPupilSwim::setFOVType(FOVTYPE type)
{
	m_fovType = type;
}

PupilSwimRe MLIQMetrics::MLPupilSwim::getPupilSwim(const cv::Mat img1, const cv::Mat img2)
{

	string info = "------getPupilSwim------";
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "PupilSwim calculation start");

	PupilSwimRe re;
	if (img1.empty() || img2.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image us NULL";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img1.size());
	if (binNum <= 0)
	{
		re.flag = false;
		re.errMsg = info + "the image size is not right, please check the input image";
		return re;
	}

	m_binNum = binNum;
	double pix2Arcmin = IQMetricUtl::instance()->getPix2Arcmin(img1.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
    updateRectByRatio1(ROIRect, 1.0/binNum);
	cv::Mat img1ROI = GetROIMat(img1, ROIRect);
	cv::Mat img2ROI = GetROIMat(img2, ROIRect);
	MLContrastRatio cr;
	cv::Rect rect;
	cv::Mat img18 = convertToUint8(img1ROI);
	cv::RotatedRect rectR = cr.getCherkerBorder(img18, rect);
	updateRotateImg(img1ROI, rectR.angle);
	updateRotateImg(img2ROI, rectR.angle);

	cv::Point2f center((float)(img18.cols / 2), (float)(img18.rows / 2));
	rect = updateRectByRatio(rect, 1.2);
	cv::Rect rectAll(0, 0, img1.cols, img1.rows);
	img1ROI = img1ROI(rect& rectAll);
	img2ROI = img2ROI(rect& rectAll);

	MLCherkerboardDetect cb;
	cb.SetChecssboardPointsClusters(80/binNum);
	cb.SetChessboardxyClassification(80/binNum);
	cb.SetChessboardUpdateFlag(false);
	//CheckerboardRe checkerRe1 = cb.detectChessboardTemplate1(img1, 0.25, binNum);
	//CheckerboardRe checkerRe2 = cb.detectChessboardTemplate1(img2, 0.25, binNum);
	int resizeNum = 1;
	cv::Mat img1Re, img2Re;
	cv::resize(img1ROI, img1Re, img1ROI.size() / resizeNum);
	cv::resize(img2ROI, img2Re, img2ROI.size() / resizeNum);
	CheckerboardRe checkerRe1;// = cb.detectChessboardCorner(img1Re, 0.25, binNum);
	
	checkerRe1 = cb.detectChessboardTemplate1(img1Re, 0.25, 4);
	
//if(checkerRe1.xLocMat.size()!= m_size)
	//	checkerRe1 = cb.detectChessboardCorner(img1Re, 0.25, binNum);
	CheckerboardRe checkerRe2;// = cb.detectChessboardCorner(img2Re, 0.25, binNum);
	checkerRe2 = cb.detectChessboardTemplate1(img2Re, 0.25, 4);

	//if (checkerRe2.xLocMat.size() != m_size)
	//	checkerRe2 = cb.detectChessboardCorner(img2Re, 0.25, binNum);

	if (checkerRe1.flag==false || checkerRe2.flag==false)
	{
		re.flag = false;
		re.errMsg = checkerRe1.errMsg + checkerRe2.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}

	re.imgdraw1 = checkerRe1.img_draw;
	re.imgdraw2 = checkerRe2.img_draw;
	if (false)
	{
		cv::Mat img18 = convertToUint8(img1Re);
		cv::Mat imgdraw1 = convertTo3Channels(img18);
		cv::Mat img28 = convertToUint8(img2Re);
		cv::Mat imgdraw2 = convertTo3Channels(img28);
		updateCornersExactly(img18, checkerRe1,imgdraw1);
		updateCornersExactly(img28, checkerRe2,imgdraw2);
		re.imgdraw1 = imgdraw1;
		re.imgdraw2 = imgdraw2;
	}
	if (checkerRe1.xLocMat.size() != checkerRe2.xLocMat.size())
	{
		re.flag = false;
		re.errMsg = "the two checkersize is not equal";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	checkerRe1.xLocMat = checkerRe1.xLocMat * resizeNum;
	checkerRe1.yLocMat = checkerRe1.yLocMat * resizeNum;
	checkerRe2.xLocMat = checkerRe2.xLocMat * resizeNum;
	checkerRe2.yLocMat = checkerRe2.yLocMat * resizeNum;
	re.xLoc1 = checkerRe1.xLocMat;
	re.yLoc1 = checkerRe1.yLocMat;
	re.xLoc2 = checkerRe2.xLocMat;
	re.yLoc2 = checkerRe2.yLocMat;
	if (checkerRe1.flag && checkerRe2.flag)
	{
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "PupilSwim calculation: corners detection successfully");
		cv::Mat subx = checkerRe1.xLocMat - checkerRe2.xLocMat;
		cv::Mat suby = checkerRe1.yLocMat - checkerRe2.yLocMat;
		subx.convertTo(subx, CV_64FC1);
		suby.convertTo(suby, CV_64FC1);
		arma::mat subxmat(reinterpret_cast<double*>(subx.data), subx.cols, subx.rows);
		arma::mat subymat(reinterpret_cast<double*>(suby.data), suby.cols, suby.rows);
		arma::mat degmat(subx.rows, subx.cols);
		int row = subx.rows;
		int col = subx.cols;
		double checkerDensity = IQMetricsParameters::CheckerDensity;
		arma::vec xdegvec = arma::linspace((-col + 1) / 2.0, (col - 1) / 2.0, col) * checkerDensity;
		arma::vec ydegvec = arma::linspace((-row + 1) / 2.0, (row - 1) / 2.0, row) * checkerDensity;

		arma::mat xdegmat = arma::repmat(xdegvec, 1, row);
		arma::mat ydegmat = arma::repmat(ydegvec, 1, col);
		ydegmat = ydegmat.t();
		cv::Mat xdegMat(xdegmat.n_cols, xdegmat.n_rows, CV_64FC1, xdegmat.memptr());
		cv::Mat ydegMat(ydegmat.n_cols, ydegmat.n_rows, CV_64FC1, ydegmat.memptr());
		arma::vec diffx = arma::vectorise(subxmat);
		arma::vec diffy = arma::vectorise(subymat);
		arma::vec xdeg = arma::vectorise(xdegmat);
		arma::vec ydeg = arma::vectorise(ydegmat);

		arma::uvec indexNan = arma::find_finite(diffx);
		arma::vec diffxnan = diffx.elem(indexNan);
		arma::vec diffynan = diffy.elem(indexNan);
		arma::vec xdegnan = xdeg.elem(indexNan);
		arma::vec ydegnan = ydeg.elem(indexNan);		 
		arma::vec px = arma::polyfit(xdegnan, diffxnan, 1);
		arma::vec py = arma::polyfit(ydegnan, diffynan, 1);
		arma::vec predict_x = arma::polyval(px, xdegnan);
		arma::vec predict_y = arma::polyval(py, ydegnan);
		arma::vec residual_x = diffxnan - predict_x;
		arma::vec residual_y = diffynan - predict_y;
		//calculate pupil swim
		arma::vec mag = pix2Arcmin * arma::sqrt(arma::pow(residual_x, 2) + arma::pow(residual_y, 2));
		arma::vec mag2 = arma::pow(mag, 2);
		double mean = arma::mean(mag2);
		double rms = sqrt(mean);
		double p90 = percentile(mag, 90);
		re.rms = rms;
		re.p90 = p90;
	}
	else
	{
		re.flag = false;
		re.errMsg = checkerRe1.errMsg + checkerRe2.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "PupilSwim calculation successfully");
	return re;
}

void MLIQMetrics::MLPupilSwim::updateCornersExactly(cv::Mat img, MLImageDetection::CheckerboardRe& checkerRe,cv::Mat& imgdraw)
{
	int len = m_len / m_binNum;
	len = 20;
	for (int i = 0; i < checkerRe.xLocMat.rows; i++)
	{
		for (int j = 0; j < checkerRe.xLocMat.cols; j++)
		{
			double x0 = checkerRe.xLocMat.at<float>(i, j);
			double y0 = checkerRe.yLocMat.at<float>(i, j);
			if (x0 > 1e-6 && y0 > 1e-6)
			{
				cv::Rect rect(x0 - len / 2, y0 - len / 2, len, len);
				cv::Mat roi = img(rect).clone();
				cv::Scalar m, std0;
				cv::meanStdDev(roi, m,std0);
				//cout << std0(0) << endl;
				if (std0(0) < 10)
				{
					checkerRe.xLocMat.at<float>(i, j) =NAN;
					checkerRe.yLocMat.at<float>(i, j) = NAN;
				}
				else
				{
					circle(imgdraw, cv::Point2f(x0, y0), 2, Scalar(0, 255, 0), -1);
				}
	/*			else
				{
					cv::Point2f pt0 = getCornerLocation1(roi);
					if (pt0.x > 1e-6 && pt0.y > 1e-6)
					{
						checkerRe.xLocMat.at<float>(i, j) = pt0.x + rect.x;
						checkerRe.yLocMat.at<float>(i, j) = pt0.y + rect.y;
						circle(imgdraw, cv::Point2f(x0, y0), 2, Scalar(0, 0, 255), -1);
						circle(imgdraw, pt0 + cv::Point2f(rect.tl()), 2, Scalar(0, 255, 0), -1);
					}
				}*/
			}

		}
	}
}

cv::Point2f MLIQMetrics::MLPupilSwim::getCornerLocation(cv::Mat roi)
{
	cv::Mat roidraw = convertTo3Channels(roi);


	vector<cv::Point2f>corners;
	int maxcorners = 1;
	double qualityLevel = 0.01;
	double minDistance = 20;
	int blockSize = 20/m_binNum;
	double k = 0.06;
	goodFeaturesToTrack(roi, corners, maxcorners, qualityLevel, minDistance, Mat(), blockSize, false, k);
	Size winSize = Size(5, 5);
	Size zeroZone = Size(-1, -1);
	//TermCriteria criteria = TermCriteria(TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001);
	 TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 40, 0.001);
     if(corners.size()<1)
		 return cv::Point2f(0, 0);
	 cv::cornerSubPix(roi, corners, winSize, zeroZone, criteria);
	drawPointsOnImage(roidraw, corners, 2, Scalar(0, 0, 255));
	if (corners.size() > 0)
		return corners[0];
	else
		return cv::Point2f(0, 0);
}

cv::Point2f MLIQMetrics::MLPupilSwim::getCornerLocation1(cv::Mat roi)
{
	cv::Mat roidraw = convertTo3Channels(roi);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	morphologyEx(roi, roi, MORPH_GRADIENT, kernel, Point(-1, -1));
	cv::Mat imgth;
	cv::threshold(roi, imgth, 0, 255, THRESH_OTSU);
	CrossCenter cc;
	cv::Point2f center = cc.find_centerLINES(roi);
	circle(roidraw, center, 1, Scalar(0, 0, 255), -1);
	return center;
}

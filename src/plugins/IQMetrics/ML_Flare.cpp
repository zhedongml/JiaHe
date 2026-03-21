#include "pch.h"
#include "ML_Flare.h"
#include"LogPlus.h"

using namespace MLImageDetection;
using namespace MLIQMetrics;
using namespace cv;
MLIQMetrics::MLFlare::MLFlare()
{
}

MLIQMetrics::MLFlare::~MLFlare()
{
}

void MLIQMetrics::MLFlare::setIsSLB(bool flag)
{
	m_IsSLB = flag;
}

void MLIQMetrics::MLFlare::setPatternCenter(cv::Point2f cen)
{
	m_center = cen;
}

FlareRe MLIQMetrics::MLFlare::getFlare(const cv::Mat imgAutoRaw, const cv::Mat imgOverRaw)// =========this========
{
	string info = "-------getFlare------";
	FlareRe re;
	if (imgAutoRaw.empty()|| imgOverRaw.empty())
	{
		re.flag = false;
		re.errMsg = info + "The input image is null!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	cv::Mat imgAuto = IQMetricUtl::instance()->getRotationAndFlipImg(imgAutoRaw, m_IsSLB);
	cv::Mat imgOver = IQMetricUtl::instance()->getRotationAndFlipImg(imgOverRaw, m_IsSLB);

	int binNum = IQMetricUtl::instance()->getBinNum(imgAuto.size());
	double pix2deg = IQMetricUtl::instance()->getPix2Degree(imgAuto.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	vector<double>xsecVec = IQMetricsParameters::xsecVec;
	double rotationAngle = IQMetricsParameters::flareRotationAngle;
	double presetFlarePeakDists = 1;     // 预设峰值距离
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	imgAuto = GetROIMat(imgAuto, ROIRect);
	imgOver = GetROIMat(imgOver, ROIRect);
	imgAuto = getFlareRotationImg(imgAuto, rotationAngle);
	imgOver = getFlareRotationImg(imgOver, rotationAngle);

	cv::Mat img8 = convertToUint8(imgAuto);
	cv::Mat imgdraw = convertTo3Channels(img8);
	vector<cv::Rect> rectSort= detectCenterFlareROI(img8);
	cv::Mat imgHDR = calculateHDRImage(imgAuto,imgOver,rectSort);

	imgHDR.convertTo(imgHDR, CV_32FC1);
	double maxV;
	cv::minMaxLoc(imgHDR, NULL, &maxV);
	imgHDR = imgHDR / maxV * 100;   // 整张图归一化
	map<string, vector<FlareSecRe>>flareMap;  // 定义一个 map，存每个 ROI 的分析结果
	for (int i = 0; i < rectSort.size(); i++)
	{
		cv::Rect rect0 = rectSort[i];
		drawRectOnImage(imgdraw, rect0);
		putTextOnImage(imgdraw, to_string(i + 1), rect0.tl());
		cv::Point2f center;
		center.x = rect0.x + rect0.width / 2;
		center.y = rect0.y + rect0.height / 2;
		circle(imgdraw, center, 5, Scalar(0, 0, 255), -1);
		cv::Rect rectRatio=updateRectByRatio(rect0, 0.5); // 将矩形按比例缩小，得到更中心区域
		drawRectOnImage(imgdraw, rectRatio, 4 / binNum);
		cv::Mat dispVal = imgHDR(rectRatio);
		double dispMedian = calculateMatMedian(dispVal); // 计算中位数亮度
		imgHDR = imgHDR / dispMedian * 100;
		vector<FlareSecRe>flareDist;  // 数组 存 ROI 在不同截线方向/位置下的结果
		for (int i = 0; i < xsecVec.size(); i++)
		{	
			XSECRe xsec=straightXsec(imgHDR, pix2deg,center, xsecVec[i],imgdraw);
			vector<double>flarePeaksL, flarePeaksR;
			double deg_offset = presetFlarePeakDists / pix2deg;  //预设角度距离转成像素距离
			flarePeaksL.push_back(xsec.avg_xsec_left.total() / 2 - deg_offset);  
			flarePeaksL.push_back(xsec.avg_xsec_left.total() / 2 + deg_offset);  // 左侧截线的中点为中心，取一个左右对称的范围
			flarePeaksR.push_back(xsec.avg_xsec_right.total() / 2 - deg_offset);
			flarePeaksR.push_back(xsec.avg_xsec_right.total() / 2 + deg_offset);
			double baselineBuffer = 0.2;
			int baselineBufferPix = baselineBuffer / pix2deg;			
			StrightXsecRe reL=processStrightXsec(xsec.avg_xsec_left,flarePeaksL,baselineBuffer);
			StrightXsecRe reR = processStrightXsec(xsec.avg_xsec_right, flarePeaksR, baselineBuffer);
			FlareSecRe flare0;
			flare0.flareMedianL = reL.flareMedian;  //左 flare 中位数
			flare0.glowMedianL = reL.glowMedian;
			flare0.flareMedianR = reR.flareMedian;
			flare0.glowMedianR = reR.glowMedian;
			flareDist.push_back(flare0);
		}	
		flareMap.insert(std::make_pair(to_string(i), flareDist));
	}
	re.flareMap = flareMap;
	re.imgdraw = imgdraw;
	return re;
}

FlareRe MLIQMetrics::MLFlare::getFlareNew(const cv::Mat imgAutoRaw, const cv::Mat imgOverRaw)
{	
		string info = "-------getFlare------";
		FlareRe re;
		if (imgAutoRaw.empty() || imgOverRaw.empty())
		{
			re.flag = false;
			re.errMsg = info + "The input image is null!";
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
			return re;
		}
		cv::Mat imgAuto = IQMetricUtl::instance()->getRotationAndFlipImg(imgAutoRaw, m_IsSLB);  //统一图片方向
		cv::Mat imgOver = IQMetricUtl::instance()->getRotationAndFlipImg(imgOverRaw, m_IsSLB);
		int binNum = IQMetricUtl::instance()->getBinNum(imgAutoRaw.size());  // 缩放
		double pix2deg = IQMetricUtl::instance()->getPix2Degree(imgAutoRaw.size());
		cv::Rect ROIRect = IQMetricsParameters::ROIRect;
		vector<double>xsecVec = IQMetricsParameters::xsecVec;
		double rotationAngle = IQMetricsParameters::flareRotationAngle; // 0
		double presetFlarePeakDists = 1;
		updateRectByRatio1(ROIRect, 1.0 / binNum);  // 缩放roi
	    imgAuto = GetROIMat(imgAuto, ROIRect);
		imgOver = GetROIMat(imgOver, ROIRect); // 确定ROI在图像中的区域
		imgAuto = getFlareRotationImg(imgAuto, rotationAngle);
		imgOver = getFlareRotationImg(imgOver, rotationAngle);
		cv::Mat img8 = convertToUint8(imgAuto);
		cv::Mat imgdraw = convertTo3Channels(img8);
		vector<cv::Rect> rectSort = detectCenterFlareROI(img8);  // roi内的contour
		if (rectSort.size() == 1)
		{
			writeFlareInfo(rectSort);
		}
		else
		{
			rectSort.clear();
			readFlareInfo(rectSort);
		}

		cv::Mat imgHDR = calculateHDRImage(imgAuto, imgOver, rectSort);
		imgHDR.convertTo(imgHDR, CV_32FC1);
		double maxV;
		cv::minMaxLoc(imgHDR, NULL, &maxV); // 返回矩阵中的最大值
		imgHDR = imgHDR / maxV * 100; 
		map<string, vector<FlareSecRe>>flareMap;
		for (int i = 0; i < rectSort.size(); i++)
		{
			cv::Rect rect0 = rectSort[i];
			drawRectOnImage(imgdraw, rect0);
			putTextOnImage(imgdraw, to_string(i + 1), rect0.tl());
			cv::Point2f center;
			center.x = rect0.x + rect0.width / 2;
			center.y = rect0.y + rect0.height / 2;
			circle(imgdraw, center, 5, Scalar(0, 0, 255), -1);
			cv::Rect rectRatio = updateRectByRatio(rect0, 0.5);   //rect0 缩小0.5
			drawRectOnImage(imgdraw, rectRatio, 4 / binNum);
			cv::Mat dispVal = imgHDR(rectRatio);
			double dispMedian = calculateMatMedian(dispVal);  //计算缩小0.5边界内的中值亮度
			imgHDR = imgHDR / dispMedian * 100;
			vector<FlareSecRe>flareDist;
			for (int i = 0; i < xsecVec.size(); i++)
			{
				XSECRe xsec = straightXsec(imgHDR, pix2deg, center, xsecVec[i], imgdraw);
				vector<double>flarePeaksL, flarePeaksR;
				double deg_offset = presetFlarePeakDists / pix2deg;
				flarePeaksL.push_back(xsec.avg_xsec_left.total() / 2 - deg_offset);
				flarePeaksL.push_back(xsec.avg_xsec_left.total() / 2 + deg_offset);
				flarePeaksR.push_back(xsec.avg_xsec_right.total() / 2 - deg_offset);
				flarePeaksR.push_back(xsec.avg_xsec_right.total() / 2 + deg_offset);
				double baselineBuffer = 0.2;
				int baselineBufferPix = baselineBuffer / pix2deg;
				StrightXsecRe reL = processStrightXsec(xsec.avg_xsec_left, flarePeaksL, baselineBuffer);
				StrightXsecRe reR = processStrightXsec(xsec.avg_xsec_right, flarePeaksR, baselineBuffer);
				FlareSecRe flare0;
				flare0.flareMedianL = reL.flareMedian;
				flare0.glowMedianL = reL.glowMedian;
				flare0.flareMedianR = reR.flareMedian;
				flare0.glowMedianR = reR.glowMedian;
				flareDist.push_back(flare0);
			}
			flareMap.insert(std::make_pair(to_string(i), flareDist));
		}
		re.flareMap = flareMap;
		re.imgdraw = imgdraw;
		return re;
	

}

vector<cv::Rect> MLIQMetrics::MLFlare::detectFlareROI(cv::Mat img)
{
	cv::Mat imgth;
	cv::threshold(img, imgth, 0, 255, THRESH_TRIANGLE);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(imgth, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	vector<cv::Rect>rectVec;
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Rect rect = cv::boundingRect(contours[i]);
		double area = cv::contourArea(contours[i]);
		double w = max(rect.width, rect.height);
		double h = min(rect.width, rect.height);
		double ratio = h / w;
		if (ratio > 0.8 && area > 1e4)
		{
			rectVec.push_back(rect);
		}
	}
	vector<cv::Rect>rectSort=getSortedRect(rectVec);
	return rectSort;
}

vector<cv::Rect> MLIQMetrics::MLFlare::detectCenterFlareROI(cv::Mat img)
{
	
	cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, cv::Size(5, 5));
	//cv::morphologyEx(img, img, MORPH_GRADIENT, kernel);
	cv::Mat imgth,imgth1;
	cv::threshold(img, imgth, 0, 255, THRESH_TRIANGLE);
	//cv::threshold(img, imgth, 0, 255, THRESH_OTSU);
	cv::Mat kernel1 = cv::getStructuringElement(MORPH_RECT, cv::Size(40, 40));
	cv::morphologyEx(imgth, imgth, MORPH_CLOSE, kernel1);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(imgth, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	vector<cv::Rect>rectVec;
	cv::Rect rect0;
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Rect rect = cv::boundingRect(contours[i]);
		double area = cv::contourArea(contours[i]);
		double w = max(rect.width, rect.height);
		double h = min(rect.width, rect.height);
		double ratio = h / w;  // 根据外接矩形的长宽比筛选
		if (ratio > 0.8 && rect.area() > 1e4&&rect.area()<2.3e4&&area>1e4)
		{
			rectVec.push_back(rect);
			rect0 = rect;
		}
	}
	return rectVec;
}

vector<cv::Rect> MLIQMetrics::MLFlare::detectCenterFlareROI(cv::Mat img, cv::Rect rect0)
{
	cv::Mat roi = img(rect0).clone();
	cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, cv::Size(5, 5));
	//cv::morphologyEx(img, img, MORPH_GRADIENT, kernel);
	cv::Mat imgth, imgth1;
	cv::threshold(roi, imgth, 0, 255, THRESH_TRIANGLE);
	//cv::threshold(img, imgth, 0, 255, THRESH_OTSU);
	cv::Mat kernel1 = cv::getStructuringElement(MORPH_RECT, cv::Size(40, 40));
	cv::morphologyEx(imgth, imgth, MORPH_CLOSE, kernel1);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(imgth, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	vector<cv::Rect>rectVec;
	//cv::Rect rect0;
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Rect rect = cv::boundingRect(contours[i]);
		double area = cv::contourArea(contours[i]);
		double w = max(rect.width, rect.height);
		double h = min(rect.width, rect.height);
		double ratio = h / w;
		if (ratio > 0.8 && rect.area() > 1e4 && rect.area() < 2.3e4 && area > 1e4)
		{
			rectVec.push_back(rect+rect0.tl());
		}
	}
	return rectVec;
}

vector<cv::Rect> MLIQMetrics::MLFlare::getSortedRect(vector<cv::Rect> rectVec)
{
	vector<cv::Rect> rectVecSort;
	arma::vec xvec(rectVec.size()),yvec(rectVec.size());
	for (int i = 0; i < rectVec.size(); i++)
	{
		xvec[i] = rectVec[i].x;
		yvec[i] = rectVec[i].y;
	}
	arma::uvec yindex = arma::sort_index(yvec);
	arma::vec xvecT = xvec(yindex.subvec(0, 1));
	if (xvecT[0] < xvecT[1])
	{
		rectVecSort.push_back(rectVec[yindex[0]]);
		rectVecSort.push_back(rectVec[yindex[1]]);

	}
	else
	{
		rectVecSort.push_back(rectVec[yindex[1]]);
		rectVecSort.push_back(rectVec[yindex[0]]);
	}

	rectVecSort.push_back(rectVec[yindex[2]]);
	arma::vec xvecB = xvec(yindex.subvec(3, 4));
	if (xvecB[0] < xvecB[1])
	{
		rectVecSort.push_back(rectVec[yindex[3]]);
		rectVecSort.push_back(rectVec[yindex[4]]);
	}
	else
	{
		rectVecSort.push_back(rectVec[yindex[4]]);
		rectVecSort.push_back(rectVec[yindex[3]]);
	}
	return rectVecSort;
}

XSECRe MLIQMetrics::MLFlare::straightXsec(cv::Mat imgOver, double pix2deg, cv::Point2f center, double xSecDist, cv::Mat& imgdraw)
{
	double xSecLen = IQMetricsParameters::xsecWidth;
	int xSecLenPix = int(xSecLen/pix2deg);
	int xSecDistPix = int(xSecDist / pix2deg);
	cv::Mat  avg_xsec_left(cv::Size(1,xSecLenPix * 2), CV_32FC1, Scalar(0));
	cv::Mat  avg_xsec_right(cv::Size(1,xSecLenPix * 2), CV_32FC1, Scalar(0));
	int avgPixWidth = IQMetricsParameters::avg_width;
	double flare_angle = IQMetricsParameters::flareAngle;
	double centerX = center.x;
	int centerY = center.y;
	vector<double>xs_left, ys_left, xs_right, ys_right;
	cv::RNG rng;
	Scalar color(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
	cout << color << endl;
	for (int i = -avgPixWidth; i <= avgPixWidth; i++)
	{
		int xSecCenterX = int(centerX + (xSecDistPix + i) * cos(deg2rad(-180 - flare_angle)));
		int xSecCenterY = int(centerY + (xSecDistPix + i) * sin(deg2rad(-180 - flare_angle)));
		cv::Mat xSec = imgOver(cv::Range(xSecCenterY - xSecLenPix, xSecCenterY + xSecLenPix), cv::Range(xSecCenterX, xSecCenterX + 1));
		avg_xsec_left = avg_xsec_left + xSec;
		cv::Point2f top(xSecCenterX, xSecCenterY - xSecLenPix);
		cv::Point2f bottom(xSecCenterX, xSecCenterY + xSecLenPix);
		cv::line(imgdraw, top, bottom, color, 1);
		if (i == 0)
		{
			xs_left.push_back(xSecCenterX);
			xs_left.push_back(xSecCenterX);
			ys_left.push_back(xSecCenterY - xSecLenPix);
			ys_left.push_back(xSecCenterY +xSecLenPix);
		}

		 xSecCenterX = int(centerX + (xSecDistPix + i) * cos(deg2rad(- flare_angle)));
		 xSecCenterY = int(centerY + (xSecDistPix + i) * sin(deg2rad( - flare_angle)));
		xSec = imgOver(cv::Range(xSecCenterY - xSecLenPix, xSecCenterY + xSecLenPix), cv::Range(xSecCenterX, xSecCenterX + 1));
		avg_xsec_right = avg_xsec_left + xSec;
		
		 top=cv::Point(xSecCenterX, xSecCenterY - xSecLenPix);
		 bottom = cv::Point(xSecCenterX, xSecCenterY + xSecLenPix);
		cv::line(imgdraw, top, bottom, color, 1);
		if (i == 0)
		{
			xs_right.push_back(xSecCenterX);
			xs_right.push_back(xSecCenterX);
			ys_right.push_back(xSecCenterY - xSecLenPix);
			ys_right.push_back(xSecCenterY + xSecLenPix);
		}
	}
	avg_xsec_left = avg_xsec_left / (2 * avgPixWidth + 1);
	avg_xsec_right = avg_xsec_right / (2 * avgPixWidth + 1);
	XSECRe re;
	re.xs_left = xs_left;
	re.xs_right = xs_right;
	re.ys_left = ys_left;
	re.ys_right = ys_right;
	re.avg_xsec_left = avg_xsec_left;
	re.avg_xsec_right = avg_xsec_right;
	return re;
}
arma::vec median_filter(const arma::vec& signal, int kernel_size = 11) {
	int half_k = kernel_size / 2;
	arma::vec filtered(signal.n_elem);
	for (size_t i = 0; i < signal.n_elem; ++i) {
		int start = std::max<int>(0, i - half_k);
		int end = std::min<int>(signal.n_elem - 1, i + half_k);
		filtered(i) = arma::median(signal.subvec(start, end));
	}
	return filtered;
}
StrightXsecRe MLIQMetrics::MLFlare::processStrightXsec(cv::Mat xSec, vector<double>flarePeaks, int baselineBuffer)
{
	StrightXsecRe re;
	arma::vec xSecVec = openCVMatToArmaVec(xSec);
	//cv::Mat xSec_filt;
	//cv::medianBlur(xSec, xSec_filt, 11);
	arma::vec xSec_filt = median_filter(xSecVec, 11);
	arma::vec glowVals = arma::join_vert(
		xSecVec.subvec(0, flarePeaks[0] - 1),
		xSecVec.subvec(flarePeaks[1], xSecVec.n_elem - 1));
	double glowMedian = arma::median(glowVals);

	arma::vec glowValsBaseline = arma::join_vert(
		xSecVec.subvec(0, flarePeaks[0] - baselineBuffer - 1),
		xSecVec.subvec(flarePeaks[1] + baselineBuffer, xSecVec.n_elem - 1));
	arma::vec glow_xs = arma::join_vert(
		arma::regspace<arma::vec>(0, flarePeaks[0] - baselineBuffer),
		arma::regspace<arma::vec>(flarePeaks[1] + baselineBuffer, xSecVec.n_elem - 1)
	);
	arma::vec coeffs = arma::polyfit(glow_xs, glowValsBaseline, 4);
	arma::vec xs = arma::regspace<arma::vec>(0, xSecVec.n_elem - 1);
	arma::vec glowBaseline = arma::polyval(coeffs, xs);
	arma::vec xSec_sub = xSecVec - glowBaseline;
	arma::vec flareIntensity = xSec_sub.subvec(flarePeaks[0], flarePeaks[1] - 1);
	double flareMedian = arma::median(flareIntensity);
	double flareGlowRatio = flareMedian / glowMedian;
	re.flareMedian = flareMedian;
	re.glowMedian = glowMedian;
	return re;
}

cv::Mat MLIQMetrics::MLFlare::calculateHDRImage(cv::Mat imgAuto, cv::Mat imgOver, vector<cv::Rect> rectVec)
{
	cv::Mat imgHDR = imgOver.clone();
	for (int i = 0; i < rectVec.size(); i++)
	{
		imgAuto(rectVec[i]).copyTo(imgHDR(rectVec[i]));
	}
	return imgHDR;
}

cv::Mat MLIQMetrics::MLFlare::getFlareRotationImg(cv::Mat img, double angle)
{
	if (angle == 0)
		return img;
	cv::Mat rotatedImg;
	cv::Point2f center((float)(img.cols / 2), (float)(img.rows / 2));  
	cv::Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);  // 旋转矩阵，定义如何围绕某个中心旋转
	warpAffine(img, rotatedImg, affine_matrix,cv::Size(img.rows,img.cols));  // 实际旋转图像
	return rotatedImg;
}

void MLIQMetrics::MLFlare::writeFlareInfo(vector<cv::Rect> rectVec)
{
	string filepath = "./config/AlgConfig/slbInfo/FlareInfo.csv";
	cv::Mat rectmat(cv::Size(4,rectVec.size()),CV_32FC1);
	for (int i = 0; i < rectVec.size(); i++)
	{
		rectmat.at<float>(i, 0) = rectVec[i].x;
		rectmat.at<float>(i, 1) = rectVec[i].y;
		rectmat.at<float>(i, 2) = rectVec[i].width;
		rectmat.at<float>(i, 3) = rectVec[i].height;
	}
	std::mutex mtx;
	mtx.lock();
	writeMatTOCSV(filepath, rectmat);
	mtx.unlock();
}

void MLIQMetrics::MLFlare::readFlareInfo(vector<cv::Rect>& rectVec)
{
	string filepath = "./config/AlgConfig/slbInfo/FlareInfo.csv";
	cv::Mat rectmat = readCSVToMat(filepath);
	for (int i = 0; i < rectmat.rows; i++)
	{
		cv::Rect rect;
		rect.x = rectmat.at<float>(i, 0);
		rect.y = rectmat.at<float>(i, 1);
		rect.width = rectmat.at<float>(i, 2);
		rect.height = rectmat.at<float>(i, 3);
		rectVec.push_back(rect);
	}

}

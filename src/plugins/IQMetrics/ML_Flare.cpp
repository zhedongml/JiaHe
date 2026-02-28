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

FlareRe MLIQMetrics::MLFlare::getFlare(const cv::Mat imgHDR)
{

	string info = "-------getFlare------";
	FlareRe re;
	if (imgHDR.empty())
	{
		re.flag = false;
		re.errMsg = info + "The input image is null!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(imgHDR.size());
	double pix2deg = IQMetricUtl::instance()->getPix2Degree(imgHDR.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	vector<double>xsecVec = IQMetricsParameters::xsecVec;
	double presetFlarePeakDists = 1;
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	cv::Mat imgHDRROI = GetROIMat(imgHDR, ROIRect);
	cv::Mat img8 = convertToUint8(imgHDRROI);
	cv::Mat imgdraw = convertTo3Channels(imgHDRROI);
	vector<cv::Rect> rectSort = detectCenterFlareROI(img8);

	imgHDRROI.convertTo(imgHDRROI, CV_32FC1);
	double maxV;
	cv::minMaxLoc(imgHDRROI, NULL, &maxV);
	imgHDRROI = imgHDRROI / maxV * 100;
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
		cv::Rect rectRatio = updateRectByRatio(rect0, 0.5);
		drawRectOnImage(imgdraw, rectRatio, 4 / binNum);
		cv::Mat dispVal = imgHDRROI(rectRatio);
		double dispMedian = calculateMatMedian(dispVal);
		imgHDRROI = imgHDRROI / dispMedian * 100;
		vector<FlareSecRe>flareDist;
		for (int i = 0; i < xsecVec.size(); i++)
		{
			XSECRe xsec = straightXsec(imgHDRROI, pix2deg, center, xsecVec[i], imgdraw);
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

FlareRe MLIQMetrics::MLFlare::getFlare(cv::Mat imgAuto, cv::Mat imgOver)
{
	string info = "-------getFlare------";
	FlareRe re;
	if (imgAuto.empty() || imgOver.empty())
	{
		re.flag = false;
		re.errMsg = info + "The input image is null!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(imgAuto.size());
	double pix2deg = IQMetricUtl::instance()->getPix2Degree(imgAuto.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	vector<double>xsecVec = IQMetricsParameters::xsecVec;
	double presetFlarePeakDists = 1;
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	imgAuto = GetROIMat(imgAuto, ROIRect);
	imgOver = GetROIMat(imgOver, ROIRect);
	cv::Mat img8 = convertToUint8(imgAuto);
	cv::Mat imgdraw = convertTo3Channels(imgAuto);
	vector<cv::Rect> rectSort = detectCenterFlareROI(img8);
	cv::Mat imgHDR = calculateHDRImage(imgAuto, imgOver, rectSort);

	imgHDR.convertTo(imgHDR, CV_32FC1);
	double maxV;
	cv::minMaxLoc(imgHDR, NULL, &maxV);
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
		cv::Rect rectRatio = updateRectByRatio(rect0, 0.5);
		drawRectOnImage(imgdraw, rectRatio, 4 / binNum);
		cv::Mat dispVal = imgHDR(rectRatio);
		double dispMedian = calculateMatMedian(dispVal);
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
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "flare calculation successfully");
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
	vector<cv::Rect>rectSort = getSortedRect(rectVec);
	return rectSort;
}

vector<cv::Rect> MLIQMetrics::MLFlare::detectCenterFlareROI(cv::Mat img)
{
	cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, cv::Size(20, 20));
	cv::morphologyEx(img, img, MORPH_CLOSE, kernel);
	cv::Mat imgth;
	cv::threshold(img, imgth, 0, 255, THRESH_OTSU);
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
		double ratio = h / w;
		if (ratio > 0.75 && area > 1e4)
		{
			rectVec.push_back(rect);
			rect0 = rect;
		}
	}
	return rectVec;
}

vector<cv::Rect> MLIQMetrics::MLFlare::getSortedRect(vector<cv::Rect> rectVec)
{
	vector<cv::Rect> rectVecSort;
	arma::vec xvec(rectVec.size()), yvec(rectVec.size());
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
	int xSecLenPix = int(xSecLen / pix2deg);
	int xSecDistPix = int(xSecDist / pix2deg);
	cv::Mat  avg_xsec_left(cv::Size(1, xSecLenPix * 2), CV_32FC1, Scalar(0));
	cv::Mat  avg_xsec_right(cv::Size(1, xSecLenPix * 2), CV_32FC1, Scalar(0));
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
			ys_left.push_back(xSecCenterY + xSecLenPix);
		}

		xSecCenterX = int(centerX + (xSecDistPix + i) * cos(deg2rad(-flare_angle)));
		xSecCenterY = int(centerY + (xSecDistPix + i) * sin(deg2rad(-flare_angle)));
		xSec = imgOver(cv::Range(xSecCenterY - xSecLenPix, xSecCenterY + xSecLenPix), cv::Range(xSecCenterX, xSecCenterX + 1));
		avg_xsec_right = avg_xsec_left + xSec;

		top = cv::Point(xSecCenterX, xSecCenterY - xSecLenPix);
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

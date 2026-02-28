#include "pch.h"
#include "ML_Ghost.h"
#include"MLGhostRectangleDetect.h"
#include<armadillo>
using namespace MLIQMetrics;
using namespace MLImageDetection;
using namespace cv;



MLGhost::MLGhost()
{
}



MLGhost::~MLGhost()
{
}

cv::Rect MLIQMetrics::MLGhost::findSquareRect(cv::Mat roi)
{

	cv::Mat roi8 = convertToUint8(roi);
	cv::Mat roidraw = convertTo3Channels(roi8);
	//cv::Mat sx, sy;
	//cv::Sobel(roi, sx, CV_32FC1, 1, 0,7);
	//cv::Sobel(roi, sy, CV_32FC1, 0, 1,7);
	//cv::reduce(sx, sx, 0, REDUCE_AVG);
	//cv::reduce(sy, sy, 1, REDUCE_AVG);
	//cv::rotate(sy, sy, ROTATE_90_COUNTERCLOCKWISE);
	cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(roi8, roi8, MORPH_OPEN, kernel);
	//cv::morphologyEx(sy, sy, MORPH_GRADIENT, kernel);
	//double maxV, maxV1, maxV2;
	//cv::minMaxLoc(sx, NULL, &maxV);
	//cv::minMaxLoc(sy, NULL, &maxV1);
	//vector<int> xVec = findPeaks(sx, maxV / 3.0, 0, 0);
	//vector<int> yVec1 = findPeaks(sy, maxV1 / 3.0, 0, 0);
	cv::Mat roith;
	cv::threshold(roi8, roith, 0, 255, THRESH_OTSU);
	cv::morphologyEx(roith, roith, MORPH_OPEN, kernel);
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	findContours(roith, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	cv::Point2f cen;
	cv::RotatedRect rectR;
	int len;
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Rect rect = cv::boundingRect(contours[i]);
		if (rect.area() > 4e4 && rect.area() < 36e4)
		{
			//drawRectOnImage(roidraw, rect);
			 rectR = cv::minAreaRect(contours[i]);
			cen = rectR.center;
			double area = cv::contourArea(contours[i]);
			len = sqrt(area);
			cv::Point2f P[4];
			rectR.points(P);

			cv::line(roidraw, P[0], P[1], Scalar(0, 0, 255), 1);
			cv::line(roidraw, P[1], P[2], Scalar(0, 0, 255), 1);
			cv::line(roidraw, P[2], P[3], Scalar(0, 0, 255), 1);
			cv::line(roidraw, P[0], P[3], Scalar(0, 0, 255), 1);
		}
	}
	cv::Mat sy = roi8(cv::Range(0, roi8.rows), cv::Range(cen.y - 5, cen.y + 5));
	cv::Mat sx = roi8(cv::Range(cen.x - 5, cen.x + 5), cv::Range(0, roi8.cols));
	cv::reduce(sx, sx, 0, REDUCE_AVG);
	cv::reduce(sy, sy, 1, REDUCE_AVG);
	//	cv::rotate(sy, sy, ROTATE_90_COUNTERCLOCKWISE);
	//findEdgePts1(sx, 0, h1, h2);
	//findEdgePts1(sy, 1, v1, v2);	
	cv::Rect rect;
	rect.x = rectR.center.x-len/2;
	rect.y = rectR.center.y - len / 2;
	rect.width = len;
	rect.height =len;
	cv::Rect rect0 = updateRectByRatio(rect, 0.5);
	cv::rectangle(roidraw, rect, Scalar(0, 255, 0), 1);
	cv::rectangle(roidraw, rect0, Scalar(0, 255, 0), 1);

	return rect0;
}



GhostRe MLIQMetrics::MLGhost::getGhostSquares(cv::Mat img)
{
	string info = "------getGhostSquares------";
	GhostRe re;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL";
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	int pix2deg = IQMetricUtl::instance()->getPix2Degree(img.size());
	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);
	vector<cv::Rect> rectsDisplay = findDisplayRect(img8, imgdraw);
	vector<cv::Rect>rectGhost=IQMetricsParameters::ghostRectVec;
	if (rectsDisplay.size() != rectGhost.size())
	{
		re.flag = false;
		re.errMsg = info + "ghost display rect size is not equal to ghost size";
		return re;
	}

	for (int i = 0; i < rectGhost.size(); i++)
	{
		SuqreGhostRe ghost = getSquareGhost(img, rectsDisplay[i], rectGhost[i], imgdraw, i);
		re.ghostMap.insert(std::make_pair(to_string(i + 1), ghost));
	}
	re.imgdraw = imgdraw;
	return re;
}
vector<int> MLIQMetrics::MLGhost::findPeaks(cv::Mat data, double minHeight, int minWidth, double minProminence)
{

	vector<int> xVec;
	// peaks.clear(); // 清空峰值数组
	data.convertTo(data, CV_64FC1);
	int thresh = 1;
	for (int i = thresh; i < data.cols - thresh; ++i)
	{
		// 判断是否为峰值，即比相邻的两个点大，并且大于设定的最小高度
		// for (int i0 = i - thresh;  i0 < i + thresh; i0++)
		//{
		//        if (data.at<double>(0, i) > data.at<double>(0, i0));
		//        break;
		//}
		if (data.at<double>(0, i) > minHeight &&
			(data.at<double>(0, i) > data.at<double>(0, i - 1) && data.at<double>(0, i) >= data.at<double>(0, i + 1)))
		{
			// 寻找峰值的宽度，即峰值到其两侧的极小值点的距离
			int left = i - thresh;
			while (left >= 0 && (data.at<double>(0, left) <= data.at<double>(0, left + thresh)))
			{
				--left;
			}

			int right = i + thresh;
			while (right < data.cols && data.at<double>(0, right) <= data.at<double>(0, right - thresh))
			{
				++right;
			}

			// 计算峰值的宽度和突出度
			int width = right - left - 2;
			double prominence =
				data.at<double>(0, i) - std::max(data.at<double>(0, left + 1), data.at<double>(0, right - 1));

			// 判断峰值的宽度和突出度是否满足设定的条件
			if (width >= minWidth && prominence > minProminence)
			{
				if (xVec.size() > 0)
				{
					int sub = i - xVec[xVec.size() - 1];
					if (sub > 100)
					{
						// peaks.push_back({i, data.at<double>(0, i), width, prominence});
						xVec.push_back(i);
					}
				}
				else
				{
					// peaks.push_back({i, data.at<double>(0, i), width, prominence});
					xVec.push_back(i);
				}
			}
		}
	}
	return xVec;
}
SuqreGhostRe MLIQMetrics::MLGhost::getSquareGhost(cv::Mat img, cv::Rect rectDisplay, cv::Rect rectGhost, cv::Mat& imgdaw, int i)
{
	SuqreGhostRe re;
	cv::Mat ghostROI = img(rectGhost).clone();
	cv::Rect rectghost = findSquareRect(ghostROI);
	rectghost = rectghost + rectGhost.tl();
	cv::rectangle(imgdaw, rectghost, cv::Scalar(0, 255, 0), 5);
	cv::Mat ghostMat = img(rectghost).clone();
	cv::Mat displayMat = img(rectDisplay).clone();
	getGhostValidData(ghostMat, re.p95Ghost, re.medianGhost);
	getGhostValidData(displayMat, re.p95Display, re.medianDisplay);
	re.ratioMedian = re.medianGhost / re.medianDisplay * 100;
	re.ratioP95 = re.p95Ghost / re.p95Display * 100;
	re.displayRect = rectDisplay;
	re.ghostRect = rectGhost;

	string str1 = "median:" + to_string(re.medianDisplay);
	string str2 = "p95:" + to_string(re.p95Display);
	putTextOnImage(imgdaw, to_string(i + 1), rectDisplay.tl() - cv::Point(0, 100), 10);
	putTextOnImage(imgdaw, str1, rectDisplay.tl());
	putTextOnImage(imgdaw, str2, rectDisplay.tl() + cv::Point(0, 200));
	str1 = "medianRatio(%):" + to_string(re.ratioMedian);
	str2 = "p95Ratio(%):" + to_string(re.ratioP95);
	putTextOnImage(imgdaw, str1, rectDisplay.tl() + cv::Point(0, 400));
	putTextOnImage(imgdaw, str2, rectDisplay.tl() + cv::Point(0, 600));
	string str3 = "median:" + to_string(re.medianGhost);
	string str4 = "p95:" + to_string(re.p95Ghost);
	putTextOnImage(imgdaw, str3, rectghost.tl());
	putTextOnImage(imgdaw, str4, rectghost.tl() + cv::Point(0, 200));
	putTextOnImage(imgdaw, to_string(i + 1), rectghost.tl() - cv::Point(0, 100), 10);

	return re;
}
GhostRe MLGhost::getGhost(cv::Mat img, ROIParaNew roiInfo)
{
	string info = "------getGhost------";
	GhostRe re;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "the input image is null";
		return re;
	}
	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);
	cv::Rect rect = IQMetricUtl::instance()->getRect(roiInfo, IQMetricsParameters::opticalCenter);
	GhostRectangleDetect rectdec;
	GhostRectRe rectRe = rectdec.getGhostRectangleRect(img, rect);
	vector<cv::Rect>rectVec;
	vector<cv::Rect>ghostRectVec;
	vector<double>lumiSquareVec;
	vector<double>lumiGhostVec;
	vector<double>ratioVec;
	for (int i = 0; i < rectRe.rectVec.size(); i++)
	{
		cv::Rect rectSquare = updateRectByRatio(rectRe.rectVec[i], 0.5);
		cv::Rect rectGhost = updateRectByRatio(rectRe.rectVec[i], 0.5);
		rectVec.push_back(updateRectByRatio(rectRe.rectVec[i], 0.5));
		ghostRectVec.push_back(updateRectByRatio(rectRe.ghostRectVec[i], 0.5));
		double lumiSquare = cv::mean(img(rectSquare))(0);
		double lumiGhost = cv::mean(img(rectGhost))(0);
		double ratio = lumiGhost / lumiSquare * 100;
		lumiSquareVec.push_back(lumiSquare);
		lumiGhostVec.push_back(lumiGhost);
		ratioVec.push_back(ratio);

		//draw
		cv::rectangle(imgdraw, rectSquare, Scalar(0, 0, 255), 2);
		cv::rectangle(imgdraw, rectGhost, Scalar(0, 255, 0), 2);
		putTextOnImage(imgdraw, numToString(lumiSquare), rectSquare.tl());
		putTextOnImage(imgdraw, numToString(lumiGhost), rectGhost.tl());
	}
	//re.ghostRectVec = ghostRectVec;
	//re.rectVec = rectVec;
	//re.ghostlumi = lumiGhostVec;
	//re.squrelumi = lumiSquareVec;
	//re.ratioVec = ratioVec;
	re.imgdraw = imgdraw;
	return re;
}
void MLIQMetrics::MLGhost::getGhostValidData(cv::Mat roi, double& p95, double& median)
{
	roi.convertTo(roi, CV_64FC1);
	arma::mat data(reinterpret_cast<double*>(roi.data), roi.cols, roi.rows);
	arma::vec datavec = arma::vectorise(data);
	p95 = percentile(datavec, 95);
	median = arma::median(datavec);
}
vector<cv::Rect>MLGhost::findDisplayRect(cv::Mat img8, cv::Mat& imgdraw)
{
	cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, cv::Size(10, 10));
	cv::morphologyEx(img8, img8, MORPH_CLOSE, kernel);
	cv::Mat imgth,imgth1;
	cv::threshold(img8, imgth, 0, 255, THRESH_OTSU);
	cv::threshold(img8, imgth1, 0, 255, THRESH_TRIANGLE);
	imgdraw = convertTo3Channels(imgth1);
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	findContours(imgth, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//arma::vec xvec, yvec;
	vector<cv::Point2f>cenVec;
	vector<cv::Rect>rectVec;
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Rect rect = cv::boundingRect(contours[i]);
		cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
		if (rect.area() > 9e3 && rect.area() < 36e4)
		{
			//drawRectOnImage(imgdraw, rect);
			cv::Rect rect0 = updateRectByRatio(rect, 0.5);
			drawRectOnImage(imgdraw, rect0);
			rectVec.push_back(rect0);
			cenVec.push_back(rectR.center);
		}
	}
	vector<cv::Rect>rectSort = sortRects(rectVec, cenVec);
	return rectSort;
}
bool MLIQMetrics::MLGhost::findEdgePts1(cv::Mat data, int flag, cv::Point2f& p1, cv::Point2f& p2)
{

	if (flag == 1)
		cv::rotate(data, data, ROTATE_90_COUNTERCLOCKWISE);
	cv::Mat gx, gy;
	cv::Mat roi1 = data(cv::Range(0, 1), cv::Range((p1.x) - 40, p1.x + 40));
	cv::Mat roi2 = data(cv::Range(0, 1), cv::Range((p2.x) - 40, p2.x + 40));
	cv::Mat gx1, gx2;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 1));
	cv::morphologyEx(data, gx, cv::MORPH_GRADIENT, element);
	cv::morphologyEx(roi1, gx1, cv::MORPH_GRADIENT, element);
	cv::morphologyEx(roi2, gx2, cv::MORPH_GRADIENT, element);

	double maxV, maxV1, maxV2;
	cv::minMaxLoc(gx, NULL, &maxV);
	cv::minMaxLoc(gx1, NULL, &maxV1);
	cv::minMaxLoc(gx2, NULL, &maxV2);

	vector<int> xVec = findPeaks(gx, maxV / 3.0, 0, 0);
	vector<int> xVec1 = findPeaks(gx1, maxV1 / 3.0, 0, 0);
	vector<int> xVec2 = findPeaks(gx2, maxV2 / 3.0, 0, 0);
	if (xVec1.size() < 1 || xVec2.size() < 1)
		return false;

	if (flag == 0)
	{
		p1.x = xVec1[0] + (p1.x) - 40;
		p2.x = xVec2[0] + (p2.x) - 40;
	}
	else if (flag == 1)
	{
		p1.y = xVec1[0] + (p1.x) - 40;
		p2.y = xVec2[0] + (p2.x) - 40;
	}
	return true;

}
vector<cv::Rect> MLGhost::sortRects(vector<cv::Rect>rectVec, vector<cv::Point2f>cenVec)
{
	arma::vec xvec(rectVec.size());
	arma::vec yvec(rectVec.size());
	for (int i = 0; i < cenVec.size(); i++)
	{
		xvec[i] = cenVec[i].x;
		yvec[i] = cenVec[i].y;
	}
	arma::uvec indexy = arma::sort_index(yvec);
	vector<cv::Rect>rectsSort;
	for (int i = 0; i < rectVec.size(); i++)
	{
		rectsSort.push_back(rectVec[indexy[i]]);
	}
	return rectsSort;
}

#include "ml_gridDetect.h"
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include<numeric> // accumulate定义的头文件
#include<time.h>
#include"CrossCenter.h"
#include"MLCherkerboardDetect.h"
#include"LogPlus.h"
using namespace MLImageDetection;
using namespace cv;

MLImageDetection::MLGridDetect::MLGridDetect()
{
}

MLImageDetection::MLGridDetect::~MLGridDetect()
{
}

void MLImageDetection::MLGridDetect::setAccurateDetectionFlag(bool flag)
{
	accurateFlag = flag;
}

GridRe MLImageDetection::MLGridDetect::getGridCorners(cv::Mat img)
{
	string info = "------getGridCorners------";
	GridRe re;
	vector<Point2f> ptsNew;
	cv::Mat indexMap;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "input image is null";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	cv::Mat img8 = convertToUint8(img);
	//img8 = rotateGridImg(img8);
	cv::Mat img_draw = convertTo3Channels(img8);
	Mat srcgray = convertToGrayImage(img8);

	cv::GaussianBlur(srcgray, srcgray, cv::Size(3, 3), 0, 0);
	Mat srcbinary;
	threshold(srcgray, srcbinary, 0, 255, THRESH_OTSU);
	// 2、Shi-Tomasi
	vector<Point2f> corners;
	int maxcorners = 2000;
	double qualityLevel = 0.01;
	double minDistance = 50;
	int blockSize = 20;
	double k = 0.06;
	goodFeaturesToTrack(srcgray, corners, maxcorners, qualityLevel, minDistance, Mat(), blockSize, false, k);
	Size winSize = Size(5, 5);
	Size zeroZone = Size(-1, -1);
	TermCriteria criteria = TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001);
	// TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 40, 0.001);
 //   cv::cornerSubPix(srcgray, corners, winSize, zeroZone, criteria);
	if (corners.size() < 5)
	{
		re.flag = false;
		re.errMsg = info + "grid detection fail";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}

	drawPointsOnImage(img_draw, corners, 2, Scalar(0, 0, 255));
	vector<Point2f> cornersNew;
	cornersNew = pointsClusters(corners, m_pointsClusters / m_binNum);
	drawPointsOnImage(img_draw, cornersNew, 2, Scalar(0, 255, 0));

	// point filter, it's hard to find the right point
	vector<Point2f> cornersNew1;
	for (int i = 0; i < cornersNew.size(); i++)
	{
		//  cout << cornersNew[i] << endl;
		double left = cv::max(0, int(cornersNew[i].x - m_len / m_binNum));
		double right = cv::min(srcgray.rows, int(cornersNew[i].x + m_len / m_binNum));
		double top = cv::max(0, int(cornersNew[i].y - m_len / m_binNum));
		double bottom = cv::min(srcgray.cols, int(cornersNew[i].y + m_len / m_binNum));
		cv::Mat roi = srcgray(cv::Rect(left, top, right - left, bottom - top));
		double corr = corrlateMat90(roi);
		double mag = getMagnitude(roi);
		//cout << corr << " ," << mag << endl;
		if (mag > 500)
		{
			cornersNew1.push_back(cornersNew[i]);
			circle(img_draw, cornersNew[i], 5, Scalar(255, 0, 255), -1, 8, 0);
		}
	}
	vector<Point2f>pts = cornersNew1;

	if (pts.size() < 5)
	{
		re.flag = false;
		re.errMsg = info + "grid detection fail";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}


	ptsNew = pointsClusters(pts, m_pointsClusters / m_binNum);
	cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, m_update, m_xyClassification / m_binNum);
	MLCherkerboardDetect cb;
	indexMap = cb.updateIndexMapKL(indexMap1, ptsNew, corners);
	if ((indexMap.rows * indexMap.cols) > corners.size())
	{
		re.flag = false;
		re.errMsg = info + "grid detection fail";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	//draw
	cv::Mat xlocMat(indexMap.size(), CV_32FC1, Scalar(0));
	cv::Mat ylocMat(indexMap.size(), CV_32FC1, Scalar(0));

	for (int i = 0; i < indexMap.rows; i++)
	{
		for (int j = 0; j < indexMap.cols; j++)
		{
			int in = indexMap.at<short>(i, j);
			if (in < corners.size())
			{
				circle(img_draw, corners[in], 2, Scalar(255, 0, 255), -1);
				// cout << corners[in] << endl;
				xlocMat.at<float>(i, j) = corners[in].x;
				ylocMat.at<float>(i, j) = corners[in].y;

			}
		}
		//  cout << endl;
	}

	re.xLocMat = xlocMat;
	re.yLocMat = ylocMat;
	re.corners = corners;
	re.imgdraw = img_draw;
	re.indexMap = indexMap;
	re.boardSize = xlocMat.size();
	return re;
}

GridRe MLImageDetection::MLGridDetect::getGridHoughLine(cv::Mat img)
{

	string info = "------getGridHoughLine-------";
	GridRe re;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "input image is null";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	vector<cv::Point2f> corners;
	cv::Mat img8 = convertToUint8(img);
	//img8 = rotateGridImg(img8);
	cv::Mat srcgray = convertToGrayImage(img8);
	cv::Mat img_draw = convertTo3Channels(img8);
	cv::Mat srcbinary;
	cv::threshold(srcgray, srcbinary, 0, 255, CV_THRESH_OTSU);
	// cv::threshold(srcgray, srcbinary, 0, 255, CV_THRESH_TRIANGLE);
	int row = srcgray.rows;
	int col = srcgray.cols;
	Mat m;
	srcbinary.convertTo(m, CV_64FC1);
	Mat m_row(1, col, CV_64FC1, Scalar(0));
	Mat m_col(row, 1, CV_64FC1, Scalar(0));
	reduce(m, m_row, 0, CV_REDUCE_SUM);
	reduce(m, m_col, 1, CV_REDUCE_SUM);
	int numR = countNonZero(m_row);
	int numC = countNonZero(m_col);
	int l = (numR < numC ? numR : numC) / 6;
	cv::Mat srccanny;
	cv::Canny(srcbinary, srccanny, 30, 90);
	vector<Vec4i> lines;
	vector<Vec4i> lines1, lines2;
	vector<double> ks1, ks2;
	vector<double> b1, b2;
	cv::HoughLinesP(srccanny, lines, 1, CV_PI / 180, l, l, l);

	for (int i = 0; i < lines.size(); i++)
	{
		cv::Point point0;
		point0.x = lines[i][0];
		point0.y = lines[i][1];
		double k = double((lines[i][3] - lines[i][1])) / double((lines[i][2] - lines[i][0]) + 1e-6);
		double b = point0.y - k * point0.x;
		if (abs(k) < 1)
		{
			ks1.push_back(k);
			b1.push_back(b);
			lines1.push_back(lines[i]);
			line(img_draw, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 0, 255), 1);
		}
		else if (abs(k) > 5)
		{
			ks2.push_back(k);
			b2.push_back(b);
			lines2.push_back(lines[i]);
			line(img_draw, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 255, 0), 1);
		}
	}
	vector<cv::Point2f>pts;
	for (int i = 0; i < ks1.size(); i++)
	{
		for (int j = 0; j < ks2.size(); j++)
		{
			cv::Point2f pt0 = getintersection(ks1[i], b1[i], ks2[j], b2[j]);
			pts.push_back(pt0);
			circle(img_draw, pt0, 2, Scalar(0, 0, 255), -1);
		}
	}
	vector<cv::Point2f>ptsNew = pointsClusters(pts, m_pointsClusters / m_binNum);
	cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, m_update, m_xyClassification / m_binNum);
	MLCherkerboardDetect cb;
	cv::Mat indexMap = cb.updateIndexMapKL(indexMap1, ptsNew, corners);
	if ((indexMap.rows * indexMap.cols) > corners.size())
	{
		re.flag = false;
		re.errMsg = info + "grid detection fail";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	//draw
	cv::Mat xlocMat(indexMap.size(), CV_32FC1, Scalar(0));
	cv::Mat ylocMat(indexMap.size(), CV_32FC1, Scalar(0));

	for (int i = 0; i < indexMap.rows; i++)
	{
		for (int j = 0; j < indexMap.cols; j++)
		{
			int in = indexMap.at<short>(i, j);
			if (in < corners.size())
			{
				circle(img_draw, corners[in], 2, Scalar(255, 0, 255), -1);
				// cout << corners[in] << endl;
				xlocMat.at<float>(i, j) = corners[in].x;
				ylocMat.at<float>(i, j) = corners[in].y;

			}
		}
	}

	re.xLocMat = xlocMat;
	re.yLocMat = ylocMat;
	re.corners = corners;
	re.imgdraw = img_draw;
	re.indexMap = indexMap;
	re.boardSize = xlocMat.size();
	return re;
}
GridRe MLImageDetection::MLGridDetect::getGridHoughLine1(cv::Mat img)
{

	string info = "------getGridHoughLine-------";
	GridRe re;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "input image is null";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	vector<cv::Point2f> corners;
	cv::Mat img8 = convertToUint8(img);
	//img8 = rotateGridImg(img8);
	cv::Mat srcgray = convertToGrayImage(img8);
	cv::Mat img_draw = convertTo3Channels(img8);
	cv::Mat srcbinary;
	cv::threshold(srcgray, srcbinary, 0, 255, CV_THRESH_OTSU);
	// cv::threshold(srcgray, srcbinary, 0, 255, CV_THRESH_TRIANGLE);
	int row = srcgray.rows;
	int col = srcgray.cols;
	Mat m;
	srcbinary.convertTo(m, CV_64FC1);
	Mat m_row(1, col, CV_64FC1, Scalar(0));
	Mat m_col(row, 1, CV_64FC1, Scalar(0));
	reduce(m, m_row, 0, CV_REDUCE_SUM);
	reduce(m, m_col, 1, CV_REDUCE_SUM);
	int numR = countNonZero(m_row);
	int numC = countNonZero(m_col);
	int l = (numR < numC ? numR : numC) / 6;
	cv::Mat srccanny;
	cv::Canny(srcbinary, srccanny, 30, 90);
	vector<Vec4i> lines;
	vector<Vec4i> lines1, lines2;
	vector<double> ks1, ks2;
	vector<double> b1, b2;
	cv::HoughLinesP(srccanny, lines, 1, CV_PI / 180, l, l, l);
	for (int i = 0; i < lines.size(); i++)
	{
		cv::Point point0;
		point0.x = lines[i][0];
		point0.y = lines[i][1];
		double k = double((lines[i][3] - lines[i][1])) / double((lines[i][2] - lines[i][0]));
		double b = point0.y - k * point0.x;
		if (abs(k) < 1)
		{
			ks1.push_back(k);
			b1.push_back(b);
			lines1.push_back(lines[i]);
			line(img_draw, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 0, 255), 1);
		}
		else if (abs(k) > 5)
		{
			k = double((lines[i][2] - lines[i][0])) / double((lines[i][3] - lines[i][1]));
			b = -(k * point0.y - point0.x);
			ks2.push_back(k);
			b2.push_back(b);
			lines2.push_back(lines[i]);
			line(img_draw, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 255, 0), 1);
		}
	}

	Mat b10(lines1.size(), 1, CV_32F);
	Mat b20(lines2.size(), 1, CV_32F);
	// ks1 分类
	for (int i = 0; i < b1.size(); i++)
	{
		b10.at<float>(i, 0) = b1[i];
	}
	Mat labelB1;
	Mat centersB1;
	cv::Size boadsize(13, 10);
	TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 0.1);
	cv::kmeans(b10, boadsize.width, labelB1, criteria, 1, KMEANS_PP_CENTERS, centersB1);

	Scalar colorTab[] = {//定义一个颜色的数组
						 Scalar(0, 0, 255),   Scalar(0, 255, 0),    Scalar(255, 0, 0),   Scalar(0, 255, 255),
						 Scalar(100, 0, 255), Scalar(50, 100, 255), Scalar(150, 0, 255), Scalar(200, 50, 255),
						 Scalar(220, 0, 200), Scalar(120, 0, 100),  Scalar(255, 0, 120) };
	vector<vector<Vec4i>> linesH;
	for (int i = 0; i < labelB1.rows; i++)
	{
		int index = labelB1.at<int>(i);
		//line(img_draw, Point(lines1[i][0], lines1[i][1]), Point(lines1[i][2], lines1[i][3]), colorTab[index], 1);
	}
	double k10 = 0;
	double sum = accumulate(ks1.begin(), ks1.end(), 0.0);
	k10 = sum / ks1.size();
	cv::Mat b1Ord;
	cv::sort(centersB1, b1Ord, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);

	// ks2 fenlei
	for (int i = 0; i < b2.size(); i++)
	{
		b20.at<float>(i, 0) = b2[i];
	}
	Mat labelB2;
	Mat centersB2;
	cv::kmeans(b20, boadsize.height, labelB2, criteria, 1, KMEANS_PP_CENTERS, centersB2);
	double k20 = 0;
	double sum2 = accumulate(ks2.begin(), ks2.end(), 0.0);
	k20 = sum2 / ks2.size();
	cv::Mat b2Ord, b2OrdIdx;
	cv::sort(centersB2, b2Ord, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
	cv::sortIdx(centersB2, b2OrdIdx, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);

	// draw lines1
	for (int i = 0; i < centersB1.rows; i++)
	{
		double k = k10;
		double b = centersB1.at<float>(i);
		cv::Point point1, point2;
		point1.x = 0;
		point1.y = k * 0 + b;
		point2.x = 3000;
		point2.y = k * 3000 + b;
		cv::line(img_draw, point1, point2, cv::Scalar(255, 255, 0), 1, 8, 0);
	}
	// draw lines2
	for (int i = 0; i < centersB2.rows; i++)
	{
		double k = k20;
		double b = centersB2.at<float>(i);
		cv::Point point1, point2;
		point1.x = 1;
		point1.y = k * point1.x + b;
		point2.x = 3000;
		point2.y = k * point2.x + b;
		cv::Point point10, point20;
		point10.x = point1.y;
		point10.y = point1.x;
		point20.x = point2.y;
		point20.y = point2.x;

		cv::line(img_draw, point10, point20, cv::Scalar(255, 0, 255), 1, 8, 0);
	}
	// calculate crosssection
	cv::Mat ordNew = Mat::zeros(boadsize.width, boadsize.height, CV_16UC1);
	for (int i = 0; i < b1Ord.rows; i++)
	{
		for (int j = 0; j < b2Ord.rows; j++)
		{
			double b1 = b1Ord.at<float>(i);
			double k1 = k10;
			double k21 = k20;
			double b21 = b2Ord.at<float>(j);
			double k2, b2;
			if (abs(k21) == 0)
			{
				k21 = 1e-6;
			}
			k2 = 1.0 / k21; // ;cv::min(k21, 1e6);
			b2 = -b21 / k21;// cv::min(k21, 1e6);
			cv::Point2f point;
			double x = 0, y = 0;
			x = (b2 - b1) / (k1 - k2);
			y = k1 * x + b1;
			point.x = x;
			point.y = y;
			corners.push_back(point);
			ordNew.at<short>(i, j) = (corners.size() - 1);
			//	draw
			circle(img_draw, point, 5, cv::Scalar(0, 0, 255), -1);
			if (i == 5 & j == 5)
			{
				circle(img_draw, point, 10, cv::Scalar(255, 0, 255), -1);
			}
		}
	}

	return GridRe();
}


GridRe MLImageDetection::MLGridDetect::getGridContour(cv::Mat img)
{
	string info = "------getGridContour------";
	GridRe re;
	cv::Mat indexMap;
	vector<cv::Point2f> corners;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "input image is null";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	cv::Mat img8 = convertToUint8(img);
	//img8 = rotateGridImg(img8);
	cv::Mat img_draw = convertTo3Channels(img8);
	Ptr<CLAHE> clahe = createCLAHE(2.0, Size(20, 20));
	clahe->apply(img8, img8);
	cv::Mat srcbinary;
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
   //  morphologyEx(img8, img8, MORPH_CLOSE, element, Point(-1, -1));
	cv::threshold(img8, srcbinary, 0, 255, CV_THRESH_OTSU);
	// cv::threshold(roi, srcbinary, 27, 255, CV_THRESH_BINARY);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	morphologyEx(srcbinary, srcbinary, MORPH_CLOSE, kernel, Point(-1, -1));
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	findContours(srcbinary, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	vector<cv::Point2f> pts;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i], false);
		if (area > 2e3 / pow(m_binNum, 2) && area < 25e4 / pow(m_binNum, 2))
		{
			Rect rect = boundingRect(contours[i]);
			Point2f P[4];
			P[0] = cv::Point2f(rect.x, rect.y);
			P[1] = cv::Point2f(rect.x, rect.y + rect.height);
			P[2] = cv::Point2f(rect.x + rect.width, rect.y + rect.height);
			P[3] = cv::Point2f(rect.x + rect.width, rect.y);

			double w = Getdistance(P[0], P[1]);
			double h = Getdistance(P[0], P[3]);
			double asp = min(w, h) / max(w, h);
			if (w > 50 / m_binNum & asp > 0.5 & asp < 1.2)
			{
				pts.push_back(P[0]);
				pts.push_back(P[1]);
				pts.push_back(P[2]);
				pts.push_back(P[3]);
			}
		}
	}
	drawPointsOnImage(img_draw, pts, 1);
	if (pts.size() < 1)
	{
		re.flag = false;
		re.errMsg = info + "grid detection fail";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}


	vector<cv::Point2f> ptsNew = pointsClusters(pts, m_pointsClusters / m_binNum);
	drawPointsOnImage(img_draw, ptsNew, 2, Scalar(0, 255, 0));
	cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, m_update, m_xyClassification / m_binNum);
	MLCherkerboardDetect cb;
	indexMap = cb.updateIndexMapKL(indexMap1, ptsNew, corners);
	if (corners.size() > ptsNew.size())
	{
		for (int i = ptsNew.size() - 1; i < corners.size(); i++)
		{
			corners[i] = getAccurateCenter(corners[i], img8);
		}
	}
	if (accurateFlag)
	{
		for (int i = 0; i < corners.size(); i++)
		{
			corners[i] = getAccurateCenter(corners[i], img8);
		}
	}

	cv::Mat xmat(indexMap.size(), CV_32FC1, Scalar(0));
	cv::Mat ymat(indexMap.size(), CV_32FC1, Scalar(0));
	if ((indexMap.rows * indexMap.cols) > corners.size())
	{
		re.flag = false;
		re.errMsg = info + "grid detection fail";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}

	// draw
	for (int i = 0; i < indexMap.rows; i++)
	{
		for (int j = 0; j < indexMap.cols; j++)
		{
			int in = indexMap.at<short>(i, j);
			if (in < corners.size())
			{
				circle(img_draw, corners[in], 10, Scalar(255, 0, 255), -1);
				xmat.at<float>(i, j) = corners[in].x;
				ymat.at<float>(i, j) = corners[in].y;
			}
		}
	}
	re.xLocMat = xmat;
	re.yLocMat = ymat;
	re.corners = corners;
	re.imgdraw = img_draw;
	re.indexMap = indexMap;
	re.boardSize = xmat.size();
	return re;
}

GridRe MLImageDetection::MLGridDetect::getGridTemplateGaussian(cv::Mat img)
{
	GridRe re;
	string info = "------getGridTemplateGaussian-------";
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "input image is null";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	vector<cv::Point2f>corners;

	cv::Mat img8 = convertToUint8(img);
	//img8 = rotateGridImg(img8);
	cv::Mat img_draw = convertTo3Channels(img8);
	cv::Mat img_draw1 = img_draw.clone();
	cv::Mat img_gray = img8;
	cv::Mat templ;

	templ =cv::imread("./config/ALGConfig/gridtemplate.tif", 0);
	if (templ.empty())
	{
		re.flag = false;
		re.errMsg = "the template image is null";
		return re;
	}
	    
	cv::resize(templ, templ, templ.size() / m_binNum);
	CrossCenter c0;
	cv::Point2f templCenter = c0.find_centerGaussian(templ);
	cv::Mat img_result, img_result1, img_result2;
	matchTemplate(img_gray, templ, img_result, TM_CCOEFF_NORMED);
	img_result = cv::abs(img_result);
	img_result.convertTo(img_result, CV_8UC1);
	cv::Mat imgre, imgre1, imgre2;
	//img_result.convertTo(img_result, CV_8UC1);
	//img_result = convertToUint8(img_result);
	threshold(img_result, imgre, 0.4, 255, CV_THRESH_BINARY);
	//threshold(img_result, imgre, 0, 255, CV_THRESH_OTSU);

	vector<cv::Point2f>pts;
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	findContours(imgre, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		cv::Rect rect = cv::boundingRect(contours[i]);
		double w = min(rect.width, rect.height);
		double h = max(rect.width, rect.height);
		double ratio = w / h;
		if (ratio > 0.7 && ratio < 1.2)
		{
			cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
			pts.push_back(rectR.center);
		}
	}

	vector<cv::Point2f>ptsNew;
	//TODO: 50
	ptsNew = pointsClusters(pts, m_pointsClusters / m_binNum);
	cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, m_update, m_xyClassification / m_binNum);
	MLCherkerboardDetect cb;
	cv::Mat indexMap = cb.updateIndexMapKL(indexMap1, ptsNew, corners);
	// draw1
	for (int i = 0; i < corners.size(); i++)
	{
		circle(img_draw1, corners[i] + templCenter, 5, Scalar(255, 0, 255));
	}

	//TODO: 添加rect越界判断
	clock_t start, finish;
	start = clock();
	for (int i = 0; i < indexMap.rows; i++)
	{
		int in = indexMap.at<short>(i, 0);
		cv::Rect rect = cv::Rect(corners[in], templ.size());

		cv::Mat roi = img_gray(rect);
		CrossCenter cc(roi, 10);
		// cv::Point2f c0 = cc.find_centerGaussian(roi);
		cv::Point2f c0 = cc.find_centerLINES(roi);
		if (c0.x > 0 && c0.y > 0)
		{
			c0 = c0 + cv::Point2f(rect.x, rect.y);
			corners[in] = c0;
		}
		circle(img_draw, c0, 1, Scalar(255, 0, 255), -1);

	}
	for (int i = 0; i < indexMap.rows; i++)
	{
		int in = indexMap.at<short>(i, indexMap.cols - 1);
		cv::Rect rect = cv::Rect(corners[in], templ.size());
		cv::Mat roi = img_gray(rect);
		CrossCenter cc(roi, 10);
		// cv::Point2f c0 = cc.find_centerGaussian(roi);
		cv::Point2f c0 = cc.find_centerLINES(roi);


		if (c0.x > 0 && c0.y > 0)
		{
			c0 = c0 + cv::Point2f(rect.x, rect.y);
			corners[in] = c0;
		}
		circle(img_draw, c0, 1, Scalar(255, 0, 255), -1);

	}
	for (int i = 1; i < indexMap.cols - 1; i++)
	{
		int in = indexMap.at<short>(0, i);
		cv::Rect rect = cv::Rect(corners[in], templ.size());
		cv::Mat roi = img_gray(rect);
		CrossCenter cc(roi, 10);
		// cv::Point2f c0 = cc.find_centerGaussian(roi);
		cv::Point2f c0 = cc.find_centerLINES(roi);

		if (c0.x > 0 && c0.y > 0)
		{
			c0 = c0 + cv::Point2f(rect.x, rect.y);
			corners[in] = c0;
		}
		circle(img_draw, c0, 1, Scalar(255, 0, 255), -1);
	}
	for (int i = 1; i < indexMap.cols - 1; i++)
	{
		int in = indexMap.at<short>(indexMap.rows - 1, i);
		cv::Rect rect = cv::Rect(corners[in], templ.size());
		cv::Mat roi = img_gray(rect);
		CrossCenter cc(roi, 10);
		//cv::Point2f c0 = cc.find_centerGaussian(roi);
		cv::Point2f c0 = cc.find_centerLINES(roi);
		if (c0.x > 0 && c0.y > 0)
		{
			c0 = c0 + cv::Point2f(rect.x, rect.y);
			corners[in] = c0;
		}
		circle(img_draw, c0, 1, Scalar(255, 0, 255), -1);
	}
	for (int i = 1; i < indexMap.rows - 1; i++)
	{
		for (int j = 1; j < indexMap.cols - 1; j++)
		{
			int in = indexMap.at<short>(i, j);
			corners[in] = corners[in] + templCenter;
		}
	}
	if ((indexMap.rows * indexMap.cols) > corners.size())
	{
		re.flag = false;
		re.errMsg = info + "grid detection fail";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	//draw
	cv::Mat xlocMat(indexMap.size(), CV_32FC1, Scalar(0));
	cv::Mat ylocMat(indexMap.size(), CV_32FC1, Scalar(0));

	for (int i = 0; i < indexMap.rows; i++)
	{
		for (int j = 0; j < indexMap.cols; j++)
		{
			int in = indexMap.at<short>(i, j);
			if (in < corners.size())
			{
				circle(img_draw, corners[in], 5, Scalar(255, 0, 255), -1);
				xlocMat.at<float>(i, j) = corners[in].x;
				ylocMat.at<float>(i, j) = corners[in].y;
			}
		}
	}

	re.xLocMat = xlocMat;
	re.yLocMat = ylocMat;
	re.corners = corners;
	re.imgdraw = img_draw;
	re.indexMap = indexMap;
	re.boardSize = xlocMat.size();

	return re;
}

GridRe MLImageDetection::MLGridDetect::getGridHist(cv::Mat img)
{

	string info = "------getGridHist------";
	GridRe re;
	cv::Mat indexMap;
	vector<cv::Point2f> corners;
	if (img.empty())
	{
		re.flag = false;
		re.errMsg = info + "input image is null";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	cv::Mat img8 = convertToUint8(img);
	//img8 = rotateGridImg(img8);
	cv::Mat img_draw = convertTo3Channels(img8);

	cv::Mat rowMat, colMat;
	cv::reduce(img8, rowMat, 0, REDUCE_AVG);
	cv::reduce(img8, colMat, 1, REDUCE_AVG);
	colMat = colMat.t();
	double maxV;
	cv::minMaxLoc(rowMat, NULL, &maxV);
	vector<int> xvec = findPeaks(rowMat, maxV * 0.8, 0, 0);
	cv::minMaxLoc(colMat, NULL, &maxV);
	vector<int> yvec = findPeaks(colMat, maxV * 0.8, 0, 0);
	cv::Mat xlocMat(xvec.size(), yvec.size(), CV_32FC1, cv::Scalar(0));
	cv::Mat ylocMat(xvec.size(), yvec.size(), CV_32FC1, cv::Scalar(0));
	if (xvec.size() < 1 || yvec.size() < 1)
	{
		re.flag = false;
		re.errMsg = info + "grid detection fail";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	vector<cv::Point2f>pts;
	for (int i = 0; i < xvec.size(); i++)
	{
		for (int j = 0; j < yvec.size(); j++)
		{
			cv::Point2f  c0(xvec[i], yvec[j]);
			if (accurateFlag)
			{
				c0 = getAccurateCenter(c0, img8);
			}
			pts.push_back(c0);
			xlocMat.at<float>(i, j) = c0.x;
			ylocMat.at<float>(i, j) = c0.y;
			circle(img_draw, c0, 2, Scalar(0, 0, 255), -1);
		}
	}

	re.xLocMat = xlocMat;
	re.yLocMat = ylocMat;
	re.corners = pts;
	re.imgdraw = img_draw;
	re.boardSize = xlocMat.size();
	return re;

}


cv::Mat MLImageDetection::MLGridDetect::rotateGridImg(cv::Mat img)
{
	cv::Mat img8;
	img8 = convertToUint8(img);
	cv::Mat imgth;
	cv::threshold(img8, imgth, 0, 255, THRESH_TRIANGLE);
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	findContours(imgth, contours, hierachy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	double theta = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Rect rect = cv::boundingRect(contours[i]);
		double area = cv::contourArea(contours[i]);
		if (area > 1e5)
		{
			cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
			cv::Point2f p[4];
			rectR.points(p);
			cv::Point2f sub1 = p[1] - p[0];
			cv::Point2f sub2 = p[2] - p[1];
			double theta1 = atan(sub1.y / sub1.x) * 180 / CV_PI;
			double theta2 = atan(sub2.y / sub2.x) * 180 / CV_PI;
			if (abs(theta1) < abs(theta2))
				theta = theta1;
			else
				theta = theta2;
		}
	}
	img = getRotationImg(img, theta);
	return img;
}
cv::Point2f MLImageDetection::MLGridDetect::getAccurateCenter(cv::Point2f c0, cv::Mat img)
{
	cv::Point2f cen;
	cv::Rect rect;
	rect.x = c0.x - m_len / 2 / m_binNum;
	rect.y = c0.y - m_len / 2 / m_binNum;
	rect.width = m_len / m_binNum;
	rect.height = m_len / m_binNum;
	if (rect.x < 0 || rect.y<0 || rect.br().x>img.cols || rect.br().y>img.rows)
		return c0;
	cv::Mat roi = img(rect).clone();
	CrossCenter cc;
	cv::Point2f c1 = cc.find_centerLINES(roi);
	if (c1.x > 1e-6 && c1.y > 1e-6)
		cen = c1 + Point2f(rect.tl());
	else
		cen = c0;
	return cen;
}


void MLImageDetection::MLGridDetect::SetbinNum(int bin)
{
	m_binNum = bin;
}

void MLImageDetection::MLGridDetect::SetGridPointsClusters(double thresh)
{
	m_pointsClusters = thresh;
}

void MLImageDetection::MLGridDetect::SetGridxyClassification(double thresh)
{
	m_xyClassification = thresh;
}

void MLImageDetection::MLGridDetect::SetGridWidth(double w)
{
	m_gridWidth = w;
}
void MLImageDetection::MLGridDetect::SetChessboardUpdateFlag(bool flag)
{
	m_update = flag;
}

#include "MLCherkerboardDetect.h"
#include "LogPlus.h"
#include "libcbdetect/boards_from_corners.h"
#include "libcbdetect/find_corners.h"
#include "ml_gridDetect.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <numeric>
#include <opencv2/imgproc/types_c.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\opencv.hpp>
#include <armadillo>

using namespace MLImageDetection;
using namespace std;
using namespace cv;
MLImageDetection::MLCherkerboardDetect::MLCherkerboardDetect()
{
}

MLImageDetection::MLCherkerboardDetect::~MLCherkerboardDetect()
{
}

cv::Rect getChessboardRect(cv::Mat roi)
{
	cv::Rect rect;
	if (roi.data != NULL)
	{
		cv::Mat img_gray, img_gray1, img_draw;
		if (roi.depth() == CV_8U)
		{
			img_gray1 = roi.clone();
			img_draw = roi.clone();
		}
		else
		{
			MLimagePublic mlp;
			img_gray1 = mlp.convertToUint8(roi);
			img_draw = mlp.convertToUint8(roi);
		}
		if (img_gray1.channels() == 3)
			cvtColor(img_gray1, img_gray1, CV_BGR2GRAY);

		std::vector<std::vector<cv::Point>> contours;
		std::vector<std::vector<cv::Point>> contoursNew;
		std::vector<cv::Vec4i> hierarchy;
		cv::Mat img_process, srcbinary;
		cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		// cv::GaussianBlur(img_gray1, img_gray1, cv::Size(9, 9), 2, 2);
		// cv::morphologyEx(img_gray1, img_gray1, cv::MORPH_CLOSE, element1);
		// cv::morphologyEx(img_gray1, img_process, cv::MORPH_GRADIENT, element1);
		threshold(img_gray1, srcbinary, 0, 255, THRESH_TRIANGLE);
		// cv::Mat img_canny;
		// cv::Canny(srcbinary, img_canny, 50, 150);
		cv::findContours(srcbinary, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
		vector<double> area_sort;
		vector<int> list;
		cv::Mat roi;
		vector<double> areaVec;
		for (int i = 0; i < contours.size(); ++i)
		{
			double area = contourArea(contours[i], false);
			// cout << area << endl;
			if (area > 1e6) //&& area < 4.2e6 / powbin)
			// if (area > 4e6 / powbin && area < 6e6 / powbin)
			{
				rect = cv::boundingRect(cv::Mat(contours[i]));
				cv::rectangle(img_draw, rect, Scalar(0, 0, 255), 1);
				contoursNew.push_back(contours[i]);
				areaVec.push_back(area);
				// roi = img_gray1(rect_new);
			}
		}
		if (contoursNew.size() > 0)
		{
			int maxPos = min_element(areaVec.begin(), areaVec.end()) - areaVec.begin();
			rect = cv::boundingRect(cv::Mat(contoursNew[maxPos]));
			roi = img_draw(rect);
		}
	}
	return rect;
}

CheckerboardRe MLImageDetection::MLCherkerboardDetect::detectChessboardCorner(cv::Mat img, double crRatio, int binNum)
{
	CheckerboardRe checkerRe;
	cv::Mat indexMap;
	m_CRAreaPercent = crRatio;
	vector<cv::Point2f> cornersRe;
	if (img.data != NULL)
	{
		cv::Mat img_gray = convertToUint8(img);
		cv::Mat img_draw = convertTo3Channels(img_gray);
		cv::Mat roi = img_gray; // img_gray; // (cv::Rect(2900, 1700, 800, 800));
		std::vector<cv::Point2f> result;
		int errCode = 0;
		cbdetect::Corner corners;
		cbdetect::Params params;
		params.corner_type = cbdetect::SaddlePoint;
		params.show_debug_image = false;
		params.find_corners_reiszed = false;
		vector<cbdetect::Board> chessboard;
		cv::GaussianBlur(roi, roi, cv::Size(3, 3), 0, 0);
		// cv::medianBlur(roi, roi, 3);
		// equalizeHist(roi, roi);
		// add the preprocess
		Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
		clahe->apply(roi, roi);
		//Mat srcbinary = roi.clone();
		//threshold(roi, srcbinary, 0, 255, THRESH_OTSU);
		// adaptiveThreshold(roi, srcbinary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 17, 0);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(80, 80), Point(-1, -1));
		//   morphologyEx(srcbinary, srcbinary, MORPH_OPEN, kernel, Point(-1, -1));
		Mat kernel1 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
		//morphologyEx(srcbinary, srcbinary, MORPH_CLOSE, kernel1, Point(-1, -1));
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		//cv::dilate(srcbinary, srcbinary, element);
		// cv::morphologyEx(srcbinary, srcbinary, MORPH_CLOSE, kernel, Point(-1, -1));

		// morphologyEx(srcbinary, srcbinary, MORPH_DILATE, kernel, Point(-1, -1));
		vector<double> x, y;
		std::vector<cv::Point2f> cornersPoint;
		vector<cv::Point2f> cornersPointNew;
		
			cbdetect::find_corners(roi, corners, params);
			// cbdetect::plot_corners(img_draw, corners);
			if (corners.p.size() > 0)
			{
				cbdetect::boards_from_corners(roi, corners, chessboard, params);
				//   cbdetect::plot_boards(img_draw, corners, chessboard, params);
			}
			else
			{
				checkerRe.flag = false;
				checkerRe.errMsg = "checker corner detection fail";
				return checkerRe;
			}
			//cornersPoint = corners.p;
			for (int j = 0; j < corners.p.size(); j++)
			{
				cv::Point2f pt;
				pt.x = corners.p.at(j).x; // +rectGrid.x;
				pt.y = corners.p.at(j).y; // +rectGrid.y;
				circle(img_draw, pt, 5, cv::Scalar(0, 0, 255), -1);
				cornersPoint.push_back(pt);
				// cout << pt << endl;
			}
			if (corners.p.size() > 0 & chessboard.size() > 0)
			{
				cbdetect::Board board;
				board = chessboard[0];
				vector<std::vector<int>> index = board.idx;
				int len = index.size();
				int len1 = index[0].size();
				indexMap = cv::Mat::zeros(len, len1, CV_16S);
				// change the vector to the mat
				for (int i = 0; i < len; i++)
				{
					for (int j = 0; j < len1; j++)
					{
						int a = index[i][j];
						indexMap.at<short>(i, j) = a;
					}
					// cout << endl;
				}
				indexMap = indexMap(cv::Range(1, indexMap.rows-1), cv::Range(1, indexMap.cols-1));
				int left = 0, right = 0;
				int top = 0, bottom = 0;
				// searchColIndex(indexMap, left, right);
				// indexMap = indexMap(cv::Range(0, indexMap.rows), cv::Range(left, right));
				// searchRowIndex(indexMap, top, bottom);

				//   transpose(indexMap, indexMap);
				//  rotate(indexMap, indexMap, ROTATE_90_CLOCKWISE);
				rotate(indexMap, indexMap, ROTATE_90_COUNTERCLOCKWISE);

				//vector<cv::Point2f> cornersPointNew;
				cv::Mat indexMapUpdate;
				indexMapUpdate = updateIndexMapKL(indexMap, cornersPoint, cornersPointNew);
				searchColIndex(indexMapUpdate, left, right);
				indexMapUpdate = indexMapUpdate(cv::Range(0, indexMap.rows), cv::Range(left, right));
				searchRowIndex(indexMapUpdate, top, bottom);
				cv::Mat indexMapNew = indexMap(Range(top, bottom), cv::Range(0, indexMap.cols));
				// cv::Mat indexMapUpdate;
				//  vector<cv::Point2f>cornersPointNew;
				indexMapUpdate = updateIndexMapKL(indexMapNew, cornersPointNew, cornersPointNew);
				int row = indexMapUpdate.rows;
				int col = indexMapUpdate.cols;
				cv::Mat xPoints = cv::Mat::zeros(row, col, CV_32F);
				cv::Mat yPoints = cv::Mat::zeros(row, col, CV_32F);
				cv::Mat xloc = cv::Mat(indexMap.size(), CV_32FC1);
				cv::Mat yloc = cv::Mat(indexMap.size(), CV_32FC1);

				// m_ChessboardCR.boardSize = indexMapUpdate.size();
				m_boardsize = indexMapUpdate.size();
				indexMap = indexMapUpdate;
				cornersRe = cornersPointNew;

				drawPointsOnImage(img_draw, cornersPointNew,5);
				for (int i = 0; i < indexMap.rows; i++)
				{
					for (int j = 0; j < indexMap.cols; j++)
					{
						int index = indexMap.at<short>(i, j);
						cv::Point2f pt0 = cornersPointNew[index];
						
						if (false)
						{
							int len = 20;
							cv::Rect rect(pt0.x - len / 2, pt0.y - len / 2, len, len);
							cv::Mat pt0ROI = roi(rect).clone();
							cv::Scalar m0, std0;
							cv::meanStdDev(pt0ROI, m0, std0);
							//cout << std0(0) << endl;
							if (std0(0) < 10)
							{
								pt0.x = NAN;
								pt0.y = NAN;
							}
						}
					
						xloc.at<float>(i, j) = pt0.x;
						yloc.at<float>(i, j) = pt0.y;
					}
				}
				checkerRe.xLocMat = xloc;
				checkerRe.yLocMat = yloc;
			}
		
		vector<cv::Point2f> centerVecP;
		vector<cv::Rect> rectVec = getCRROI(cornersPointNew, indexMap, centerVecP);
		//drawRectsOnImage(img_draw, rectVec,2);
		checkerRe.pts = cornersPointNew;
		checkerRe.rectVec = rectVec;
		checkerRe.flag = true;
		checkerRe.boardSize = indexMap.size();
		checkerRe.img_draw = img_draw;
	}

	return checkerRe;
}
CheckerboardRe MLImageDetection::MLCherkerboardDetect::detectChessboardCorner1(cv::Mat img, double crRatio, int binNum)
{
	CheckerboardRe checkerRe;
	cv::Mat indexMap;
	vector<cv::Point2f> cornersRe;
	m_CRAreaPercent = crRatio;

	if (img.data != NULL)
	{
		cv::Mat img_gray = convertToUint8(img_gray);
		cv::Mat img_draw = convertTo3Channels(img_gray);
		cv::Mat roi = img_gray; // img_gray; // (cv::Rect(2900, 1700, 800, 800));
		std::vector<cv::Point2f> result;
		int errCode = 0;
		cbdetect::Corner corners;
		cbdetect::Params params;
		params.corner_type = cbdetect::SaddlePoint;
		params.show_debug_image = false;
		params.find_corners_reiszed = false;
		vector<cbdetect::Board> chessboard;
		cv::GaussianBlur(roi, roi, cv::Size(3, 3), 0, 0);
		Ptr<CLAHE> clahe = createCLAHE(2.0, Size(20, 20));
		clahe->apply(roi, roi);
		Mat srcbinary;
		threshold(roi, srcbinary, 0, 255, THRESH_OTSU);
		// adaptiveThreshold(roi, srcbinary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 17, 0);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(80, 80), Point(-1, -1));
		Mat kernel1 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
		morphologyEx(srcbinary, srcbinary, MORPH_CLOSE, kernel1, Point(-1, -1));
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		cv::dilate(srcbinary, srcbinary, element);
		cv::Mat roiimg = roi.clone(); // srcbinary.clone();
		vector<double> x, y;
		std::vector<cv::Point2f> cornersPoint;
		if (roiimg.data)
		{
			cbdetect::find_corners(roi, corners, params);
			// cbdetect::plot_corners(img_draw, corners);
		   // cornersPoint = corners.p;
			for (int j = 0; j < corners.p.size(); j++)
			{
				cv::Point2f pt;
				pt.x = corners.p.at(j).x; // +rectGrid.x;
				pt.y = corners.p.at(j).y; // +rectGrid.y;
				cornersPoint.push_back(pt);
				circle(img_draw, pt, 10, cv::Scalar(0, 0, 255), -1);
			}
			double thred1 = m_pointsClusters;
			vector<cv::Point2f> ptsNew = pointsClusters(cornersPoint, 120.0 / binNum);
			int thred2 = m_xyClassification;
			cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, false, 150.0 / binNum);
			vector<cv::Point2f> corners1;
			indexMap = updateIndexMapKL(indexMap1, ptsNew, corners1);
			// draw
			for (int i = 0; i < indexMap.rows; i++)
			{
				for (int j = 0; j < indexMap.cols; j++)
				{
					int in = indexMap.at<short>(i, j);
					// cout << in << endl;
					if (in > -1)
						circle(img_draw, corners1[in], 10, Scalar(255, 0, 255), -1);
				}
			}
			cornersRe = corners1;
		}
		vector<cv::Point2f> centerVecP;
		vector<cv::Rect> rectVec = getCRROI(cornersRe, indexMap, centerVecP);
		for (int i = 0; i < rectVec.size(); i++)
		{
			cv::rectangle(img_draw, rectVec[i], Scalar(0, 0, 255), 2);
		}
		checkerRe.pts = cornersRe;
		checkerRe.rectVec = rectVec;
		checkerRe.flag = true;
		checkerRe.boardSize = indexMap.size();
		checkerRe.img_draw = img_draw;
	}
	return checkerRe;
}

CheckerboardRe MLImageDetection::MLCherkerboardDetect::detectChessboardHoughLine(cv::Mat img, double crRatio,
	int binNum)
{
	CheckerboardRe checkerRe;
	cv::Mat indexMap;
	vector<cv::Point2f> corners;
	m_CRAreaPercent = crRatio;

	if (img.data != NULL)
	{
		cv::Mat img_draw = convertToUint8(img);
		cv::Rect rectGrid = getChessboardRect(img_draw);
		cv::Mat roi = GetROIMat(img_draw, rectGrid);
		if (rectGrid.width < 0)
		{
			rectGrid.x = 0;
			rectGrid.y = 0;
		}
		// test end
		// vector<Point2f> corns;
		cv::Mat ordList;
		MLGridDetect grid;
		corners;// = grid.GridHoughLine(roi, indexMap, m_boardsize);
		for (int i = 0; i < corners.size(); i++)
		{
			corners[i].x = corners[i].x + rectGrid.x;
			corners[i].y = corners[i].y + rectGrid.y;
			circle(img_draw, corners[i], 5, cv::Scalar(255, 0, 255), -1);
		}
		vector<cv::Point2f> centerVecP;
		vector<cv::Rect> rectVec = getCRROI(corners, indexMap, centerVecP);
		for (int i = 0; i < rectVec.size(); i++)
		{
			cv::rectangle(img_draw, rectVec[i], Scalar(0, 0, 255), 2);
		}
		checkerRe.pts = corners;
		checkerRe.rectVec = rectVec;
		checkerRe.flag = true;
		checkerRe.boardSize = indexMap.size();
		checkerRe.img_draw = img_draw;
	}

	return checkerRe;
}
CheckerboardRe MLImageDetection::MLCherkerboardDetect::detectChessboardTemplate(cv::Mat img, double crRatio, int binNum)
{
	CheckerboardRe checkerRe;
	vector<cv::Point2f> corners;
	m_CRAreaPercent = crRatio;
	cv::Mat indexMap;
	if (img.data != NULL)
	{
		cv::Mat img_draw = convertToUint8(img);
		cv::Rect rect = getChessboardRect(img_draw);
		cv::Mat roi = GetROIMat(img_draw, rect);

		if (rect.width < 0)
		{
			rect.x = 0;
			rect.y = 0;
		}
		cv::Point2f start = cv::Point2f(rect.x, rect.y);
		cv::Mat img_draw1 = roi.clone();
		cv::Mat templ = cv::imread("./config/ALGConfig/chessboardtemplate1.tif", 0);
		cv::Mat templ1 = cv::imread("./config/ALGConfig/chessboardtemplate2.tif", 0);
		// 自动检测中心坐标
		// MLBMWFiducialDetect bwm;
		// bwm.BMWHoughLineDetect(templ);
		// bwm.BMWHoughLineDetect(templ1);
		//模板定，坐标定
		cv::Point c0 = cv::Point(templ.rows / 2, templ.cols / 2);
		cv::Point c1 = cv::Point(templ1.rows / 2, templ1.cols / 2);
		vector<cv::Point> P, P1;
		// TODO:0.9 *50 hard code
		GetAllMinLoc(roi, templ, 0.9, Scalar(0, 0, 0), &P, 18);
		GetAllMinLoc(roi, templ1, 0.9, Scalar(0, 0, 0), &P1, 18);
		vector<cv::Point2f> pts, ptsNew;
		for (int i = 0; i < P.size(); i++)
		{
			pts.push_back(P[i] + c0);
		}
		for (int i = 0; i < P1.size(); i++)
		{
			pts.push_back(P1[i] + c1);
		}
		ptsNew = pointsClusters(pts, 50);
		cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, false, 50);
		indexMap = updateIndexMapKL(indexMap1, ptsNew, corners);
		for (int i = 0; i < corners.size(); i++)
		{
			corners[i] = corners[i] + start;
		}
		// draw
		for (int i = 0; i < indexMap.rows; i++)
		{
			for (int j = 0; j < indexMap.cols; j++)
			{
				int in = indexMap.at<short>(i, j);
				circle(img_draw, corners[in], 10, Scalar(255, 0, 255), -1);
			}
		}
		vector<cv::Point2f> centerVecP;
		vector<cv::Rect> rectVec = getCRROI(corners, indexMap, centerVecP);
		for (int i = 0; i < rectVec.size(); i++)
		{
			cv::rectangle(img_draw, rectVec[i], Scalar(0, 0, 255), 2);
		}
		checkerRe.pts = corners;
		checkerRe.rectVec = rectVec;
		checkerRe.flag = true;
		checkerRe.boardSize = indexMap.size();
		checkerRe.img_draw = img_draw;
	}
	return checkerRe;
}
CheckerboardRe MLImageDetection::MLCherkerboardDetect::detectChessboardContour(cv::Mat img, double crRatio, int binNum)
{
	CheckerboardRe checkerRe;
	vector<cv::Point2f> corners;
	cv::Mat indexMap;
	m_CRAreaPercent = crRatio;

	if (img.data != NULL)
	{
		cv::Mat gray;
		if (img.channels() == 3)
			cvtColor(img, gray, COLOR_BGR2GRAY);
		else
			gray = img.clone();
		gray = convertToUint8(gray);
		cv::Mat img_draw = convertTo3Channels(gray);
		cv::Rect rect;
		cv::Mat roi = gray.clone();
		cv::Mat img_draw1 = img_draw.clone();
		cv::Mat srcbinary;
		Ptr<CLAHE> clahe = createCLAHE(2.0, Size(20, 20));
		// clahe->apply(roi, roi);
		// cv::equalizeHist(roi, roi);
		//cv::threshold(roi, srcbinary, 0, 255, CV_THRESH_TRIANGLE);
		  cv::threshold(roi, srcbinary, 0, 255, CV_THRESH_OTSU);

		Mat kernel = getStructuringElement(MORPH_RECT, Size(80, 80), Point(-1, -1));
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(16 / binNum, 16 / binNum));
		cv::dilate(srcbinary, srcbinary, element);
		cv::Mat srccanny;
		Canny(srcbinary, srccanny, 30, 90);
		vector<vector<Point>> contours;
		vector<Vec4i> hierachy;
		findContours(srcbinary, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		//drawContours(img_draw1, contours, -1, Scalar(0, 255, 0), 3, 8);

		vector<Vec4i> hierachy_new;
		vector<cv::Point2f> pts;
		for (int i = 0; i < contours.size(); i++)
		{
			cv::Rect rect = cv::boundingRect(contours[i]);
			double area = rect.area();
			double ratio = double(min(rect.width, rect.height)) / double(max(rect.width, rect.height));

			// cout << area << endl;
			if (area > 1e4 / pow(binNum, 2) & area < 2e6 / pow(binNum, 2) & ratio> 0.9 & ratio < 1.2)
			{
				//drawContours(img_draw1, contours, i, Scalar(0, 0, 255), 3, 8);

				pts.push_back(Point2f(rect.x, rect.y));
				pts.push_back(Point2f(rect.x + rect.width, rect.y));
				pts.push_back(Point2f(rect.x, rect.y + rect.height));
				pts.push_back(Point2f(rect.x + rect.width, rect.y + rect.height));
			}
		}
		// draw
		for (int i = 0; i < pts.size(); i++)
		{
			circle(img_draw1, pts[i], 5, Scalar(0, 0, 255), -1);
		}
		
		int thred1 = m_pointsClusters;
		vector<cv::Point2f> ptsNew = pointsClusters(pts, m_pointsClusters);
		
		drawPointsOnImage(img_draw1, ptsNew,5,Scalar(0,255,0));
		cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, m_update, m_xyClassification / binNum);
		indexMap = updateIndexMapKL(indexMap1, ptsNew, corners);

		cv::Mat xLocMat(indexMap1.size(),CV_32FC1,Scalar(-1)),
			yLocMat(indexMap1.size(), CV_32FC1, Scalar(-1));
		// draw
		for (int i = 0; i < indexMap.rows; i++)
		{
			for (int j = 0; j < indexMap.cols; j++)
			{
				int in = indexMap.at<short>(i, j);
				if (in > -1)
				{
					circle(img_draw, corners[in], 5, Scalar(255, 0, 255), -1);
					xLocMat.at<float>(i, j) = corners[in].x;
					yLocMat.at<float>(i, j) = corners[in].y;

				}
			}
		}

		vector<cv::Point2f> centerVecP;
		vector<cv::Rect> rectVec = getCRROI(corners, indexMap, centerVecP);
		for (int i = 0; i < rectVec.size(); i++)
		{
			cv::rectangle(img_draw, rectVec[i], Scalar(0, 0, 255), 2);
		}
		checkerRe.pts = corners;
		checkerRe.rectVec = rectVec;
		checkerRe.flag = true;
		checkerRe.boardSize = indexMap.size();
		checkerRe.img_draw = img_draw;
		checkerRe.xLocMat = xLocMat;
		checkerRe.yLocMat = yLocMat;
	}
	return checkerRe;
}
CheckerboardRe MLImageDetection::MLCherkerboardDetect::detectChessboardTemplate1(cv::Mat img, double crRatio,
	int binNum)
{
	m_CRAreaPercent = crRatio;
	CheckerboardRe checkerRe;
	vector<cv::Point2f> corners;
	cv::Mat indexMap;
	cv::Mat img_draw;
	if (img.data != NULL)
	{
		img = convertToUint8(img);
		img_draw = convertTo3Channels(img);
		cv::Mat img_draw1 = convertTo3Channels(img);
		//string filepath1 = "E:/project/IQMetric/x64/Debug/config/ALGConfig/chessboardtemplate1.tif";
		string filepath1 = "./config/ALGConfig/chessboardtemplate1.tif";
		cv::Mat templ = cv::imread(filepath1, 0);
		if(templ.empty())
			templ = cv::imread("../config/ALGConfig/chessboardtemplate1.tif", 0);
		if (templ.data == NULL)
		{
			checkerRe.flag = false;
			checkerRe.errMsg = "Input templ is null" + filepath1;
			LOG4CPLUS_INFO(LogPlus::getInstance()->logger, checkerRe.errMsg.c_str());
			return checkerRe;
		}
		cv::resize(templ,templ, templ.size() / binNum);
		Ptr<CLAHE> clahe = createCLAHE(2.0, Size(10, 10));
		// clahe->apply(gray, gray);

		vector<cv::Point2f> pts1 = matchTemplateMaxLocs(img, templ, 0.7, binNum);
		drawPointsOnImage(img_draw1, pts1, 5,Scalar(0,255,0));
		vector<cv::Point2f> ptsNew = pointsClusters(pts1, m_pointsClusters);
		cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, m_update, m_xyClassification);
		indexMap = updateIndexMapKL(indexMap1, ptsNew, corners);
		// draw
		cv::Mat xLocMat(indexMap.size(), CV_32FC1, Scalar(-1));
		cv::Mat yLocMat(indexMap.size(), CV_32FC1, Scalar(-1));

		for (int i = 0; i < indexMap.rows; i++)
		{
			for (int j = 0; j < indexMap.cols; j++)
			{
				int in = indexMap.at<short>(i, j);
				if (in >= 0)
				{
					circle(img_draw, corners[in], 24/binNum, Scalar(0, 0, 255), -1);
					xLocMat.at<float>(i, j) = corners[in].x;
					yLocMat.at<float>(i, j) = corners[in].y;

				}
			}
		}
		vector<cv::Point2f> centerVecP;
		vector<cv::Rect> rectVec = getCRROI(corners, indexMap, centerVecP);
		//drawRectsOnImage(img_draw, rectVec);
		checkerRe.xLocMat = xLocMat;
		checkerRe.yLocMat = yLocMat;
		checkerRe.pts = corners;
		checkerRe.rectVec = rectVec;
		checkerRe.indexMap = indexMap;
 		checkerRe.boardSize = indexMap.size();		
		checkerRe.img_draw = img_draw;
		checkerRe.center = getPtsCenter(corners);
	}
	return checkerRe;
}
CheckerboardRe MLImageDetection::MLCherkerboardDetect::detectChessboardHist(cv::Mat img, double crRatio)
{
	m_CRAreaPercent = crRatio;
	CheckerboardRe checkerRe;
	vector<cv::Point2f> corners;
	cv::Mat img_draw;
	if (img.data != NULL)
	{
		img = convertToUint8(img);
		img_draw = convertTo3Channels(img);
		cv::Mat img_draw1 = convertTo3Channels(img);

		cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, cv::Size(5, 5));
		cv::morphologyEx(img, img, MORPH_GRADIENT, kernel);
		//cv::resize(img, img, img.size() / 2);
		cv::Mat rowMat, colMat;
		//img.convertTo(img, CV_64FC1);
		cv::reduce(img, rowMat, 0, REDUCE_AVG);
		cv::reduce(img, colMat, 1, REDUCE_AVG);
		colMat = colMat.t();
		double maxV;
		cv::minMaxLoc(rowMat, NULL, &maxV);
		vector<int> xvec = findPeaks(rowMat, maxV * 0.2, 0, 0);
		cv::minMaxLoc(colMat, NULL, &maxV);
		vector<int> yvec = findPeaks(colMat, maxV * 0.2, 0, 0);
		if (xvec.size() < 1 || yvec.size() < 1)
		{
			checkerRe.flag = false;
			checkerRe.errMsg = "checker detection fail";
			//LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
			return checkerRe;
		}
		cv::Mat xLocMat(xvec.size(), yvec.size(), CV_32FC1, cv::Scalar(0));
		cv::Mat yLocMat(xvec.size(), yvec.size(), CV_32FC1, cv::Scalar(0));
		cv::Mat indexMap(xvec.size(), yvec.size(), CV_16S, cv::Scalar(0));

		int n = 0;
		for (int i = 0; i < xvec.size(); i++)
		{
			for (int j = 0; j < yvec.size(); j++)
			{
				cv::Point2f  c0(xvec[i], yvec[j]);
		
				corners.push_back(c0);
				xLocMat.at<float>(i, j) = c0.x;
				yLocMat.at<float>(i, j) = c0.y;
				indexMap.at<short>(i, j) = n;
				n++;
				circle(img_draw, c0, 2, Scalar(0, 0, 255), -1);
				circle(img_draw1, cv::Point(xvec[i], yvec[j]), 2, Scalar(0, 0, 255), -1);
			}
		}
		vector<cv::Point2f> centerVecP;
		vector<cv::Rect> rectVec = getCRROI(corners, indexMap, centerVecP);
		//drawRectsOnImage(img_draw, rectVec);
		checkerRe.xLocMat = xLocMat;
		checkerRe.yLocMat = yLocMat;
		checkerRe.pts = corners;
		checkerRe.rectVec = rectVec;
		checkerRe.indexMap = indexMap;
		checkerRe.boardSize = indexMap.size();
		checkerRe.img_draw = img_draw;
		checkerRe.center = getPtsCenter(corners);
	}
	return checkerRe;
}
vector<cv::Point2f> MLImageDetection::MLCherkerboardDetect::matchTemplateMaxLocs(cv::Mat img, cv::Mat templ, double score,
	int bin)
{
	cv::Point2f c0 = cv::Point(templ.rows / 2, templ.cols / 2);
	vector<cv::Point2f> pts;
	cv::Mat img_draw = convertTo3Channels(img);
	cv::Mat img_result;
	matchTemplate(img, templ, img_result, TM_CCOEFF_NORMED);
	img_result = cv::abs(img_result);
	cv::Mat imgre;
	threshold(img_result, imgre, score, 255, CV_THRESH_BINARY);
	imgre.convertTo(imgre, CV_8UC1);
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	findContours(imgre, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	vector<Vec4i> hierachy_new;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i], false);
		if (area > 50 / pow(bin, 2) & area < 1e5 / pow(bin, 2))
		{
			// cout << area << endl;
			drawContours(img_draw, contours, i, Scalar(rand() * 255, rand() * 255, rand() * 255), 3, 8);
			Rect rect = boundingRect(contours[i]);
			rectangle(img_draw, rect, Scalar(255, 0, 255), 2);
			cv::Mat roi1 = img_result(rect);
			double minval, maxval;
			Point minloc, maxloc;
			cv::minMaxLoc(roi1, &minval, &maxval, &minloc, &maxloc);
			cv::Point2f loc = cv::Point2f(maxloc + cv::Point(rect.x, rect.y));

			cv::Rect rect1 = cv::Rect(loc.x, loc.y, templ.rows, templ.cols);
			rectangle(img_draw, rect1, Scalar(0, 0, 255), 2);
			cv::Mat roi = img(rect1).clone();
			Scalar m, std;
			cv::meanStdDev(roi, m, std);
			if (std(0) >5 )
			{
				pts.push_back(loc+c0);
				circle(img_draw, loc+c0, 5, Scalar(255, 0, 255), -1);
			}
		}
	}
	return pts;
}
vector<cv::Rect> MLImageDetection::MLCherkerboardDetect::getCRROI(vector<cv::Point2f> corners, cv::Mat indexMap,
	vector<cv::Point2f>& centerVec)
{
	vector<cv::Rect> pos;
	if (corners.size() > 0 & indexMap.data != NULL)
	{
		int row = indexMap.rows;
		int col = indexMap.cols;
		cv::Mat xPoints = cv::Mat::zeros(row, col, CV_32F);
		cv::Mat yPoints = cv::Mat::zeros(row, col, CV_32F);
		//m_ChessboardCR.boardSize = indexMap.size();
		getXYcoordinate(indexMap, corners, xPoints, yPoints);
		// draw  test the function result
		for (int i = 0; i < xPoints.rows; i++)
		{
			for (int j = 0; j < xPoints.cols; j++)
			{
				float x = xPoints.at<float>(i, j);
				float y = yPoints.at<float>(i, j);
				// cv::circle(m_img_drawP, Point2f(x, y), 2, cv::Scalar(255, 255, 0), -1);
			}
		}
		cv::Mat xcenter, ycenter;
		cv::Mat px1, px2, px3, px4, py1, py2, py3, py4;
		getCenter(xPoints, xcenter, px1, px2, px3, px4);
		getCenter(yPoints, ycenter, py1, py2, py3, py4);
		// for (int i = 0; i < xcenter.rows; i++)
		//{
		//    for (int j = 0; j < xcenter.cols; j++)
		//    {
		//        float x = xcenter.at<float>(i, j);
		//        float y = ycenter.at<float>(i, j);
		//        cv::circle(m_img_drawP, cv::Point2f(x, y), 1, cv::Scalar(255, 255, 0), -1);
		//    }
		//}
		cv::Mat boardArea = getArea(px1, px2, px3, px4, py1, py2, py3, py4);
		cv::Mat boardRadius;
		// double percent = HydrusParameters::crArea; // 0.8;//crAreaPercent;
		double percent = m_CRAreaPercent; // 0.8;//crAreaPercent;

		cv::sqrt(boardArea * percent, boardRadius);
		for (int i = 0; i < xcenter.rows; i++)
		{
			for (int j = 0; j < xcenter.cols; j++)
			{
				float x = xcenter.at<float>(i, j) - boardRadius.at<float>(i, j) / 2;
				float y = ycenter.at<float>(i, j) - boardRadius.at<float>(i, j) / 2;
				Point2f center;
				center.x = xcenter.at<float>(i, j);
				center.y = ycenter.at<float>(i, j);
				cv::Point2f start = cv::Point2f(x, y);
				cv::Point2f end = cv::Point2f(x + boardRadius.at<float>(i, j), y + boardRadius.at<float>(i, j));
				// cv::Rect temp = cv::Rect(x, y, boardRadius.at<float>(i, j), boardRadius.at<float>(i, j));
				cv::Rect temp = cv::Rect(start, end);
				//cv::rectangle(m_img_drawP, temp, cv::Scalar(255, 0, 255), 1);
				//cv::circle(m_img_drawP, center, 5, Scalar(0, 0, 255), -1);
				pos.push_back(temp);
				centerVec.push_back(center);
			}
		}
		// m_img_drawP = img_draw.clone();
	}
	return pos;
}
bool MLImageDetection::MLCherkerboardDetect::getNewROIRect(vector<Rect> roiVecP, vector<Rect> roiVecN, vector<Rect>& NewroiVecP,
	vector<Rect>& NewroiVecN)
{
	bool flag = true;
	if (roiVecN.size() > 0 & roiVecP.size() > 0)
	{
		bool flagReorder = ReoderROIVec(roiVecP, roiVecN, NewroiVecP, NewroiVecN);
	}

	else
	{
		if (roiVecP.size() == 0)
		{
			NewroiVecP = roiVecN;
			NewroiVecN = roiVecN;
		}
		else if (roiVecN.size() == 0)
		{
			NewroiVecP = roiVecP;
			NewroiVecN = roiVecP;
		}
	}
	return flag;
}
int MLImageDetection::MLCherkerboardDetect::findNegtiveNum(vector<int> list)
{
	int len = 0;
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i] < 0)
		{
			len = len + 1;
		}
	}
	return len;
}
int MLImageDetection::MLCherkerboardDetect::findMapNegNum(cv::Mat list)
{
	int len = 0;
	for (int i = 0; i < list.rows; i++)
	{
		for (int j = 0; j < list.cols; j++)
		{
			int temp = list.at<short>(i, j);
			if (temp < 0)
			{
				len = len + 1;
			}
		}
	}
	return len;
}
bool MLImageDetection::MLCherkerboardDetect::searchColIndex(cv::Mat indexMap, int& start, int& end)
{
	bool flag = true;
	int row = indexMap.rows;
	int col = indexMap.cols;
	start = 0;
	end = 0;
	int i = 0;
	for (i = 0; i < col / 2; i++)
	{
		cv::Mat temp = indexMap.colRange(i, i + 1);
		int len = 0;
		len = findMapNegNum(temp);
		if (len > 10 /*col * 0.03*/)
		{
			continue;
		}
		else
		{
			break;
		}
	}
	start = i;
	for (i = col; i > col / 2; i--)
	{
		cv::Mat temp = indexMap.colRange(i - 1, i);
		int len = findMapNegNum(temp);
		if (len > 10 /*col * 0.03*/)
		{
			continue;
		}
		else
		{
			break;
		}
	}
	end = i;
	return flag;
}
bool MLImageDetection::MLCherkerboardDetect::searchRowIndex(cv::Mat indexMap, int& start, int& end)
{
	bool flag = true;
	int row = indexMap.rows;
	int col = indexMap.cols;
	start = 0;
	end = 0;
	int i = 0;
	for (i = 0; i < row / 2; i++)
	{
		cv::Mat temp = indexMap.rowRange(i, i + 1);
		int len = 0;
		len = findMapNegNum(temp);
		if (len > 10 /*row * 0.03*/)
		{
			continue;
		}
		else
		{
			break;
		}
	}
	start = i;
	for (i = row; i > row / 2; i--)
	{
		cv::Mat temp = indexMap.rowRange(i - 1, i);
		int len = findMapNegNum(temp);
		if (len > 10 /*row * 0.03*/)
		{
			continue;
		}
		else
		{
			break;
		}
	}
	end = i;
	return flag;
}
cv::Mat MLImageDetection::MLCherkerboardDetect::updateIndexMap(cv::Mat indexMap, vector<cv::Point2f> cornersPoint,
	vector<cv::Point2f>& cornersPointNew)
{
	int row = indexMap.rows;
	int col = indexMap.cols;
	cornersPointNew = cornersPoint;
	vector<cv::Point> negPos;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			int temp = indexMap.at<short>(i, j);
			if (temp < 0)
			{
				negPos.push_back(cv::Point(i, j));
			}
		}
	}
	vector<cv::Point> failUpdatePos = negPos;
	while (failUpdatePos.size() > 0)
	{
		negPos.clear();
		negPos = failUpdatePos;
		failUpdatePos.clear();
		for (int i = 0; i < negPos.size(); i++)
		{
			int x = negPos[i].x;
			int y = negPos[i].y;
			int left = -1, right = -1, top = -1, bottom = -1;
			if (x - 1 >= 0)
			{
				left = indexMap.at<short>(x - 1, y);
			}
			if (x + 1 < row)
			{
				right = indexMap.at<short>(x + 1, y);
			}
			if (y - 1 >= 0)
			{
				top = indexMap.at<short>(x, y - 1);
			}
			if (y + 1 < col)
			{
				bottom = indexMap.at<short>(x, y + 1);
			}
			int flag = 0;
			//	cout << x << ',' << y << endl;
			//	cout << left << ',' << right << ',' << top << ',' << bottom << endl;
			int size = cornersPointNew.size();
			if (left >= 0 & left < size & right >= 0 & right < size)
			{
				flag = 1;
			}
			else if (top >= 0 & top < size & bottom >= 0 & bottom < size)
			{
				flag = 2;
			}
			else if (top >= 0 & top < size & right >= 0 & right < size)
			{
				flag = 3;
			}
			else if (top >= 0 & top < size & left >= 0 & left < size)
			{
				flag = 4;
			}
			else if (bottom >= 0 & bottom < size & right >= 0 & right < size)
			{
				flag = 5;
			}
			else if (bottom >= 0 & bottom < size & left >= 0 & left < size)
			{
				flag = 6;
			}
			else
			{
				failUpdatePos.push_back(negPos[i]);
			}
			// cout << flag << endl;
			switch (flag)
			{
			case 1: {
				cv::Point2f pt;
				pt.x = (cornersPointNew[left].x + cornersPointNew[right].x) / 2.0;
				pt.y = (cornersPointNew[left].y + cornersPointNew[right].y) / 2.0;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break;
			}
			case 2: {
				cv::Point2f pt;
				pt.x = (cornersPointNew[top].x + cornersPointNew[bottom].x) / 2.0;
				pt.y = (cornersPointNew[top].y + cornersPointNew[bottom].y) / 2.0;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break;
			}
			case 3: {
				cv::Point2f pt;
				pt.x = cornersPointNew[top].x;
				pt.y = cornersPointNew[right].y;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break;
			}
			case 4: {
				cv::Point2f pt;
				pt.x = cornersPointNew[top].x;
				pt.y = cornersPointNew[left].y;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break;
			}
			case 5: {
				cv::Point2f pt;
				pt.x = cornersPointNew[bottom].x;
				pt.y = cornersPointNew[right].y;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break;
			}
			case 6: {
				cv::Point2f pt;
				pt.x = cornersPointNew[bottom].x;
				pt.y = cornersPointNew[left].y;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break;
			}
			}
		}
	}

	return indexMap;
}
cv::Mat MLImageDetection::MLCherkerboardDetect::updateIndexMapKL(cv::Mat indexMap, vector<cv::Point2f> cornersPoint,
	vector<cv::Point2f>& cornersPointNew)
{
	cv::Mat a = indexMap.clone();
	a.setTo(0, a < 0);
	int num = cv::countNonZero(a);
	if (num == a.total() - 1 && cornersPoint.size() == a.total())
	{
		cornersPointNew = cornersPoint;
		return indexMap;
	}
	//if()
	if (cornersPoint.size() > 0)
	{
		int row = indexMap.rows;
		int col = indexMap.cols;
		cornersPointNew = cornersPoint;
		vector<cv::Point> negPos;
		double width = 0;
		for (int i = 0; i < row - 1; i++)
		{
			for (int j = 0; j < col - 1; j++)
			{
				int temp = indexMap.at<short>(i, j);
				int temp1 = indexMap.at<short>(i, j + 1);
				if (temp > 0 & temp1 > 0)
				{
					cv::Point2f d = (cornersPoint[temp1] - cornersPoint[temp]);
					width = sqrt(d.x * d.x + d.y * d.y);
					break;
				}
			}
			if (width > 0)
				break;
		}

		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				int temp = indexMap.at<short>(i, j);
				if (temp < 0)
				{
					negPos.push_back(cv::Point(i, j));
				}
			}
		}
		vector<cv::Point> failUpdatePos = negPos;
		while (failUpdatePos.size() > 0 & width > 10)
		{
			negPos.clear();
			negPos = failUpdatePos;
			failUpdatePos.clear();
			for (int i = 0; i < negPos.size(); i++)
			{
				int x = negPos[i].x;
				int y = negPos[i].y;
				int left = -1, right = -1, top = -1, bottom = -1;
				if (x - 1 >= 0)
				{
					top = indexMap.at<short>(x - 1, y);
				}
				if (x + 1 < row)
				{
					bottom = indexMap.at<short>(x + 1, y);
				}
				if (y - 1 >= 0)
				{
					left = indexMap.at<short>(x, y - 1);
				}
				if (y + 1 < col)
				{
					right = indexMap.at<short>(x, y + 1);
				}
				int flag = 0;
				//	cout << x << ',' << y << endl;
				//	cout << left << ',' << right << ',' << top << ',' << bottom << endl;
				int size = cornersPointNew.size();
				if (left >= 0 & left < size & right >= 0 & right < size)
				{
					flag = 1;
				}
				else if (top >= 0 & top < size & bottom >= 0 & bottom < size)
				{
					flag = 2;
				}
				else if (top >= 0 & top < size & right >= 0 & right < size)
				{
					flag = 3;
				}
				else if (top >= 0 & top < size & left >= 0 & left < size)
				{
					flag = 4;
				}
				else if (bottom >= 0 & bottom < size & right >= 0 & right < size)
				{
					flag = 5;
				}
				else if (bottom >= 0 & bottom < size & left >= 0 & left < size)
				{
					flag = 6;
				}
				else if (left >= 0 & left < size)
				{
					flag = 7;
				}
				else if (right >= 0 & right < size)
				{
					flag = 8;
				}
				else if (top >= 0 & top < size)
				{
					flag = 9;
				}
				else if (bottom >= 0 & bottom < size)
				{
					flag = 10;
				}
				else
				{
					failUpdatePos.push_back(negPos[i]);
				}
				// cout << flag << endl;
				switch (flag)
				{
				case 1: {
					cv::Point2f pt;
					pt.x = (cornersPointNew[left].x + cornersPointNew[right].x) / 2.0;
					pt.y = (cornersPointNew[left].y + cornersPointNew[right].y) / 2.0;
					cornersPointNew.push_back(pt);
					indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
					break;
				}
				case 2: {
					cv::Point2f pt;
					pt.x = (cornersPointNew[top].x + cornersPointNew[bottom].x) / 2.0;
					pt.y = (cornersPointNew[top].y + cornersPointNew[bottom].y) / 2.0;
					cornersPointNew.push_back(pt);
					indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
					break;
				}
				case 3: {
					cv::Point2f pt;
					pt.x = cornersPointNew[top].x;
					pt.y = cornersPointNew[right].y;
					cornersPointNew.push_back(pt);
					indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
					break;
				}
				case 4: {
					cv::Point2f pt;
					pt.x = cornersPointNew[top].x;
					pt.y = cornersPointNew[left].y;
					cornersPointNew.push_back(pt);
					indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
					break;
				}
				case 5: {
					cv::Point2f pt;
					pt.x = cornersPointNew[bottom].x;
					pt.y = cornersPointNew[right].y;
					cornersPointNew.push_back(pt);
					indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
					break;
				}
				case 6: {
					cv::Point2f pt;
					pt.x = cornersPointNew[bottom].x;
					pt.y = cornersPointNew[left].y;
					cornersPointNew.push_back(pt);
					indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
					break;
				}
				case 7: {
					cv::Point2f pt;
					pt.x = cornersPointNew[left].x + width;
					pt.y = cornersPointNew[left].y;
					cornersPointNew.push_back(pt);
					indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
					break;
				}
				case 8: {
					cv::Point2f pt;
					pt.x = cornersPointNew[right].x - width;
					pt.y = cornersPointNew[right].y;
					cornersPointNew.push_back(pt);
					indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
					break;
				}
				case 9: {
					cv::Point2f pt;
					pt.x = cornersPointNew[top].x;
					pt.y = cornersPointNew[top].y + width;
					cornersPointNew.push_back(pt);
					indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
					break;
				}
				case 10: {
					cv::Point2f pt;
					pt.x = cornersPointNew[bottom].x;
					pt.y = cornersPointNew[bottom].y - width;
					cornersPointNew.push_back(pt);
					indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
					break;
				}
				}
			}
		}
	}

	return indexMap;
}
cv::RotatedRect MLImageDetection::MLCherkerboardDetect::getCherkerBorder(cv::Mat img, cv::Rect& rect)
{
	cv::RotatedRect rectR;
	if (img.data != NULL)
	{
		cv::Mat gray = convertToGrayImage(img);
		cv::Mat imgdraw = convertTo3Channels(img);
		//cv::Mat preImg = preProcess(gray);
		cv::Mat srcbinary;
		threshold(gray, srcbinary, 0, 255, THRESH_OTSU);
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
			int binNum = 1;// IQMetricUtl::instance()->getBinNum(img.size());
			double powbin = pow(binNum, 2);
			rect = cv::boundingRect(contours[i]);
			double maxLen = max(rect.width, rect.height);
			double minLen = min(rect.width, rect.height);
			double ratio = minLen / maxLen;
			if (area > 2e5 /*&& ratio > 0.8 && ratio < 1.2*/)
			{
				rectR = cv::minAreaRect(contours[i]);
				// center = rectR.center;
				cv::rectangle(imgdraw, rect, Scalar(255, 0, 255), 5);
				break;
			}
		}
	}
	return rectR;
}
bool MLImageDetection::MLCherkerboardDetect::getXYcoordinate(cv::Mat indexMap, vector<cv::Point2f> cornersPoint, cv::Mat& x,
	cv::Mat& y)
{
	bool flag = true;
	int row = indexMap.rows;
	int col = indexMap.cols;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			int list = indexMap.at<short>(i, j);
			if (list > cornersPoint.size())
			{
				// cout << list << endl;
			}
			else
			{
				x.at<float>(i, j) = cornersPoint[list].x;
				y.at<float>(i, j) = cornersPoint[list].y;
			}
		}
	}
	return flag;
}
bool MLImageDetection::MLCherkerboardDetect::getCenter(cv::Mat ip, cv::Mat& p, cv::Mat& px1, cv::Mat& px2, cv::Mat& px3,
	cv::Mat& px4)
{
	bool flag = true;
	int row = ip.rows;
	int col = ip.cols;
	px1 = ip(cv::Range(0, row - 1), cv::Range(0, col - 1));
	px2 = ip(cv::Range(1, row), cv::Range(0, col - 1));
	px3 = ip(cv::Range(0, row - 1), cv::Range(1, col));
	px4 = ip(cv::Range(1, row), cv::Range(1, col));
	cv::Mat temp1, temp2;
	cv::add(px1, px2, temp1);
	cv::add(px3, px4, temp2);
	cv::add(temp1, temp2, p);
	p = p / 4.0;

	// Px1 = iP(1:end - 1, 1 : end - 1);
	// Px2 = iP(2:end, 1 : end - 1);
	// Px3 = iP(1:end - 1, 2 : end);
	// Px4 = iP(2:end, 2 : end);
	// p = (Px1 + Px2 + Px3 + Px4) / 4;
	return flag;
}
cv::Mat MLImageDetection::MLCherkerboardDetect::getArea(cv::Mat x1, cv::Mat x2, cv::Mat x3, cv::Mat x4, cv::Mat y1, cv::Mat y2,
	cv::Mat y3, cv::Mat y4)
{
	/*   s1 = 1 / 2 * sqrt(((x2 - x1). ^ 2 + (y2 - y1). ^ 2).*((x3 - x1). ^ 2 + (y3 - y1). ^ 2) - ((x2 - x1).*(x3 - x1)
	   + (y2 - y1).*(y3 - y1)). ^ 2); s2 = 1 / 2 * sqrt(((x2 - x4). ^ 2 + (y2 - y4). ^ 2).*((x3 - x4). ^ 2 + (y3 - y4).
	   ^ 2) - ((x2 - x4).*(x3 - x4) + (y2 - y4).*(y3 - y4)). ^ 2); s = s1 + s2;*/
	cv::Mat s1, s2, s;
	cv::Mat temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10;
	cv::subtract(x2, x1, temp1);
	cv::subtract(y2, y1, temp2);
	cv::subtract(x3, x1, temp3);
	cv::subtract(y3, y1, temp4);
	cv::multiply(temp1, temp1, temp5);
	cv::multiply(temp2, temp2, temp6);
	cv::multiply(temp3, temp3, temp7);
	cv::multiply(temp4, temp4, temp8);
	cv::multiply(temp1, temp3, temp9);
	cv::multiply(temp2, temp4, temp10);

	cv::add(temp5, temp6, temp6);
	cv::add(temp7, temp8, temp7);
	cv::multiply(temp6, temp7, temp6);

	cv::add(temp9, temp10, temp9);
	cv::multiply(temp9, temp9, temp9);

	cv::subtract(temp6, temp9, temp6);

	cv::sqrt(temp6, s1);
	s1 = s1 * 0.5;

	//  s2 = 1 / 2 * sqrt(((x2 - x4). ^ 2 + (y2 - y4). ^ 2).*((x3 - x4). ^ 2 + (y3 - y4). ^ 2)
	//- ((x2 - x4).*(x3 - x4) + (y2 - y4).*(y3 - y4)). ^ 2);

	cv::subtract(x2, x4, temp1);
	cv::subtract(y2, y4, temp2);
	cv::subtract(x3, x4, temp3);
	cv::subtract(y3, y4, temp4);
	cv::multiply(temp1, temp1, temp5);
	cv::multiply(temp2, temp2, temp6);
	cv::multiply(temp3, temp3, temp7);
	cv::multiply(temp4, temp4, temp8);
	cv::multiply(temp1, temp3, temp9);
	cv::multiply(temp2, temp4, temp10);
	cv::add(temp5, temp6, temp5);
	cv::add(temp7, temp8, temp7);
	cv::multiply(temp5, temp7, temp5);
	cv::add(temp9, temp10, temp9);
	cv::multiply(temp9, temp9, temp9);
	cv::subtract(temp5, temp9, temp5);
	cv::sqrt(temp5, s2);
	s2 = s2 * 0.5;
	cv::add(s1, s2, s);
	return s;
}
bool MLImageDetection::MLCherkerboardDetect::ReoderROIVec(vector<cv::Rect> roiVecP, vector<cv::Rect> roiVecN,
	vector<cv::Rect>& NewroiVecP, vector<cv::Rect>& NewroiVecN)
{

	bool flag = true;
	int len = min(roiVecN.size(), roiVecP.size());
	vector<cv::Rect> roiVecS, roivecB;
	if (roiVecN.size() == len)
	{
		roiVecS = roiVecN;
		roivecB = roiVecP;
	}
	else if (roiVecP.size() == len)
	{
		roiVecS = roiVecP;
		roivecB = roiVecN;
	}

	for (int i = 0; i < roiVecS.size(); i++)
	{
		cv::Rect temp = roiVecS[i];
		if (temp.width > 5 & temp.height > 5)
		{
			int list = findNearstList(roivecB, temp);
			if (list >= 0 & list < roivecB.size())
			{
				NewroiVecP.push_back(roiVecS[i]);
				NewroiVecN.push_back(roivecB[list]);
			}
		}
	}
	if ((NewroiVecN.size() != NewroiVecP.size()) || NewroiVecN.size() == 0 || NewroiVecP.size() == 0)
	{
		flag = false;
	}
	return flag;
}
int MLImageDetection::MLCherkerboardDetect::findNearstList(vector<cv::Rect> roiVecN, cv::Rect temp)
{
	int list = -1;
	vector<double> dis;
	for (int i = 0; i < roiVecN.size(); i++)
	{
		double deltx = abs(roiVecN[i].x - temp.x);
		double delty = abs(roiVecN[i].y - temp.y);
		double tmpDis = sqrt(deltx * deltx + delty * delty);
		dis.push_back(tmpDis);
	}
	if (dis.size() > 0)
	{
		double minValue = *min_element(dis.begin(), dis.end());
		int minPosition = min_element(dis.begin(), dis.end()) - dis.begin();
		if (minValue < 60)
		{
			list = minPosition;
		}
	}
	return list;
}
std::vector<double> MLImageDetection::MLCherkerboardDetect::CalculateROIValue(cv::Mat img, vector<cv::Rect> roiVec)
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
std::vector<double> MLImageDetection::MLCherkerboardDetect::CalculateCR(vector<double> posValue, vector<double> negValue)
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

void MLImageDetection::MLCherkerboardDetect::updateImgdraw(cv::Mat& imgdraw, vector<cv::Point2f> pts)
{
	for (int i = 0; i < pts.size(); i++)
	{
		circle(imgdraw, pts[i], 5, Scalar(0, 0, 255), -1);
	}
}
cv::Point2f MLImageDetection::MLCherkerboardDetect::getPtsCenter(vector<cv::Point2f>pts)
{
	cv::Point2f c0;
	arma::vec xv(pts.size());
	arma::vec yv(pts.size());
	for (int i = 0; i < pts.size(); i++)
	{
		xv[i] = pts[i].x;
		yv[i] = pts[i].y;
	}
	c0.x = arma::mean(xv);
	c0.y = arma::mean(yv);
	return c0;
}

void MLImageDetection::MLCherkerboardDetect::SetCRAreaPercent(double percent)
{
	m_CRAreaPercent = percent;
}
void MLImageDetection::MLCherkerboardDetect::SetChecssboardPointsClusters(double thresh)
{
	m_pointsClusters = thresh;
}
void MLImageDetection::MLCherkerboardDetect::SetChessboardxyClassification(double thresh)
{
	m_xyClassification = thresh;
}
void MLImageDetection::MLCherkerboardDetect::SetChessboardUpdateFlag(bool flag)
{
	m_update = flag;
}
void MLImageDetection::MLCherkerboardDetect::SetRotationAngle(double angle)
{
	m_rotationAngle = angle;
}
void MLImageDetection::MLCherkerboardDetect::SetChessboardSize(cv::Size size)
{
	m_boardsize = size;
}






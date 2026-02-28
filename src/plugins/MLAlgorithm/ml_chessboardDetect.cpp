#include "ml_chessboardDetect.h"
#include "FiducialDetect.h"
#include "libcbdetect/boards_from_corners.h"
#include "libcbdetect/find_corners.h"
#include "ml_BMWfiducialDetect.h"
#include "ml_gridDetect.h"
#include <algorithm>
#include <color-util/XYZ_to_Lab.hpp>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <numeric>
#include <opencv2/imgproc/types_c.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\opencv.hpp>

using namespace algorithm;
using namespace std;

algorithm::MLOptParaChessboard::MLOptParaChessboard()
{
}
algorithm::MLOptParaChessboard::~MLOptParaChessboard()
{
}
ChessboardcrRe algorithm::MLOptParaChessboard::ContrastChessboard(cv::Mat imgP, cv::Mat imgN)
{
	ChessboardcrRe re;
	m_ChessboardCR = re;
	double crValue = 0;
	if (imgP.data != NULL & imgN.data != NULL)
	{
		m_img_drawN.release();
		m_img_drawP.release();
		if (m_rotationAngle == 0)
		{
			cv::RotatedRect rectR;
			cv::Rect rect = getChessboardRect(imgP, rectR);
			imgP = getRotationImg(imgP, rectR.angle);
			imgN = getRotationImg(imgN, rectR.angle);
		}
		double rotationAngle = HydrusParameters::roationAngle;
		imgP = getRotationImg(imgP, rotationAngle);
		imgN = getRotationImg(imgN, rotationAngle);
		imgP = getRotationImg(imgP, m_rotationAngle);
		imgN = getRotationImg(imgN, m_rotationAngle);

		cv::Mat posROI_gray = GetROIMat(imgP, m_rect);
		cv::Mat negROI_gray = GetROIMat(imgN, m_rect);
		vector<cv::Point2f> pointsP, pointsN;
		vector<cv::Rect> roiVecP, roiVecN, NewroiVecP, NewroiVecN;

		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);
		m_img_drawP = img_drawP.clone();
		m_img_drawN = img_drawN.clone();

		vector<cv::Point2f> corners;
		cv::Mat indexMap;
		vector<cv::Point2f> cornersP, cornersN;
		cv::Mat indexMapP, indexMapN;

		int binNum = sqrt(7920 * 6004 / imgP.total());
		if (binNum == 1)
		{
			cv::resize(img_drawP, img_drawP, img_drawP.size() / 4);
			cv::resize(img_drawN, img_drawN, img_drawN.size() / 4);
		}

		cornersP = detectChessboardContour(img_drawP, indexMapP, 4);
		cornersN = detectChessboardContour(img_drawN, indexMapN);

		if (cornersP.size() != cornersN.size() || cornersN.size() <= 3)
		{
			cornersP = detectChessboardCorner(img_drawP, indexMapP);
			cornersN = detectChessboardCorner(img_drawN, indexMapN);
		}
		if (binNum == 1)
		{
			for (int i = 0; i < cornersP.size(); i++)
			{
				cornersP[i] = cornersP[i] * 4;
				//circle(img_drawP, cornersP[i], 10, Scalar(255, 0, 255), -1);
			}
			cv::resize(img_drawP, img_drawP, img_drawP.size() * 4);
			cv::resize(img_drawN, img_drawN, img_drawN.size() * 4);

		}

		// cornersN = cornersP;
		// indexMapN = indexMapP;


		m_boardsize = indexMapP.size();
		string dir, filepathN, filepathP;
		if (DEBUG == 1)
		{
			dir = "E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\ROIInfo\\";
		}
		else if (DEBUG == 0)
		{
			string dir = "./config/ROIInfo/";
		}
		if (m_boardsize.width == 5)
		{
			filepathN = dir + "roiVecNSmall.txt";
			filepathP = dir + "roiVecPSmall.txt";
		}
		else if (m_boardsize.width == 8)
		{
			filepathN = dir + "roiVecNBig.txt";
			filepathP = dir + "roiVecPBig.txt";
		}

		roiVecP = getCRROI(cornersP, indexMapP);
		roiVecN = getCRROI(cornersN, indexMapN); // roiVecP;
		if (cornersP.size() != indexMapP.rows * indexMapP.cols || cornersN.size() != indexMapN.rows * indexMapN.cols ||
			cornersP.size() != cornersN.size())
		{
			roiVecP.clear();
			roiVecN.clear();
			readROItxtFile(filepathP, roiVecP);
			readROItxtFile(filepathN, roiVecN);
			// return m_ChessboardCR;
		}

		m_ChessboardCR.boardSize = indexMapP.size();
		// old intreface
		// roiVecP = detectChessboard(img_drawP);
		// roiVecN = detectChessboard(img_drawN);

		getNewROIRect(roiVecP, roiVecN, NewroiVecP, NewroiVecN);

		if (NewroiVecN.size() < 1 || NewroiVecP.size() < 1)
		{
			readROItxtFile(filepathP, NewroiVecP);
			readROItxtFile(filepathN, NewroiVecN);
			// return m_ChessboardCR;
		}

		writeROIFile(filepathN, NewroiVecN);
		writeROIFile(filepathP, NewroiVecP);

		//
		double k = HydrusParameters::kET;
		double b = HydrusParameters::bET;
		double darkLevel = 0;
		if (m_exposureTime > 0)
		{
			darkLevel = k * m_exposureTime + b;
		}
		posROI_gray = posROI_gray - darkLevel;
		negROI_gray = negROI_gray - darkLevel;
		vector<double> posValue, negValue;
		posValue = CalculateROIValue(posROI_gray, NewroiVecN);
		negValue = CalculateROIValue(negROI_gray, NewroiVecP);
		if (posValue.size() < 1 || negValue.size() < 1)
			return m_ChessboardCR;
		if (posValue.size() != negValue.size())
			return m_ChessboardCR;
		vector<double> crVec = CalculateCR(posValue, negValue);
		// draw
		if (NewroiVecP.size() == NewroiVecN.size())
		{
			for (int i = 0; i < NewroiVecP.size(); i++)
			{
				// draw
				cv::rectangle(img_drawP, NewroiVecN[i], cv::Scalar(255, 0, 255), 1);
				cv::rectangle(img_drawN, NewroiVecP[i], cv::Scalar(255, 0, 255), 1);
			}
		}
		m_img_drawP = img_drawP.clone();
		m_img_drawN = img_drawN.clone();
		//  m_boardsize = indexMapP.size();
		cv::Size boardSize = cv::Size(m_boardsize.width - 1, m_boardsize.height - 1);
		cv::Mat crAll = cv::Mat::zeros(boardSize, CV_32FC1);
		cv::Mat grayLevelP = cv::Mat::zeros(boardSize, CV_32FC1);
		cv::Mat grayLevelN = cv::Mat::zeros(boardSize, CV_32FC1);
		if ((boardSize.height) * (boardSize.width) != posValue.size())
			return m_ChessboardCR;
		int i = 0;
		for (int n = 0; n < boardSize.height; n++)
		{
			for (int m = 0; m < boardSize.width; m++)
			{
				crAll.at<float>(n, m) = crVec[i];
				grayLevelP.at<float>(n, m) = posValue[i];
				grayLevelN.at<float>(n, m) = negValue[i];
				i++;
			}
		}

		if (crVec.size() > 0)
		{
			crValue = accumulate(crVec.begin(), crVec.end(), 0.0) / crVec.size();
		}
		re.CRAll = crAll;
		re.cr = crValue;
		re.greyLevelN = grayLevelN;
		re.greyLevelP = grayLevelP;
		m_ChessboardCR = re;
	}
	return re;
}
cv::Rect algorithm::MLOptParaChessboard::getChessboardRect(cv::Mat roi, cv::RotatedRect& rectR)
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
			img_gray1 = convertToUint8(roi);
			img_draw = convertToUint8(roi);
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
		cv::findContours(srcbinary, contours, hierarchy, RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		vector<double> area_sort;
		vector<int> list;
		cv::Mat roi;
		vector<double> areaVec;
		for (int i = 0; i < contours.size(); ++i)
		{
			double area = contourArea(contours[i], false);
			// cout << area << endl;
			if (area > 3e3) //&& area < 4.2e6 / powbin)
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
			rectR = cv::minAreaRect(contoursNew[maxPos]);
			roi = img_draw(rect);
		}
	}
	return rect;
}

vector<cv::Point2f> algorithm::MLOptParaChessboard::detectChessboardCorner(cv::Mat img, cv::Mat& indexMap, int binNum)
{
	vector<cv::Point2f> cornersRe;
	if (img.data != NULL)
	{
		cv::Mat img_gray;
		if (img.channels() == 3)
			cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
		else
			img_gray = img.clone();
		img_gray = convertToUint8(img_gray);
		cv::Mat img_draw = img_gray.clone();
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
		Mat srcbinary = roi.clone();
		threshold(roi, srcbinary, 0, 255, THRESH_OTSU);
		// adaptiveThreshold(roi, srcbinary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 17, 0);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(80, 80), Point(-1, -1));
		//   morphologyEx(srcbinary, srcbinary, MORPH_OPEN, kernel, Point(-1, -1));
		Mat kernel1 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
		morphologyEx(srcbinary, srcbinary, MORPH_CLOSE, kernel1, Point(-1, -1));
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		cv::dilate(srcbinary, srcbinary, element);
		// cv::morphologyEx(srcbinary, srcbinary, MORPH_CLOSE, kernel, Point(-1, -1));

		// morphologyEx(srcbinary, srcbinary, MORPH_DILATE, kernel, Point(-1, -1));
		cv::Mat roiimg = roi.clone(); // srcbinary.clone();
		vector<double> x, y;
		std::vector<cv::Point2f> cornersPoint;
		if (roiimg.data)
		{
			cbdetect::find_corners(roiimg, corners, params);
			// cbdetect::plot_corners(img_draw, corners);
			if (corners.p.size() > 0)
			{
				cbdetect::boards_from_corners(roiimg, corners, chessboard, params);
				//   cbdetect::plot_boards(img_draw, corners, chessboard, params);
			}
			//cornersPoint = corners.p;
			for (int j = 0; j < corners.p.size(); j++)
			{
				cv::Point2f pt;
				pt.x = corners.p.at(j).x; // +rectGrid.x;
				pt.y = corners.p.at(j).y; // +rectGrid.y;
				circle(img_draw, pt, 10, cv::Scalar(255, 0, 255), -1);
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
				indexMap = indexMap(cv::Range(0, indexMap.rows), cv::Range(0, indexMap.cols));
				int left = 0, right = 0;
				int top = 0, bottom = 0;
				// searchColIndex(indexMap, left, right);
				// indexMap = indexMap(cv::Range(0, indexMap.rows), cv::Range(left, right));
				// searchRowIndex(indexMap, top, bottom);

				//   transpose(indexMap, indexMap);
				//  rotate(indexMap, indexMap, ROTATE_90_CLOCKWISE);
				rotate(indexMap, indexMap, ROTATE_90_COUNTERCLOCKWISE);

				vector<cv::Point2f> cornersPointNew;
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
				// m_ChessboardCR.boardSize = indexMapUpdate.size();
				m_boardsize = indexMapUpdate.size();
				indexMap = indexMapUpdate;
				cornersRe = cornersPointNew;
				// draw
				for (int i = 0; i < cornersPointNew.size(); i++)
				{
					cv::circle(img_draw, cornersPointNew[i], 5, cv::Scalar(255, 0, 255), -1);
				}
			}
		}
		m_img_draw = img_draw.clone();
	}
	return cornersRe;
}

vector<cv::Point2f> algorithm::MLOptParaChessboard::detectChessboardHoughLine(cv::Mat img, cv::Mat& indexMap)
{
	vector<cv::Point2f> corners;
	if (img.data != NULL)
	{
		cv::Mat img_draw = convertToUint8(img);
		cv::RotatedRect rectR;
		cv::Rect rectGrid;//= getChessboardRect(img_draw, rectR);
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
		corners = grid.GridHoughLine(roi, indexMap, m_boardsize);
		for (int i = 0; i < corners.size(); i++)
		{
			corners[i].x = corners[i].x + rectGrid.x;
			corners[i].y = corners[i].y + rectGrid.y;
			circle(img_draw, corners[i], 5, cv::Scalar(255, 0, 255), -1);
		}
	}

	return corners;
}
vector<cv::Point2f> algorithm::MLOptParaChessboard::detectChessboardTemplate(cv::Mat img, cv::Mat& indexMap)
{
	vector<cv::Point2f> corners;
	if (img.data != NULL)
	{
		cv::Mat img_draw = convertToUint8(img);
		cv::RotatedRect rectR;
		cv::Rect rect; // = getChessboardRect(img_draw,rectR);
		cv::Mat roi = GetROIMat(img_draw, rect);

		if (rect.width < 0)
		{
			rect.x = 0;
			rect.y = 0;
		}
		cv::Point2f start = cv::Point2f(rect.x, rect.y);
		cv::Mat img_draw1 = roi.clone();
		cv::Mat templ = cv::imread("./config\\templateImg\\chessboardtemplate1.tif", 0);
		cv::Mat templ1 = cv::imread("./config\\templateImg\\chessboardtemplate2.tif", 0);
		// 自动检测中心坐标
		// MLBMWFiducialDetect bwm;
		// bwm.BMWHoughLineDetect(templ);
		// bwm.BMWHoughLineDetect(templ1);
		//模板定，坐标定 
		cv::Point c0 = cv::Point(79, 96);
		cv::Point c1 = cv::Point(83, 99);
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
		cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, true, 50);
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

		m_img_draw = img_draw.clone();

		// cv::Mat img_result, img_result1, img_result2;
		// matchTemplate(roi, templ, img_result, TM_CCOEFF_NORMED);
		// matchTemplate(roi, templ1, img_result1, TM_CCOEFF_NORMED);
		// cv::Mat imgre, imgre1;
		// threshold(img_result, imgre, 0.95, 255, CV_THRESH_BINARY);
		// threshold(img_result, imgre1, 0.95, 255, CV_THRESH_BINARY);
		// vector<cv::Point2f>pts1;
		// findNonZero(imgre, pts);
		// findNonZero(imgre, pts1);
		// vector<cv::Point2f>ptsNew1;
		// ptsNew = pointsClusters(pts, 25);
		// ptsNew1 = pointsClusters(pts, 25);
		// for (int i = 0; i < ptsNew1.size(); i++)
		//{
		//    ptsNew.push_back(ptsNew1[i]);
		//}

		// for (int i = 0; i < ptsNew.size(); i++)
		//{
		//    circle(img_draw1, ptsNew[i], 5, Scalar(255, 0, 255), -1);
		//}
	}
	return corners;
}
vector<cv::Point2f> algorithm::MLOptParaChessboard::detectChessboardContour(cv::Mat img, cv::Mat& indexMap, int binNum)
{
	vector<cv::Point2f> corners;
	if (img.data != NULL)
	{
		cv::Mat img_draw = convertToUint8(img);
		cv::RotatedRect rectR;
		cv::Rect rect;
		//= getChessboardRect(img_draw, rectR);
		cv::Mat roi = GetROIMat(img_draw, rect);
		Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
		clahe->apply(roi, roi);
		if (rect.width < 0)
		{
			rect.x = 0;
			rect.y = 0;
		}
		cv::Point2f start = cv::Point2f(rect.x, rect.y);
		// zhao chu lunkuo dian
		cv::Mat img_draw1 = roi.clone();
		cv::Mat srcbinary;
		cv::threshold(roi, srcbinary, 0, 255, CV_THRESH_OTSU);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(80, 80), Point(-1, -1));
		//  morphologyEx(srcbinary, srcbinary, MORPH_OPEN, kernel, Point(-1, -1));
		// cv::GaussianBlur(roi, roi, cv::Size(11,11), 0, 0);
		// adaptiveThreshold(roi, srcbinary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 17, 0);
		// NaiveRemoveNoise(srcbinary, 1000);
		// Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
		// morphologyEx(srcbinary, srcbinary, MORPH_OPEN, kernel, Point(-1, -1));
		// the last code
		int bin = binNum;
		Mat kernel1 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
		cv::erode(srcbinary, srcbinary, kernel1);
		// cv::dilate(srcbinary, srcbinary, kernel1);

		cv::Mat srccanny;
		Canny(srcbinary, srccanny, 30, 90);
		vector<vector<Point>> contours;
		vector<Vec4i> hierachy;
		findContours(srcbinary, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		vector<Vec4i> hierachy_new;
		vector<cv::Point2f> pts;
		for (int i = 0; i < contours.size(); i++)
		{
			double area = contourArea(contours[i], false);
			if (area > 1e4 / pow(bin, 2) & area < 1e6 / pow(bin, 2))
			{
				drawContours(img_draw1, contours, i, Scalar(rand() * 255, rand() * 255, rand() * 255), 3, 8);
				RotatedRect rect = minAreaRect(contours[i]);
				cv::Point2f P[4];
				rect.points(P);
				double w = Getdistance(P[0], P[1]); // abs(P[0].x - P[2].x);
				double h = Getdistance(P[0], P[3]); // abs(P[0].y - P[2].y);
				double asp = max(w, h) / min(w, h);
				// TODO:config
				int singlecheckerWidth = HydrusParameters::singlecheckerWidth;
				if (w > singlecheckerWidth / bin /*& w < 200*/ & asp > 0.9 & asp < 1.2)
				{
					for (int j = 0; j <= 3; j++)
					{
						line(img_draw1, P[j], P[(j + 1) % 4], Scalar(0, 0, 255), 2);
					}
					pts.push_back(P[0]);
					pts.push_back(P[1]);
					pts.push_back(P[2]);
					pts.push_back(P[3]);
				}
			}
		}
		// draw
		for (int i = 0; i < pts.size(); i++)
		{
			circle(img_draw1, pts[i], 5, Scalar(255, 0, 255), -1);
		}

		// dian fen lei
		int thred1 = HydrusParameters::pointsClusters;
		vector<cv::Point2f> ptsNew = pointsClusters(pts, thred1 / bin);
		int thred2 = HydrusParameters::xyClassification;
		cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, false, thred2 / bin);
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
				if (in > -1)
					circle(img_draw, corners[in], 10, Scalar(255, 0, 255), -1);
			}
		}

		m_img_draw = img_draw.clone();
	}
	return corners;
}
vector<cv::Point2f> algorithm::MLOptParaChessboard::detectChessboardTemplate1(cv::Mat img, cv::Mat& indexMap)
{
	vector<cv::Point2f> corners;
	if (img.data != NULL)
	{
		cv::Mat img_draw = convertToUint8(img);
		cv::RotatedRect rectR;
		cv::Rect rect; // = getChessboardRect(img_draw,rectR);
		cv::Mat roi = GetROIMat(img_draw, rect);

		if (rect.width < 0)
		{
			rect.x = 0;
			rect.y = 0;
		}
		cv::Point2f start = cv::Point2f(rect.x, rect.y);
		cv::Mat img_draw1 = roi.clone();
		cv::Mat templ = cv::imread(
			"E:\\Algorithm\\LN_Algorithm_Libiary\\AlgorithmConfig\\templateImgs\\chessboardtemplate1.tif", 0);
		cv::Mat templ1 = cv::imread(
			"E:\\Algorithm\\LN_Algorithm_Libiary\\AlgorithmConfig\\templateImgs\\chessboardtemplate2.tif", 0);
		// cv::Mat templ = cv::imread("E:\\project\\hydrus2Imgs\\roi1.tif", 0);
		// cv::Mat templ1 = cv::imread("E:\\project\\hydrus2Imgs\\roi1.tif", 0);

		// 自动检测中心坐标
		// MLBMWFiducialDetect bwm;
		// bwm.BMWHoughLineDetect(templ);
		// bwm.BMWHoughLineDetect(templ1);s
		//模板定，坐标定 
		cv::Point c0 = cv::Point(79, 96);
		cv::Point c1 = cv::Point(83, 99);
		vector<cv::Point> pts1 = matchTemplateMaxLocs(roi, templ, 0.5);
		vector<cv::Point> pts2 = matchTemplateMaxLocs(roi, templ1, 0.5);
		vector<cv::Point2f> pts;
		for (int i = 0; i < pts1.size(); i++)
		{
			pts.push_back(pts1[i] + c0);
		}
		for (int i = 0; i < pts2.size(); i++)
		{
			pts.push_back(pts2[i] + c1);
		}
		// draw
		for (int i = 0; i < pts.size(); i++)
		{
			circle(img_draw1, pts[i], 5, Scalar(255, 0, 255), -1);
		}
		vector<cv::Point2f> ptsNew = pointsClusters(pts, 50);
		cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, true, 50);
		MLOptParaChessboard cb;
		indexMap = cb.updateIndexMapKL(indexMap1, ptsNew, corners);
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
				if (in >= 0)
				{
					circle(img_draw, corners[in], 10, Scalar(255, 0, 255), -1);
				}
			}
		}
		m_img_draw = img_draw.clone();
	}
	return corners;
}
vector<cv::Point> algorithm::MLOptParaChessboard::matchTemplateMaxLocs(cv::Mat img, cv::Mat templ, double score)
{

	vector<cv::Point> pts;
	cv::Mat img_draw = img.clone();
	cv::Mat img_result;
	matchTemplate(img, templ, img_result, TM_CCOEFF_NORMED);
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
		if (area > 5e3 & area < 1e5)
		{
			// cout << area << endl;
			drawContours(img_draw, contours, i, Scalar(rand() * 255, rand() * 255, rand() * 255), 3, 8);
			Rect rect = boundingRect(contours[i]);
			rectangle(img_draw, rect, Scalar(255, 0, 255), 2);
			cv::Mat roi1 = img_result(rect);
			double minval, maxval;
			Point minloc, maxloc;
			cv::minMaxLoc(roi1, &minval, &maxval, &minloc, &maxloc);
			pts.push_back(maxloc + cv::Point(rect.x, rect.y));
		}
	}
	for (int i = 0; i < pts.size(); i++)
	{
		circle(img_draw, pts[i], 5, Scalar(255, 0, 255), -1);
	}
	return pts;
}
vector<cv::Rect> algorithm::MLOptParaChessboard::getCRROI(vector<cv::Point2f> corners, cv::Mat indexMap)
{
	vector<cv::Rect> pos;
	if (corners.size() > 0 & indexMap.data != NULL)
	{
		int row = indexMap.rows;
		int col = indexMap.cols;
		cv::Mat xPoints = cv::Mat::zeros(row, col, CV_32F);
		cv::Mat yPoints = cv::Mat::zeros(row, col, CV_32F);
		m_ChessboardCR.boardSize = indexMap.size();
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
		for (int i = 0; i < xcenter.rows; i++)
		{
			for (int j = 0; j < xcenter.cols; j++)
			{
				float x = xcenter.at<float>(i, j);
				float y = ycenter.at<float>(i, j);
				cv::circle(m_img_drawP, cv::Point2f(x, y), 1, cv::Scalar(255, 255, 0), -1);
			}
		}
		cv::Mat boardArea = getArea(px1, px2, px3, px4, py1, py2, py3, py4);
		cv::Mat boardRadius;
		double percent = HydrusParameters::crArea; // 0.8;//crAreaPercent;
		cv::sqrt(boardArea * percent, boardRadius);
		for (int i = 0; i < xcenter.rows; i++)
		{
			for (int j = 0; j < xcenter.cols; j++)
			{
				float x = xcenter.at<float>(i, j) - boardRadius.at<float>(i, j) / 2;
				float y = ycenter.at<float>(i, j) - boardRadius.at<float>(i, j) / 2;
				cv::Point2f start = cv::Point2f(x, y);
				cv::Point2f end = cv::Point2f(x + boardRadius.at<float>(i, j), y + boardRadius.at<float>(i, j));
				// cv::Rect temp = cv::Rect(x, y, boardRadius.at<float>(i, j), boardRadius.at<float>(i, j));
				cv::Rect temp = cv::Rect(start, end);
				cv::rectangle(m_img_drawP, temp, cv::Scalar(255, 0, 255), 1);
				pos.push_back(temp);
			}
		}
		// m_img_drawP = img_draw.clone();
	}
	return pos;
}
bool algorithm::MLOptParaChessboard::getNewROIRect(vector<Rect> roiVecP, vector<Rect> roiVecN, vector<Rect>& NewroiVecP,
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
int algorithm::MLOptParaChessboard::findNegtiveNum(vector<int> list)
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
int algorithm::MLOptParaChessboard::findMapNegNum(cv::Mat list)
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
bool algorithm::MLOptParaChessboard::searchColIndex(cv::Mat indexMap, int& start, int& end)
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
bool algorithm::MLOptParaChessboard::searchRowIndex(cv::Mat indexMap, int& start, int& end)
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
cv::Mat algorithm::MLOptParaChessboard::updateIndexMap(cv::Mat indexMap, vector<cv::Point2f> cornersPoint,
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
cv::Mat algorithm::MLOptParaChessboard::updateIndexMapKL(cv::Mat indexMap, vector<cv::Point2f> cornersPoint,
	vector<cv::Point2f>& cornersPointNew)
{
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
					if (width > 0)
						break;
				}
				if (width > 0)
					break;
			}
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
bool algorithm::MLOptParaChessboard::getXYcoordinate(cv::Mat indexMap, vector<cv::Point2f> cornersPoint, cv::Mat& x,
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
bool algorithm::MLOptParaChessboard::getCenter(cv::Mat ip, cv::Mat& p, cv::Mat& px1, cv::Mat& px2, cv::Mat& px3,
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
cv::Mat algorithm::MLOptParaChessboard::getArea(cv::Mat x1, cv::Mat x2, cv::Mat x3, cv::Mat x4, cv::Mat y1, cv::Mat y2,
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
bool algorithm::MLOptParaChessboard::ReoderROIVec(vector<cv::Rect> roiVecP, vector<cv::Rect> roiVecN,
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
int algorithm::MLOptParaChessboard::findNearstList(vector<cv::Rect> roiVecN, cv::Rect temp)
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
std::vector<double> algorithm::MLOptParaChessboard::CalculateROIValue(cv::Mat img, vector<cv::Rect> roiVec)
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
std::vector<double> algorithm::MLOptParaChessboard::CalculateCR(vector<double> posValue, vector<double> negValue)
{
	vector<double> crVec;
	if (posValue.size() == negValue.size())
	{
		for (int i = 0; i < posValue.size(); i++)
		{
			if (posValue[i] >= negValue[i] & negValue[i] != 0 & negValue[i] > 0)
			{
				crVec.push_back(posValue[i] / negValue[i]);
			}
			else if (negValue[i] >= posValue[i] & posValue[i] != 0 & posValue[i] > 0)
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
cv::Mat algorithm::MLOptParaChessboard::getCheckboardImgRotated(cv::Mat img)
{
	cv::Mat imgR;
	if (img.data != NULL)
	{
		cv::RotatedRect rectR;
		cv::Rect rect = getChessboardRect(img, rectR);
		imgR = getRotationImg(img, rectR.angle);

	}
	return imgR;
}
void algorithm::MLOptParaChessboard::SetChessboardImage(cv::Mat imgP, cv::Mat imgN)
{
	m_imgP = imgP;
	m_imgN = imgN;
	hydrusConfig.GetNewPara(imgP);
	m_para = hydrusConfig.m_para;
	crAreaPercent = HydrusParameters::crArea;
	m_rect = m_para.rect;

	// MLOptParaKunLun kl;s
	//  m_rect = kl.GetNewPara(imgP).rect;
}
void algorithm::MLOptParaChessboard::SetExposureTime(float time)
{
	m_exposureTime = time;
}
void algorithm::MLOptParaChessboard::SetChessboardSize(cv::Size size)
{
	m_boardsize = size;
}
void algorithm::MLOptParaChessboard::SetROIRect(cv::Rect ROIRect)
{
	m_rect = ROIRect;
}
void algorithm::MLOptParaChessboard::SetRotationDeg(float degree)
{
	m_rotationAngle = degree;
}
ChessboardcrRe algorithm::MLOptParaChessboard::GetContrastChessboard()
{
	ChessboardcrRe result;
	result = ContrastChessboard(m_imgP, m_imgN);
	return result;
}
cv::Mat algorithm::MLOptParaChessboard::GetRoiCR()
{
	if (m_ChessboardCR.CRAll.data != NULL)
	{
		return m_ChessboardCR.CRAll;
	}
	else
	{
		ContrastChessboard(m_imgP, m_imgN);
		return m_ChessboardCR.CRAll;
	}
}
double algorithm::MLOptParaChessboard::GetChessboradCr()
{
	return m_ChessboardCR.cr;
}
void algorithm::MLOptParaChessboard::setChessboardCr(ChessboardcrRe re)
{
	m_ChessboardCR = re;
}
cv::Mat algorithm::MLOptParaChessboard::GetGreyLevelP()
{
	if (m_ChessboardCR.greyLevelP.data != NULL)
	{
		return m_ChessboardCR.greyLevelP;
	}
	else
	{
		ContrastChessboard(m_imgP, m_imgN);
		return m_ChessboardCR.greyLevelP;
	}
}
cv::Mat algorithm::MLOptParaChessboard::GetGreyLevelN()
{
	if (m_ChessboardCR.greyLevelN.data != NULL)
	{
		return m_ChessboardCR.greyLevelN;
	}
	else
	{
		ContrastChessboard(m_imgP, m_imgN);
		return m_ChessboardCR.greyLevelN;
	}
}
cv::Mat algorithm::MLOptParaChessboard::getImgDraw()
{
	return m_img_draw;
}
cv::Mat algorithm::MLOptParaChessboard::getImgDrawPos()
{
	return m_img_drawP;
}

cv::Mat algorithm::MLOptParaChessboard::getImgDrawNeg()
{
	return m_img_drawN;
}

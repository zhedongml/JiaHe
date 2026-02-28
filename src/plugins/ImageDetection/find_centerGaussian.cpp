#pragma once
//#include "pch.h"
#include "CrossCenter.h"
#include<armadillo>
//#include <MLAlgorithm/ml_optpara_kunlun.h>
//#include"qlog.h"
using namespace std;
using namespace cv;
using namespace MLImageDetection;
bool findGaussianFitRawData(vector<double> x, vector<double> y, vector<cv::Point2f>& fitRawData)
{
	int len = CrossCenterParameter::searchlength;
	// int len = 5;
	 //int len = 10;// thresh 3
	if (x.size() == y.size() & x.size() > len)
	{
		double max = *max_element(y.begin(), y.end());
		double min = *min_element(y.begin(), y.end());
		int list = 0;
		vector<int> list0;
		for (int i = 0; i < y.size(); i++)
		{
			if (abs(y[i] - max) < 1)// thresh 4 Maximum search
			{
				/*		list = i;
						break;*/
				list0.push_back(i);
			}
		}
		list = int(std::accumulate(list0.begin(), list0.end(), 0.0) / list0.size());
		// vector<cv::Point2f>g_cvRawPointVec;
		int start = list - len / 2;
		int end = list + len / 2;
		if (start > 0 & end < x.size())
		{
			for (int i = start; i < end; i++)
			{
				if (x[i] > 0 & y[i] > 0)
				{
					fitRawData.push_back(cv::Point(x[i], y[i]));
				}
				// cout << cv::Point(x[i], y[i]) << endl;
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;

}
bool gaussiantCurveFit(const vector<cv::Point2f>& cvRawPointVec, int n, cv::Mat& A)
{
	int N = cvRawPointVec.size();
	if (N > 1)
	{
		cv::Mat Y(N, 1, CV_32FC1, cv::Scalar(0));
		cv::Mat X(N, n + 1, CV_32FC1, cv::Scalar(0));
		for (int i = 0; i < N; ++i)
		{
			float* ptr = X.ptr<float>(i);
			for (int j = 0; j < n + 1; ++j)
			{
				ptr[j] = pow(cvRawPointVec[i].x, j);
			}
			Y.at<float>(i, 0) = log(cvRawPointVec[i].y+1);
		}
		//Solve matrix A：X*A = Y
		if (!X.empty() & !Y.empty() & X.rows == Y.rows & X.rows > 3)
		{
			bool flag = solve(X, Y, A, cv::DECOMP_QR);
		}
	}
	return true;
}
double findIndexByGaussianFit(vector<cv::Point2f>gaussFitdata)
{
	double index = -1;
	cv::Mat A(3, 1, CV_32FC1, cv::Scalar(0));
	gaussiantCurveFit(gaussFitdata, 2, A);
	float a0 = A.at<float>(0, 0);
	float a1 = A.at<float>(1, 0);
	float a2 = A.at<float>(2, 0);
	double total = NAN;
	if (abs(a1) > 1e-6)
	{
		float fParmB = -1 / (a2);
		float fParmC = -a1 / (2 * a2);
		float fParmA = std::exp(a0 + fParmC * fParmC / fParmB);
		//  cout << fParmA << ',' << fParmC << ',' << fParmB << endl;
		vector<double> xfit, yfit;
		double dMaxYValue = -1.0;
		double yfitmax = 0;
		double maxX = gaussFitdata[gaussFitdata.size() - 1].x;
		for (double i = 0; i < maxX; i = i + 0.1)
		{
			double x = i;
			double y = 0;
			if (int(fParmA) != 0)
			{
				y = fParmA * exp(-pow(x - fParmC, 2) / fParmB);
			}
			if (y >= dMaxYValue)
			{
				dMaxYValue = y;
			}
			xfit.push_back(x);
			yfit.push_back(y);

		}
		int listLineFit = max_element(yfit.begin(), yfit.end()) - yfit.begin();
		index = xfit[listLineFit];
		return index;
	}
	else
		return -1;
}
double findIndexByPolynominal(vector<cv::Point2f>gaussFitdata,int num=2)
{
	double index = -1;
	arma::vec xvec(gaussFitdata.size()), yvec(gaussFitdata.size());
	for (int i = 0; i < gaussFitdata.size(); i++)
	{
		xvec[i] = gaussFitdata[i].x;
		yvec[i] = gaussFitdata[i].y;
	}
	arma::vec p = arma::polyfit(xvec, yvec, num);
	arma::vec xvecfit = arma::regspace(arma::min(xvec), 0.1, arma::max(xvec));
	arma::vec yvecfit = arma::polyval(p, xvecfit);
	arma::uvec indexvec = arma::find(yvecfit == arma::max(yvecfit));
	index = xvecfit[indexvec[0]];
	return index;
}

vector<int> dataFilter(vector<double> rawdata, double thresh)
{
	vector<int>vecList;
	if (rawdata.size() > 0)
	{
		double mean = std::accumulate(rawdata.begin(), rawdata.end(), 0.0) / rawdata.size();
		for (int i = 0; i < rawdata.size(); i++)
		{
			double temp = abs(rawdata[i] - mean);
			if (temp < thresh)
			{
				vecList.push_back(i);
			}
		}
	}
	return vecList;
}
cv::Vec4f CrossCenter::verticalLineFit(cv::Mat img_gray,bool fitflag)
{
	int row = img_gray.rows;
	int col = img_gray.cols;
	double minMat, maxMat;
	cv::Point  minIdx, maxIdx;
	cv::minMaxLoc(img_gray, &minMat, &maxMat, &minIdx, &maxIdx);
	cv::Scalar meanAll;
	meanAll = cv::mean(img_gray);
	vector<cv::Point2f> fitLineData;
	int len = CrossCenterParameter::iterationInterval;
	int iteration = max(img_gray.rows / 80, len);
	//int iteration = 10;
	vector<double>rawData;
	vector<int>vecList;
	double num = 2;

	for (int i0 = iteration; i0 < row; i0 = i0 + iteration)// iteration thrsh2
	{
		//vector<double>x, y;
		cv::Rect rec;// = cv::Rect(bottom, left, top - bottom, right - left);
		cv::Mat rowMat = img_gray(cv::Range(i0, i0 + 1), cv::Range(0, img_gray.cols));
		cv::Mat rowth;
		cv::threshold(rowMat, rowth, 0, 255, THRESH_OTSU);
		double maxVec;
		int roiLen = CrossCenterParameter::roilength;
		bool flag = getROIRect(img_gray, rowMat, i0, 0, roiLen, rec, maxVec);

		if (!flag)
			continue;
		//y = rowMat;
		//x = linespace(0, img_gray.cols, 1);
		cv::Mat roi = img_gray(rec);
		cv::Scalar meanRoi = cv::mean(roi);
		cv::Mat meanMat, stdMat;
		cv::meanStdDev(roi, meanMat, stdMat);
		double corr = corrlateMat180(roi);//corrlateMat(roi);

		if (maxVec > maxMat / 10 /*& meanRoi(0) > meanAll(0)*/ /*& corr > 0.1*/ & stdMat.at<double>(0) > 2) // data need to be change as different image,thresh1
		{
			vector<cv::Point2f>gaussFitdata;
			cv::Point maxLoc;
			cv::minMaxLoc(rowMat, NULL, NULL, NULL, &maxLoc);

			double index=-1;
            if (fitflag==false)
            {
                index = maxLoc.x;
			}
            else
            {
            int len = CrossCenterParameter::searchlength;
			int start = max(0, maxLoc.x - len);
			int end = min(img_gray.cols, maxLoc.x + len);

			for (int j = start; j < end; j++)
			{
				gaussFitdata.push_back(cv::Point2f(j, rowMat.at<uchar>(0, j)));
			}
			//index = findIndexByGaussianFit(gaussFitdata);
			index = findIndexByPolynominal(gaussFitdata, 2);
			}
			if (index > -1)
			{
				fitLineData.push_back(cv::Point2f(index, i0));
				rawData.push_back(index);
				cv::circle(m_img_draw, cv::Point2f(index, i0), 1, cv::Scalar(255, 0, 0), -1);
			}

		}
	}
	if (rawData.size() == 0) {
		return Vec4f();
	}
	fitLineData = pointFinterByAngle(fitLineData);
	if (fitLineData.size() < 2)
		return Vec4f();
	//fit the vertical line
	int thred = CrossCenterParameter::dataFilterVer;
	// vecList = dataFilter(rawData, thred);
	 //dataClusters(rawData,vecList,thred);
	vector<double> rawDataNew = rawData;
	//vector<cv::Point2f> fitLineDataFindal=fitLineData;
	// not need refilter

	vector<cv::Point2f>fitLineDataFindal;
	fitLineDataFindal = fitLineData;
	cv::Vec4f lines;
	if (fitLineDataFindal.size() > 1/*& stdev<40*/)
	{
		cv::fitLine(fitLineDataFindal, lines, cv::DIST_L1, 0, 1e-2, 1e-2);
		cv::Point point0;
		point0.x = lines[2];
		point0.y = lines[3];
		double k = lines[1] / lines[0];
		cv::Point point1, point2;
		point1.x = 0;
		point1.y = k * (0 - point0.x) + point0.y;
		point2.x = 3000;
		point2.y = k * (3000 - point0.x) + point0.y;
		cv::Point point10, point20;
		point10.x = point1.y; point10.y = point1.x;
		point20.x = point2.y; point20.y = point2.x;
		//cv::line(m_img_draw, point1, point2, cv::Scalar(0, 255, 0), 1, 8, 0);
	}

	return lines;
}


PointSet CrossCenter::verticalScan(cv::Mat img_gray)
{
	int num = 0;
	int row = img_gray.rows;
	int col = img_gray.cols;
	double minMat, maxMat;
	cv::Point  minIdx, maxIdx;
	cv::minMaxLoc(img_gray, &minMat, &maxMat, &minIdx, &maxIdx);
	cv::Scalar meanAll;
	meanAll = cv::mean(img_gray);
	vector<cv::Point2f> fitLineData;
	int len = CrossCenterParameter::iterationInterval;
	int iteration = max(img_gray.rows / 50, len);
	//int iteration = 10;
	vector<double>rawData;
	vector<int>vecList;

	for (int i0 = iteration; i0 < row; i0 = i0 + iteration)// iteration thrsh2
	{
		//vector<double>x, y;
		cv::Rect rec;// = cv::Rect(bottom, left, top - bottom, right - left);
		cv::Mat rowMat = img_gray(cv::Range(i0, i0 + 1), cv::Range(0, img_gray.cols));
		int roiLen = CrossCenterParameter::roilength;
		double maxVec;
		bool flag = getROIRect(img_gray, rowMat, i0, 0, roiLen, rec, maxVec);
		if (!flag)
			continue;
		//y = rowMat;
		//x = linespace(0, img_gray.cols, 1);
		cv::Mat roi = img_gray(rec).clone();
		cv::Scalar meanRoi = cv::mean(roi);
		cv::Mat meanMat, stdMat;
		cv::meanStdDev(roi, meanMat, stdMat);
		double corr = corrlateMat180(roi);//corrlateMat(roi);

		if (maxVec > maxMat / 10 & meanRoi(0) > meanAll(0) /*& corr > 0.1*//* & stdMat.at<double>(0) > 2*/) // data need to be change as different image,thresh1
		{
			vector<Peak>peaks;
			//vector<int>xVec11=findPeaksByContour(rowMat);
			vector<int>xVec1 = findPeaks(rowMat, peaks, maxMat / 10.0, 4, maxMat / 20.0);
			//vector<int>xVec1 = findPeakByMaxLoc(rowMat);
		//	vector<int>xVec1 = findPeaks1(rowMat, maxVec > maxMat / 10,50);
			//for (int i = 0; i < rowMat.cols; i++)
			//	cout << double(rowMat.at<uchar>(0, i)) << endl;
			vector<int>xVec;
			int start = rec.tl().x;
			int end = rec.br().x;
			for (int i = 0; i < xVec1.size(); i++)
			{
				//if (xVec1[i] > start && xVec1[i] < end)
					xVec.push_back(xVec1[i]);
			}
		
			vector<cv::Point2f>gaussFitdata;
			for (int i = 0; i < xVec.size(); i++)
			{
				cv::Point maxLoc;
				int len = CrossCenterParameter::searchlength;
				int start = max(0, xVec[i] - len);
				int end = min(img_gray.cols, xVec[i] + len);
				gaussFitdata.clear();
				for (int j = start; j < end; j++)
				{
					gaussFitdata.push_back(cv::Point2f(j, rowMat.at<uchar>(0, j)));
				}
				float x = findGaussianFitMaxValue(gaussFitdata);
				if (x < 1e-6)
					continue;
				
					fitLineData.push_back(cv::Point2f(x, i0));
					cv::circle(m_img_draw, cv::Point2f(x, i0), 5, cv::Scalar(255, 0, 0), -1);
				
			}
		}
	}
	PointSet ptset;
	PointSet ptset2;
	PointSet ptset1 = pointFilterAngle(fitLineData, 5);
	if (ptset1.flag)
	{
		ptset.angle1 = ptset1.angle1;
		ptset.pts = ptset1.pts;
		ptset.flag = true;
		if(ptset1.ptsOther.size()>fitLineData.size()/4.0)
	    ptset2 = pointFilterAngle(ptset1.ptsOther, 5);
	}
	if (ptset2.flag&&ptset2.pts.size()>fitLineData.size()/4.0)
	{
		int subAng = abs(ptset1.angle1 - ptset2.angle1)-90/2;
		if (abs(subAng) <=1)
		{
			ptset.angle2 = ptset2.angle1;
			ptset.ptsOther = ptset2.pts;
		}
	}
	//draw
	if (ptset.flag)
	{
		for (int i = 0; i < ptset.pts.size(); i++)
		{
			cv::circle(m_img_draw, ptset.pts[i], 5, Scalar(0, 0, 255),-1);
		}
		for (int i = 0; i < ptset.ptsOther.size(); i++)
		{
			cv::circle(m_img_draw, ptset.ptsOther[i], 5, Scalar(255, 255, 0), -1);
		}

	}
	return ptset;
}
PointSet CrossCenter::horizontalScan(cv::Mat img_gray)
{
	int num = 0;
	int row = img_gray.rows;
	int col = img_gray.cols;
	double minMat, maxMat;
	cv::Point  minIdx, maxIdx;
	cv::minMaxLoc(img_gray, &minMat, &maxMat, &minIdx, &maxIdx);
	cv::Scalar meanAll;
	meanAll = cv::mean(img_gray);
	vector<cv::Point2f> fitLineData;
	int len = CrossCenterParameter::iterationInterval;
	int iteration = max(img_gray.cols / 50, len);
	//int iteration = 10;
	vector<double>rawData;
	vector<int>vecList;

	for (int i0 = 0; i0 < col; i0 = i0 + iteration)// iteration thrsh2
	{
		cv::Rect rec;
		//cv::Mat rowMat = img_gray(cv::Range(i0, i0 + 1), cv::Range(0, img_gray.cols));
		cv::Mat rowMat = img_gray(cv::Range(0, img_gray.rows), cv::Range(i0, i0 + 1));

		double maxVec;
		int roiLen = CrossCenterParameter::roilength;
		bool flag = getROIRect(img_gray, rowMat, i0, 1, roiLen, rec, maxVec);
		if (!flag)
			continue;
		cv::Mat roi = img_gray(rec).clone();
		cv::Scalar meanRoi = cv::mean(roi);
		cv::Mat meanMat, stdMat;
		cv::meanStdDev(roi, meanMat, stdMat);
		double corr = corrlateMat180(roi);//corrlateMat(roi);

		if (maxVec > maxMat / 10 & meanRoi(0) > meanAll(0) /*& corr > 0.1*/ /*& stdMat.at<double>(0) > 2*/) // data need to be change as different image,thresh1
		{

			vector<Peak>peaks;
			if (rowMat.cols == 1);
			cv::rotate(rowMat, rowMat, ROTATE_90_COUNTERCLOCKWISE);
			//vector<int>xVec1 = findPeaksByContour(rowMat);
            vector<int>xVec1 = findPeaks(rowMat, peaks, maxMat / 10.0, 4, maxMat / 20.0);
			//vector<int>xVec1 = findPeakByMaxLoc(rowMat);
		//	vector<int>xVec1 = findPeaks1(rowMat, maxVec > maxMat / 10,50);

		
			vector<int>xVec;
			int start = rec.tl().y;
			int end = rec.br().y;
			for (int i = 0; i < xVec1.size(); i++)
			{
				//if (xVec1[i] > start && xVec1[i] < end)
					xVec.push_back(xVec1[i]);
			}


			vector<cv::Point2f>gaussFitdata;
			for (int i = 0; i < xVec.size(); i++)
			{
				cv::Point maxLoc;
				int len = CrossCenterParameter::searchlength;
				int start = max(0, xVec[i] - len);
				int end = min(int(rowMat.total()), xVec[i] + len);
				gaussFitdata.clear();
				for (int j = start; j < end; j++)
				{
					gaussFitdata.push_back(cv::Point2f(j,rowMat.at<uchar>(0, j)));
				}
				float x = findGaussianFitMaxValue(gaussFitdata);
				if (x < 1e-6)
					continue;
				
					fitLineData.push_back(cv::Point2f(i0, x));
					cv::circle(m_img_draw, cv::Point2f(i0, x), 5, cv::Scalar(255, 0, 0), -1);
				
			}
		}
	}
	PointSet ptset;
	PointSet ptset2;
	PointSet ptset1 = pointFilterAngle(fitLineData, 5);
	if (ptset1.flag)
	{
		ptset.angle1 = ptset1.angle1;
		ptset.pts = ptset1.pts;
		ptset.flag = true;
		if (ptset1.ptsOther.size() > fitLineData.size() / 4.0)
		ptset2 = pointFilterAngle(ptset1.ptsOther, 5);
	}
	if (ptset2.flag&&ptset2.pts.size()>fitLineData.size()/4.0)
	{
		int subAng = abs(ptset1.angle1 - ptset2.angle1)-90/2;
		if (abs(subAng)<=1)
		{
			ptset.angle2 = ptset2.angle1;
			ptset.ptsOther = ptset2.pts;
		}
	}
	//draw
	if (ptset.flag)
	{
		for (int i = 0; i < ptset.pts.size(); i++)
		{
			cv::circle(m_img_draw, ptset.pts[i], 5, Scalar(0, 0, 255), -1);
		}
		for (int i = 0; i < ptset.ptsOther.size(); i++)
		{
			cv::circle(m_img_draw, ptset.ptsOther[i], 5, Scalar(255, 255, 0), -1);
		}
	}
	return ptset;
}
float CrossCenter::findGaussianFitMaxValue(vector<cv::Point2f> pts)
{

	double maxfit = 0;
	cv::Mat A(3, 1, CV_32FC1, cv::Scalar(0));
	gaussiantCurveFit(pts, 2, A);
	float a0 = A.at<float>(0, 0);
	float a1 = A.at<float>(1, 0);
	float a2 = A.at<float>(2, 0);
	if (abs(a1) > 1e-6)
	{
		float fParmB = -1 / (a2);
		float fParmC = -a1 / (2 * a2);
		float fParmA = std::exp(a0 + fParmC * fParmC / fParmB);
		vector<double>xfit, yfit;
		double dMaxYValue = -1.0;
		double minP = pts[0].x;
		double maxP = pts[pts.size() - 1].x;
		double step = 1;// (maxP - minP) / 40;
		for (int i = minP; i <= maxP; i = i + step)
		{
			double x = i;
			double y = 0;
			if (int(fParmA) != 0)
			{
				y = fParmA * exp(-pow(x - fParmC, 2) / fParmB);
			}
			if (y >= dMaxYValue)
			{
				dMaxYValue = y;
			}
			xfit.push_back(x);
			yfit.push_back(y);
		}
		int maxIndex = max_element(yfit.begin(), yfit.end()) - yfit.begin();
		maxfit = xfit[maxIndex];
	}
	return maxfit;
}
cv::Vec4f CrossCenter::horizontalLineFit(cv::Mat img_gray, bool fitflag)
{

	int row = img_gray.rows;
	int col = img_gray.cols;
	double minMat, maxMat;
	cv::Point  minIdx, maxIdx;
	cv::minMaxLoc(img_gray, &minMat, &maxMat, &minIdx, &maxIdx);
	cv::Scalar meanAll = cv::mean(img_gray);

	int len = CrossCenterParameter::iterationInterval;
	int iteration = max(img_gray.rows / 80, len);
	//int iteration = 10;
	vector<cv::Point2f> fitLineData;
	vector<double>rawData;
	vector<int>vecList;
	// debug
	int num = 2;
	for (int j0 = iteration; j0 < col; j0 = j0 + iteration)
	{
		vector<double>x, y;
		cv::Rect rec;
		cv::Mat rowMat = img_gray(cv::Range(0, img_gray.rows), cv::Range(j0, j0 + 1));
		double maxVec;
		int roiLen = CrossCenterParameter::roilength;
		bool flag = getROIRect(img_gray, rowMat, j0, 1, roiLen, rec, maxVec);
		if (!flag)
			continue;
		y = rowMat;
		x = linespace(0, img_gray.rows, 1);
		cv::Mat roi = img_gray(rec);
		cv::Scalar meanRoi = cv::mean(roi);
		double corr = corrlateMat180(roi);
		cv::Mat meanMat, stdMat;
		cv::meanStdDev(roi, meanMat, stdMat);
		if (maxVec > maxMat / 10 /*& meanRoi(0) > meanAll(0) & corr > 0.1 */& stdMat.at<double>(0) > 2)
		{
			vector<cv::Point2f>gaussFitdata;
			cv::Point maxLoc;
			cv::minMaxLoc(rowMat, NULL, NULL, NULL, &maxLoc);
            double index=-1;
            if (fitflag == false)
            {
                index = maxLoc.y;
            }
            else
            {            
			int len = CrossCenterParameter::searchlength;
			int start = max(0, maxLoc.y - len);
			int end = min(int(rowMat.total()), maxLoc.y + len);
			for (int j = start; j < end; j++)
			{
				gaussFitdata.push_back(cv::Point2f(j, rowMat.at<uchar>(j, 0)));
			}
			//index = findIndexByGaussianFit(gaussFitdata);
			index = findIndexByPolynominal(gaussFitdata);

			}
			if (index > -1)
			{
				fitLineData.push_back(cv::Point2f(j0, index));
				rawData.push_back(index);
				cv::circle(m_img_draw, cv::Point2f(j0, index), 1, cv::Scalar(0, 0, 255), -1);
			}

		}
	}
	if (rawData.size() == 0) {
		return Vec4f();
	}
	fitLineData = pointFinterByAngle(fitLineData);
	if (fitLineData.size() < 2)
		return Vec4f();
	int thred = CrossCenterParameter::dataFilterHor;
	// vecList = dataFilter(rawData, thred);
 //	dataClusters(rawData, vecList, thred);
	vector<double> rawDataNew = rawData;
	//  vector<cv::Point2f> fitLineDataFindal = fitLineData;
	  // not need refilter 
	vector<cv::Point2f>fitLineDataFindal;

	fitLineDataFindal = fitLineData;
	cv::Vec4f lines;
	if (fitLineDataFindal.size() > 1/*& stdev<40*/)
	{
		cv::fitLine(fitLineDataFindal, lines, cv::DIST_L1, 0, 1e-2, 1e-2);
		cv::Point point0;
		point0.x = lines[2];
		point0.y = lines[3];
		double k = lines[1] / lines[0];
		cv::Point point1, point2;
		point1.x = 0;
		point1.y = k * (0 - point0.x) + point0.y;
		point2.x = 3000;
		point2.y = k * (3000 - point0.x) + point0.y;
		cv::Point point10, point20;
		point10.x = point1.y; point10.y = point1.x;
		point20.x = point2.y; point20.y = point2.x;
		//cv::line(m_img_draw, point1, point2, cv::Scalar(0, 255, 0), 1, 8, 0);
	}

	return lines;
}

bool CrossCenter::getROIRect(cv::Mat img, cv::Mat rowMat, int i0, int flag, int len, cv::Rect& rect, double& maxgray)
{

	cv::Point maxLoc;
	cv::minMaxLoc(rowMat, NULL, &maxgray, NULL, &maxLoc);
    int len1 = cv::min(int(rowMat.total()), len);
    if (rowMat.total() < len)
    {
        rect = cv::Rect(0, 0, rowMat.cols, rowMat.rows);
    }
    else
    {
        rect.x = maxLoc.x - len / 2;
        rect.y = maxLoc.y - len / 2;
        rect.width = len;
        rect.height = len;
    }
    if (flag == 0)
        rect.y = rect.y + i0;
    else
        rect.x = rect.x + i0;
	if (rect.x < 0 || rect.y < 0)
		return false;
	if (rect.x + len > img.cols || rect.y + len > img.rows)
		return false;
	return true;
}
vector<double> CrossCenter::linespace(double start, double end, double step)
{
	vector<double>xVec;
	for (int i = start; i < end; i = i + step)
	{
		xVec.push_back(i);
	}
	return xVec;
}
vector<cv::Point2f> CrossCenter::pointFinterByAngle(vector<cv::Point2f> pts, double thresh)
{
	if (pts.size() <= 3)
		return pts;
	vector<cv::Point2f> ptf;
	vector<int>angleVec;
	int n = (180 / thresh) + 1;
	vector<int>historgram(n, 0);
	for (int i = 0; i < pts.size() - 1; i++)
	{
		double delty = pts[i + 1].y - pts[i].y;
		double deltx = pts[i + 1].x - pts[i].x;
		if (deltx == 0)
			deltx = 1e-6;
		double angle = atan(delty / deltx) * 180 / CV_PI + 90;
		int ang = round(angle / thresh);
		angleVec.push_back(ang);
		historgram[ang] = historgram[ang] + 1;
	}
	int max = max_element(historgram.begin(), historgram.end()) - historgram.begin();
	int maxV = *max_element(historgram.begin(), historgram.end());
	if (maxV == angleVec.size())
		return pts;

	vector<int>iVec;
	for (int i = 0; i < angleVec.size(); i++)
	{
		if (angleVec[i] == max)
		{
			if (iVec.size() > 0)
			{
				if (find(iVec.begin(), iVec.end(), i) == iVec.end())
				{
					ptf.push_back(pts[i]);
					ptf.push_back(pts[i + 1]);
					iVec.push_back(i);
					iVec.push_back(i + 1);
				}
				else
				{
					ptf.push_back(pts[i + 1]);
					iVec.push_back(i + 1);
				}
			}
			else
			{
				ptf.push_back(pts[i]);
				ptf.push_back(pts[i + 1]);
				iVec.push_back(i);
				iVec.push_back(i + 1);
			}
		}
	}
	return ptf;
}
vector<cv::Point2f> CrossCenter::pointClusterByAngle(vector<cv::Point2f> pts, double thresh)
{
	if (pts.size() <= 3)
		return pts;

	PointSet ptset1 = pointFilterAngle(pts, thresh);
	PointSet ptset2 = pointFilterAngle(ptset1.ptsOther, thresh);

	return vector<cv::Point2f>();
}
PointSet CrossCenter::pointFilterAngle(vector<cv::Point2f> pts, double thresh)
{
	PointSet ptsSet;
	if (pts.size() < 1)
		return ptsSet;
	if (pts.size() == 2)
	{
		double delty1 = pts[1].y - pts[0].y;
		double deltx1 = pts[1].x - pts[0].x;
		if (deltx1 == 0)
			deltx1 = 1e-6;
		double angle = atan(delty1 / deltx1) * 180 / CV_PI + 90;
		int ang = round(angle / thresh);
		ptsSet.flag = true;
		ptsSet.pts = pts;
		ptsSet.angle1 = ang;
		return ptsSet;
	}
	if (pts.size() == 3)
	{
		double delty1 = pts[1].y - pts[0].y;
		double deltx1 = pts[1].x - pts[0].x;
		if (deltx1 == 0)
			deltx1 = 1e-6;
		double angle = atan(delty1 / deltx1) * 180 / CV_PI + 90;
		int ang1 = round(angle / thresh);
		double delty2 = pts[2].y - pts[1].y;
		double deltx2 = pts[2].x - pts[1].x;
		if (deltx2 == 0)
			deltx2 = 1e-6;
		 angle = atan(delty2 / deltx2) * 180 / CV_PI + 90;
		int ang2 = round(angle / thresh);

		ptsSet.flag = true;
		ptsSet.pts = pts;
		ptsSet.angle1 = (ang1+ang2)/2;
		return ptsSet;

	}
	vector<int>angleVec;
	int n = 180 / thresh + 1;
	vector<int>historgram(n, 0);
	vector<cv::Point2f>pts1, pts2;
	int max, maxV;
	int j;
	for (j = 0; j < pts.size(); j++)
	{
		cv::Point2f pt0 = pts[j];
		pts1.push_back(pt0);
		for (int i = j+1; i < pts.size(); i++)
		{
			double delty1 = pts[i].y - pt0.y;
			double deltx1 = pts[i].x - pt0.x;
			if (deltx1 == 0)
				deltx1 = 1e-6;
			double angle = atan(delty1 / deltx1) * 180 / CV_PI + 90;
			int ang = round(angle / thresh);
			historgram[ang] = historgram[ang] + 1;
			angleVec.push_back(ang);
		}
		max = max_element(historgram.begin(), historgram.end()) - historgram.begin();
		maxV = *max_element(historgram.begin(), historgram.end());
		if (maxV > pts.size() / 3)
		{
			ptsSet.flag = true;
			break;
		}
		else
		{
			vector<int>a(n, 0);
			historgram=a;
			pts1.clear();
			angleVec.clear();
		}
	}
	if (!ptsSet.flag)
		return ptsSet;

	for (int i = 0; i < angleVec.size(); i++)
	{
		if (angleVec[i] == max)
		{
			pts1.push_back(pts[i + 1+j]);
		}
		else
		{
			pts2.push_back(pts[i + 1+j]);
		}
	}
	
	ptsSet.pts = pts1;
	ptsSet.angle1 = max;	
	ptsSet.ptsOther = pts2;
	return ptsSet;
}
vector<int> CrossCenter::findPeaks( cv::Mat data, std::vector<Peak>& peaks, double minHeight, int minWidth, double minProminence)
{
	vector<int>xVec;
	peaks.clear();  // 清空峰值数组
	data.convertTo(data, CV_64FC1);

	int thresh=1;
	for (int i = thresh; i < data.cols - thresh; ++i) {
		// 判断是否为峰值，即比相邻的两个点大，并且大于设定的最小高度
		//for (int i0 = i - thresh;  i0 < i + thresh; i0++)
		//{
		//	if (data.at<double>(0, i) > data.at<double>(0, i0));
		//	break;
		//}
		if (data.at<double>(0, i) > minHeight &&
			(data.at<double>(0, i) > data.at<double>(0, i - 1) &&
			data.at<double>(0, i) >= data.at<double>(0, i + 1))) {
			// 寻找峰值的宽度，即峰值到其两侧的极小值点的距离
			int left = i - thresh;
			while (left >= 0 &&( data.at<double>(0, left) <= data.at<double>(0, left+thresh))) 
			{
				--left;
			}

			int right = i + thresh;
			while (right < data.cols && data.at<double>(0, right) <= data.at<double>(0, right-thresh)) {
				++right;
			}

			// 计算峰值的宽度和突出度
			int width = right - left - 2;
			double prominence = data.at<double>(0, i) - std::max(data.at<double>(0, left + 1), data.at<double>(0, right - 1));

			// 判断峰值的宽度和突出度是否满足设定的条件
			if (width >= minWidth && prominence > minProminence) {
				if (peaks.size() > 0)
				{
					int sub = i - peaks[peaks.size() - 1].index;
					if (sub > 100)
					{
						peaks.push_back({ i, data.at<double>(0, i), width, prominence });
						xVec.push_back(i);
					}
					
				}
				else
				{
					peaks.push_back({ i, data.at<double>(0, i), width, prominence });
					xVec.push_back(i);
				}
				}
		}
	}
	return xVec;
}
vector<int> CrossCenter::findPeaksByContour(cv::Mat rowMat)
{
	cv::Mat rowth;
	cv::threshold(rowMat, rowth, 0, 255, THRESH_OTSU);
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	findContours(rowth, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1));
	vector<int> xVec;
	if (contours.size() > 0)
	{
		if (contours.size() == 1)
		{
			cv::Point maxLoc;
			cv::minMaxLoc(rowMat, NULL, NULL, NULL, &maxLoc);
			xVec.push_back(maxLoc.x);
			return xVec;
		}
		for (int i = 0; i < contours.size(); i++)
		{
			cv::Rect rect = cv::boundingRect(contours[i]);
			double area = rect.area();
			if (area < 6)
				continue;
			//cv::circle(rowMat, rect.tl(), 5, Scalar(255, 0, 255), -1);
			int x1 = rect.tl().x + rect.width / 2.0;
			xVec.push_back(x1);
		}
	}
	return xVec;
}
vector<int> CrossCenter::findPeakByMaxLoc(cv::Mat rowMat)
{
	vector<int>xVec;
	cv::Point maxLoc;
	cv::minMaxLoc(rowMat, NULL, NULL, NULL, &maxLoc);
	xVec.push_back(maxLoc.x);
	return xVec;
}
vector<int> CrossCenter::findPeaks1(cv::Mat rowMat, double minpeakdistance, double minpeakheight)
{
	// minpeakheight ---最小峰值大小
	// minpeak distance -- 峰值之间的最小距离
	vector<int>diff_v(rowMat.cols - 1);
	for (int i = 0; i < rowMat.cols-1; i++)
	{
		if (rowMat.at<uchar>(0, i + 1) - rowMat.at<uchar>(0, i) > 0)
			diff_v[i] = 1;
		else if (rowMat.at<uchar>(0, i + 1) - rowMat.at<uchar>(0, i) < 0)
			diff_v[i] = -1;
		else diff_v[i] = 0;
	}
	for (int i = diff_v.size() - 1; i >= 0; i--)
	{
		if (diff_v[i] == 0 && i == diff_v.size() - 1)
		{
			diff_v[i] = 1;
		}
		else if (diff_v[i] == 0)
		{
			if (diff_v[i + 1] >= 0)
				diff_v[i] = 1;
			else
				diff_v[i] = -1;
		}
	}
	vector<int>peak_height;
	for (int i = 0; i < diff_v.size() - 1; i++)
	{
		if ((diff_v[i + 1] - diff_v[i] == -2) && (rowMat.at<uchar>(0,i + 1) >= minpeakheight))
		{
			peak_height.push_back(i + 1);
		}
	}
	//对波峰之间的距离进行判断，从第二个到最后一个
	int peak_height_length = peak_height.size();
	int peak_num = 0;  //计算波峰数量
	vector<int>peak_height1;


	if (peak_height.size() <= 1)
		return peak_height;

	for (int i = 1; i < peak_height_length - 1; i++)
	{
		if (peak_height[i] - minpeakdistance >= peak_height[i - 1] && peak_height[i] + minpeakdistance >= peak_height[i + 1])
		{
			peak_num++;
			peak_height1.push_back(i);
		}
	}
	//判断第一个数据值
	if (peak_height[0] + minpeakdistance >= peak_height[1])
	{
		peak_num = peak_num + 1;
		peak_height1.push_back(peak_height[0]);
	}
	//判断最后一个
	if (peak_height[peak_height_length - 1] - minpeakdistance >= peak_height[peak_height_length - 2])
	{
		peak_num = peak_num + 1;
		peak_height1.push_back(peak_height[peak_height_length - 1]);
	}
	return peak_height1;
}
cv::Point2f CrossCenter::find_centerGaussian(cv::Mat img,bool fitflag)
{
	if (img.data != NULL)
	{
		cv::Mat img_gray;
		if (img.channels() == 3)
			cvtColor(img, img_gray, CV_BGR2GRAY);
		else
			img_gray = img.clone();
		cv::Mat img_draw = img.clone();
		cv::Mat img_gray1;
		if (img_gray.depth() == CV_8U)
		{
			img_gray1 = img_gray.clone();
			//m_img_draw = img.clone();
		}
		else
		{
			img_gray1 = convertToUint8(img_gray);
		//	m_img_draw = convertToUint8(img);
		}
        img_draw = convertTo3Channels(img_gray1);
        m_img_draw = img_draw.clone();
		//GaussianBlur(img_gray1, img_gray1, Size(5, 5), 5, 5);

		cv::Vec4f lineVertical = verticalLineFit(img_gray1, fitflag);
        cv::Vec4f lineHorizontal = horizontalLineFit(img_gray1, fitflag);

		if (!(lineVertical[0] == 0 & lineVertical[1] == 0) & !(lineHorizontal[0] == 0 & lineHorizontal[1] == 0))
		{
			m_center = calLinesIntersection(lineVertical, lineHorizontal);
			// draw line
			// vertical
			cv::Point point0;
			point0.x = lineVertical[2];
			point0.y = lineVertical[3];
			double k = lineVertical[1] / lineVertical[0];
			double b = point0.y - k * point0.x;
			double a1 = k;
			double b1 = -1;
			double c1 = b;
			m_angle1.push_back(atan(k));
     
			cv::Point point1, point2;
			point1.x = 0;
			point1.y = k * (0 - point0.x) + point0.y;
			point2.x = 10000;
			point2.y = k * (10000 - point0.x) + point0.y;
			cv::line(m_img_draw, point1, point2, cv::Scalar(0, 255, 0), 1, 8, 0);
			m_a1 = a1; m_b1 = b1; m_c1 = c1;
            m_thetaV = atan(-a1 / (b1 + 1e-6));
			// horizontal line
			point0.x = lineHorizontal[2];
			point0.y = lineHorizontal[3];
			k = lineHorizontal[1] / lineHorizontal[0];
			m_angle2.push_back(atan(k));
			b = point0.y - k * point0.x;
			double a2 = k;
			double b2 = -1;
			double c2 = b;
			//	cv::Point point1, point2;
			point1.x = 0;
			point1.y = k * (0 - point0.x) + point0.y;
			point2.x = 10000;
			point2.y = k * (10000 - point0.x) + point0.y;
			cv::line(m_img_draw, point1, point2, cv::Scalar(0, 255, 0), 1, 8, 0);
			cv::circle(m_img_draw, m_center, 3, cv::Scalar(0, 255, 0), -1);
			cv::circle(img_draw, m_center, 1, cv::Scalar(0, 0, 255), -1);

			m_a2 = a2; m_b2 = b2; m_c2 = c2;

			       m_thetaH = atan(-a2 / (b2 + 1e-6));
			// add the theta estimate
			double theta = 0;
			theta = abs(m_angle1[0] - m_angle2[0]);
			if (theta<CV_PI / 4 | theta>CV_PI * 3 / 4)
			{
				m_center = { 0,0 };
			}
			if (m_center.x != 0 & m_center.y != 0)
			{
				roi_center = find_roi_center(m_center + center_offset, a1, b1, c1, a2, b2, c2);
				for (int i = 0; i < roi_center.size(); i++)
				{
					cv::circle(m_img_draw, roi_center[i], 1, cv::Scalar(0, 255, 0), -1);

				}
			}

			return m_center;
		}
		else
			return m_center;
	}
	return m_center;
}

cv::Point2f CrossCenter::find_centerGaussianEn(cv::Mat img)
{
	if (img.data != NULL)
	{
		cv::Mat img_gray = convertToUint8(img);
		cv::Mat img_draw = convertTo3Channels(img_gray);
		m_img_draw = img_draw.clone();
		img_draw = img.clone();
		img_draw = convertTo3Channels(img_draw);

		//GaussianBlur(img_gray, img_gray, Size(5, 5), 0, 0);
		cv::medianBlur(img_gray, img_gray, 5);
		//cv::Mat element = cv::getStructuringElement(MORPH_RECT, Size(20, 20));
		//cv::morphologyEx(img_gray, img_gray, MORPH_CLOSE, element);
		//vector<PointInfo> pts1 = verticalScan(img_gray);
		PointSet ptset1=verticalScan(img_gray);
		if (ptset1.flag && ptset1.ptsOther.size() < 2)
		{
			PointSet ptset2 = horizontalScan(img_gray);
			if (ptset2.flag)
			{
				int sub = abs(ptset1.angle1 - ptset2.angle1)-90/2;
				if (abs(sub)<=2)
				{
					ptset1.ptsOther = ptset2.pts;
					ptset1.angle2 = ptset2.angle1;
				}
			}
		}
		if (ptset1.ptsOther.size() < 2)
			return cv::Point2f();
		cv::Vec4f lineVertical, lineHorizontal;		
		cv::fitLine(ptset1.pts, lineVertical, cv::DIST_L1, 0, 1e-2, 1e-2);
		cv::fitLine(ptset1.ptsOther, lineHorizontal, cv::DIST_L1, 0, 1e-2, 1e-2);
		
		if (!(lineVertical[0] == 0 & lineVertical[1] == 0) & !(lineHorizontal[0] == 0 & lineHorizontal[1] == 0))
		{
			m_center = calLinesIntersection(lineVertical, lineHorizontal);
			// draw line
			// vertical
			cv::Point point0;
			point0.x = lineVertical[2];
			point0.y = lineVertical[3];
			double k = lineVertical[1] / lineVertical[0];
			double b = point0.y - k * point0.x;
			double a1 = k;
			double b1 = -1;
			double c1 = b;
			m_angle1.push_back(atan(k));
			cv::Point point1, point2;
			point1.x = 0;
			point1.y = k * (0 - point0.x) + point0.y;
			point2.x = 10000;
			point2.y = k * (10000 - point0.x) + point0.y;
			cv::line(m_img_draw, point1, point2, cv::Scalar(0, 255, 0), 1, 8, 0);
			m_a1 = a1; m_b1 = b1; m_c1 = c1;
            m_thetaV = atan(-a1 / (b1 + 1e-6));

			// horizontal line
			point0.x = lineHorizontal[2];
			point0.y = lineHorizontal[3];
			k = lineHorizontal[1] / lineHorizontal[0];
			m_angle2.push_back(atan(k));
			b = point0.y - k * point0.x;
			double a2 = k;
			double b2 = -1;
			double c2 = b;
			//	cv::Point point1, point2;
			point1.x = 0;
			point1.y = k * (0 - point0.x) + point0.y;
			point2.x = 10000;
			point2.y = k * (10000 - point0.x) + point0.y;
			cv::line(m_img_draw, point1, point2, cv::Scalar(0, 255, 0), 1, 8, 0);
			cv::circle(m_img_draw, m_center, 3, cv::Scalar(0, 255, 0), -1);
			cv::circle(img_draw, m_center, 1, cv::Scalar(0, 0, 255), -1);
			m_a2 = a2; m_b2 = b2; m_c2 = c2;
            m_thetaH = atan(-a2 / (b2 + 1e-6));

			// add the theta estimate
			double theta = 0;
			theta = abs(m_angle1[0] - m_angle2[0]);
			if (theta<CV_PI / 4 | theta>CV_PI * 3 / 4)
			{
				m_center = { 0,0 };
			}
			if (m_center.x != 0 & m_center.y != 0)
			{
				roi_center = find_roi_center(m_center + center_offset, a1, b1, c1, a2, b2, c2);
				for (int i = 0; i < roi_center.size(); i++)
				{
					cv::circle(m_img_draw, roi_center[i], 3, cv::Scalar(0, 255, 0), -1);

				}
			}

			return m_center;
		}
		else
			return m_center;
	}
	return m_center;
}

cv::Point2f CrossCenter::find_centerGradAndGaussian(cv::Mat img)
{
	cv::Point2f center;
	if (img.data != NULL)
	{

		cv::Mat img_draw = convertToUint8(img);
		cv::Point2f c0 = find_centerGrad(img);
		int w = CrossCenterParameter::roilength / m_binNum;
		if ((c0.x) > 1e-6 && (c0.y) > 1e-6)
		{
			int top = cv::max((int)(c0.y - w / 2), 0);
			int bottom = cv::min((int)c0.y + w / 2, img.rows);
			int left = cv::max(0, (int)c0.x - w / 2);
			int right = cv::min(img.cols, (int)c0.x + w / 2);
			Rect rect = Rect(left, top, right - left, bottom - top);
			cv::Mat roi = img(rect);
			cv::Point2f c1 = find_centerGaussian(roi); //
			if (c1.x != 0 && c1.y != 0)
			{
				center = c1 + cv::Point2f(rect.x, rect.y);
			}
			else
				center = c0;

			cv::circle(img_draw, center, 2, Scalar(255, 0, 255), -1);
		}
	}
	return center;
}

cv::Point2f CrossCenter::find_centerHoughLineAndWidth(cv::Mat img, int i, int signFlag)
{
	cv::Point re = cv::Point(0, 0);
	if (img.data != NULL)
	{
		cv::Mat img8 = convertToUint8(img);
		cv::Mat img_draw = img8.clone();
		cv::Point2f c0; //=find_centerGrad(img8);
						//   if (abs(c0.x) > 1e-6 & abs(c0.y) > 1e-6)
		{
			CrossCenter cc(img, 10);
			cv::Point center = cc.get_crossCenter(1); // use the houghline
			double linewidth = cc.get_line_width();
			switch (i)
			{
			case 1: {
				re.x = center.x - linewidth / 2.0 * signFlag;
				re.y = center.y;
				break;
			}
			case 7: {
				re.x = center.x + linewidth / 2.0 * signFlag;
				re.y = center.y;
				break;
			}
			case 3: {
				re.x = center.x;
				re.y = center.y - linewidth / 2.0 * signFlag;
				break;
			}
			case 5: {
				re.x = center.x;
				re.y = center.y + linewidth / 2.0 * signFlag;
				break;
			}
			}
			cv::circle(img_draw, re, 5, cv::Scalar(255, 0, 255), -1);
		}
	}
	return re;

}

cv::Point2f CrossCenter::find_centerLSD(cv::Mat img)
{
	if (img.empty())
		return cv::Point2f();
	cv::Mat tmp, src_gray;
	if (img.channels() == 3)
		cv::cvtColor(img, tmp, COLOR_RGB2GRAY);
	else
		tmp = img.clone();
	tmp.convertTo(src_gray, CV_64FC1);
	cv::Mat img_draw = convertTo3Channels(img);
	cv::Mat gray;
	cv::cvtColor(img_draw, gray, COLOR_RGB2GRAY);
	gray.convertTo(gray, CV_64FC1);

	//src.convertTo(img_draw,CV_8UC3);
	int cols = src_gray.cols;
	int rows = src_gray.rows;
	image_double image = new_image_double(cols, rows);
	image->data = gray.ptr<double>(0);
	ntuple_list ntl = lsd(image);
	//cv::Mat lsd = cv::Mat::zeros(rows, cols, CV_8UC1);
	cv::Point pt1, pt2;
	vector<Vec4i>lines;
	for (int j = 0; j != ntl->size; ++j)
	{
		pt1.x = int(ntl->values[0 + j * ntl->dim]);
		pt1.y = int(ntl->values[1 + j * ntl->dim]);
		pt2.x = int(ntl->values[2 + j * ntl->dim]);
		pt2.y = int(ntl->values[3 + j * ntl->dim]);
		int width = int(ntl->values[4 + j * ntl->dim]);
		//  cv::line(lsd, pt1, pt2, cv::Scalar(255), width, CV_AA);
		cv::line(img_draw, pt1, pt2, cv::Scalar(255), width, LINE_8);
		Vec4i line(pt1.x, pt1.y, pt2.x, pt2.y);
		lines.push_back(line);
		//double k = (double)(pt2.y - pt1.y) / (double)(pt2.x - pt1.x);
		//double angle = atan(k) * 180 / CV_PI;
		//cout << "k= " << angle << endl;
	}
	free_ntuple_list(ntl);

	if (!lines.empty())
	{
		vector<Vec4i> lines1, lines2;
		vector<float> ks1, ks2;
		sepLines(lines, lines1, ks1, lines2, ks2);
		float a1, b1, c1, a2, b2, c2;

		if (!lines1.empty() && !lines2.empty())
		{
			double mean1, mean2;
			mean1 = std::accumulate(ks1.begin(), ks1.end(), 0.0) / ks1.size();
			mean2 = std::accumulate(ks2.begin(), ks2.end(), 0.0) / ks2.size();
			int width1, width2;
			width1 = calLine(lines1, ks1, a1, b1, c1);
			width2 = calLine(lines2, ks2, a2, b2, c2);

			/*  if (abs(mean1) > 0.2)
			  {
				  width1 = calLineVertical(lines1, ks1, a1, b1, c1);
			  }
			  else
			  {
				  width1 = calLine(lines1, ks1, a1, b1, c1);
			  }
			  if (abs(mean2) > 0.2)
			  {
				  width2 = calLineVertical(lines2, ks2, a2, b2, c2);
			  }
			  else
			  {
				  width2 = calLine(lines2, ks2, a2, b2, c2);
			  }*/
			m_line_width = max(width1, width2);
			m_center = calLinesCross(a1, b1, c1, a2, b2, c2);
			if (abs(m_center.x > 1e-6) & abs(m_center.y) > 1e-6)
			{
				double flag1 = abs(-a1 / b1);
				double flag2 = abs(-a2 / b2);
				if (flag1 > flag2)
				{
					m_a1 = a1;
					m_b1 = b1;
					m_c1 = c1;
					m_a2 = a2;
					m_b2 = b2;
					m_c2 = c2;
    

				}
				else
				{
					m_a1 = a2;
					m_b1 = b2;
					m_c1 = c2;
					m_a2 = a1;
					m_b2 = b1;
					m_c2 = c1;

				}
                m_thetaV = atan(-a1 / (b1 + 1e-6));
                m_thetaH = atan(-a2 / (b2 + 1e-6));
				cv::Point p1, p2;
				p1.x = 2000;
				p1.y = -(m_a1 * p1.x + m_c1) / m_b1;
				p2.x = 3000;
				p2.y = -(m_a1 * p2.x + m_c1) / m_b1;
				cv::line(m_img_draw, p1, p2, Scalar(0, 0, 255), 1);

				p1.x = 0;
				p1.y = -(m_a2 * p1.x + m_c2) / m_b2;
				p2.x = 4000;
				p2.y = -(m_a2 * p2.x + m_c2) / m_b2;
				cv::line(m_img_draw, p1, p2, Scalar(0, 0, 255), 1);

				double ang1 = atan2(-m_a1, m_b1);
				double ang2 = atan2(-m_a2, m_b2);
				if (m_angle1.size() > 1 & m_angle2.size() > 1)
				{
					double angle1 = std::accumulate(m_angle1.begin(), m_angle1.end(), 0.0) / m_angle1.size();
					double angle2 = std::accumulate(m_angle2.begin(), m_angle2.end(), 0.0) / m_angle2.size();
					m_angle1.clear();
					m_angle1.push_back(angle1);
					m_angle2.clear();
					m_angle2.push_back(angle2);
				}
				cv::circle(m_img_draw, m_center, 2, cv::Scalar(255, 0, 0), -1);
				roi_center = find_roi_center(m_center + center_offset, a1, b1, c1, a2, b2, c2);
				for (int i = 0; i < roi_center.size(); i++)
				{
					cv::circle(m_img_draw, roi_center[i], 5, cv::Scalar(255, 0, 0), -1);
				}
				return m_center;

			}
			else
			{
				m_center.x = 0; m_center.y = 0;
				return m_center;
			}
		}
		else
			return m_center;
	}

	return cv::Point2f();
}

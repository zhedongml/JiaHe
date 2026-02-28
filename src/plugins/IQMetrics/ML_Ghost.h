#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
#include<opencv2/opencv.hpp>
#include<iostream>
#include"IQMetricUtl.h"
//#include"ml_image_public.h"
namespace MLIQMetrics {
	struct SuqreGhostRe
	{
		double medianGhost = 0;
		double p95Ghost = 0;
		double medianDisplay = 0;
		double p95Display = 0;
		double ratioMedian = 0;
		double ratioP95 = 0;
		cv::Rect displayRect;
		cv::Rect ghostRect;
	};
	struct  GhostRe
	{
		map<string, SuqreGhostRe>ghostMap;
		cv::Mat imgdraw;
		bool flag = true;
		string errMsg = "";
	};
	class IQMETRICS_API MLGhost:public MLImageDetection::MLimagePublic
	{
	public:
		MLGhost();
		~MLGhost();
	public:
		GhostRe getGhostSquares(cv::Mat img);	
		SuqreGhostRe getSquareGhost(cv::Mat img, cv::Rect rectDisplay,cv::Rect rectGhost,cv::Mat& imgdaw,int i);
		GhostRe getGhost(cv::Mat img, ROIParaNew roiInfo= ROIParaNew());
	private:
		vector<cv::Rect>findDisplayRect(cv::Mat img,cv::Mat &imgdraw);
		vector<cv::Rect> sortRects(vector<cv::Rect>rectVec, vector<cv::Point2f>cenVec);
		cv::Rect findSquareRect(cv::Mat roi);
		vector<int> findPeaks(cv::Mat data, double minHeight, int minWidth, double minProminence);
		bool findEdgePts1(cv::Mat data, int flag, cv::Point2f& p1, cv::Point2f& p2);
		void getGhostValidData(cv::Mat roi, double& p95, double& median);
	};
}

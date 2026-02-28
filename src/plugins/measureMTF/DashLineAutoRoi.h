#pragma once

#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include "harris.h"
class DashLineAutoRoi
{
	
public:
	struct ClusterPoint {
		cv::Point2d pt;
		int count = 0;
	};
	DashLineAutoRoi();
	cv::Point2d getCornnerPoint(cv::Mat cvImg,cv::Point2d nearPt = cv::Point2d());
	double cal_distance(cv::Point2d pt1, cv::Point2d pt2);
	std::vector<DashLineAutoRoi::ClusterPoint> clusterpoint(std::vector<cv::Point2d> pts, int thresh = 50);
	int DashLineAutoRoi::findClosestCorner2(cv::Point2d centerPt, std::vector<ClusterPoint>& pts);
	cv::Mat convet16bitTo8bit(cv::Mat& rawimg);
private:
	double scaled = 0.5;
};


#pragma once

#include<opencv2/opencv.hpp>
#include<iostream>
#include"IQMetricUtl.h"
#include"ml_gridDetect.h"
namespace MLIQMetrics {
	struct LateralColorRe
	{
		double meandis=0;
		double maxdis = 0;
		double disRGArcmin = 0;
		double disRBArcmin = 0;
		double disGBArcmin = 0;
		cv::Mat subXRGArcmin;
		cv::Mat subYRGArcmin;
		cv::Mat subXRBArcmin;
		cv::Mat subYRBArcmin;
		cv::Mat subXGBArcmin;
		cv::Mat subYGBArcmin;	
		double grLCS = 0;
		double gbLCS = 0;
		map<string, cv::Point2f>RLoc;
		map<string, cv::Point2f>GLoc;
		map<string, cv::Point2f>BLoc;
		cv::Mat locxR;
		cv::Mat locyR;
		cv::Mat locxG;
		cv::Mat locyG;
		cv::Mat locxB;
		cv::Mat locyB;
		cv::Mat imgdrawR;
		cv::Mat imgdrawG;
		cv::Mat imgdrawB;
		cv::Mat imgdraw;
		bool flag = "true";
		string errMsg = "";
	};

	class IQMETRICS_API MLLateralColor :public MLImageDetection::MLimagePublic
	{
	public:
		MLLateralColor();
		~MLLateralColor();
	public:
		void setIsSLB(bool flag);
		void setPatternCenter(cv::Point2f cen);
		void setIsUpdateSLB(bool flag);
		LateralColorRe getLateralColorGrid(cv::Mat rImg, cv::Mat gImg, cv::Mat bImg);
		LateralColorRe getLateralColorGridNew(cv::Mat rImg, cv::Mat gImg, cv::Mat bImg);
		LateralColorRe getLateralColorCross(cv::Mat rImg, cv::Mat gImg, cv::Mat bImg);
		LateralColorRe getLateralColorGridCenter(const cv::Mat rImg, const cv::Mat gImg, const cv::Mat bImg);
		void updateImgdraw(cv::Point2f cen, cv::Mat &imgdraw,int binNum);
	private:
		void writeLateralGridCenter(MLImageDetection::GridRe re);
		void readLateralGridCenter(MLImageDetection::GridRe &re);

	private:
		int m_resizeNum = 4;
		vector<string> m_str = { "2","4" ,"5" ,"6" ,"8" };
		bool m_IsSLB = true;
		cv::Point2f m_center;
		int m_len = 7000;
		bool m_updateSLB = true;
	};
}


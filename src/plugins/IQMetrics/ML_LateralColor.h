#pragma once

#include<opencv2/opencv.hpp>
#include<iostream>
#include"IQMetricUtl.h"
namespace MLIQMetrics {
	struct LateralColorRe
	{
		double meandis=0;
		double maxdis = 0;
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
		void setFOVType(FOVTYPE type);
		void setIsUpdateSLB(bool flag);
		LateralColorRe getLateralColorGrid(cv::Mat rImg, cv::Mat gImg, cv::Mat bImg);
		LateralColorRe getLateralColorCross(cv::Mat rImg, cv::Mat gImg, cv::Mat bImg);
		LateralColorRe getLateralColorCrossPreLoc(const cv::Mat rImg, const cv::Mat gImg, const cv::Mat bImg);
		void updateImgdraw(cv::Point2f cen, cv::Mat &imgdraw,int binNum);
	private:
		cv::Mat convertVecToMat(vector<double>subvec);
		void updateLateralColorRe(LateralColorRe& re, bool isSLB, FOVTYPE type);

	private:
		int m_resizeNum = 4;
		vector<string> m_str = { "2","4" ,"5" ,"6" ,"8" };
		bool m_IsSLB = true;
		FOVTYPE m_FOVType;
		bool m_IsUpdateSLB = false;
	};
}


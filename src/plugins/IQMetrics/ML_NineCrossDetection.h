#pragma once
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
namespace MLIQMetrics
{
	static bool isNineCrossDetectionFlagBig = false;
	static bool isNineCrossDetectionFlagSmall=false;

	class IQMETRICS_API MLNineCrossDetection
	{
	public:
		MLNineCrossDetection();
		~MLNineCrossDetection();
		void setBinNum(int num);
		void setFOVType(FOVTYPE type);
		void setDetectionFlag(bool flag, FOVTYPE type);
		bool getDtectionFlag(FOVTYPE type);
		map<string, cv::Point2f> getNineCrosshairLocation(cv::Mat img, FOVTYPE crossType);
		map<string, cv::Point2f> getNineCrosshairCenter(cv::Mat img, FOVTYPE crossType);
		map<string, cv::Rect>getNineCrossRect();
		map<string, cv::Point2f> getNineCrossLocationPreLoc(cv::Mat img, map<string, cv::Point2f>cenMap);	
	private: 
		void saveCrossLocation(vector<cv::Point2f>pts, FOVTYPE type);
		map<string, cv::Point2f> readCrossLocation(cv::Mat img,FOVTYPE type);
		int m_ROILenBig = 2200;
		int m_ROILenSmall = 700;
	private:
		vector<double> xdegNine = { -1280,0,1280,-1280,0,1280,-1280,0,1280 };
		vector<double> ydegNine = { -1280,-1280,-1280,0,0,0,1280,1280,1280 };
		vector<double> xdegNineBig = { -5318,0,5318,-5318,0,5318,-5318,0,5318 };
		vector<double> ydegNineBig = { -3830,-3830,-3830,0,0,0,3830,3830,3830 };
		//static bool isNineCrossDetectionFlagBig;
		//static bool isNineCrossDetectionFlagSmall;
		map<string, cv::Rect>m_cenRectMap;
		FOVTYPE m_fovType;
		int m_binNum = 1;
	};
}

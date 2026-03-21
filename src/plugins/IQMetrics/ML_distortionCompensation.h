#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
namespace MLIQMetrics
{
	struct DistortionCompensationRe
	{
		cv::Mat corrtedImg;
		bool flag = true;
		string errMsg = "";
	};
	class IQMETRICS_API MLDistortionCompensation
	{
	public:
		MLDistortionCompensation();
		~MLDistortionCompensation();
		bool getDistortionCompensationMap(cv::Size s, cv::Mat &map_x, cv::Mat& map_y,double phi_x=3.02,double phi_y=-1.7);		
		DistortionCompensationRe getDistortionCompensationMapImg(cv::Mat img, cv::Mat map_x, cv::Mat map_y);
	};
}


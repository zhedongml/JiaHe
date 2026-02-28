#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
#include "CrossCenter.h"
#include "ml_image_public.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
namespace MLIQMetrics
{
	struct ColorUniformityRe
	{
		double deltx = 0;
		double delty = 0;
		cv::Mat imgdraw;
		bool flag = true;
		string errMsg = "";
	};
	class IQMETRICS_API MLColorUniformity :public MLImageDetection::MLimagePublic
	{
	public:
		MLColorUniformity();
		~MLColorUniformity();
	public:
		ColorUniformityRe getColorUniformity(cv::Mat x, cv::Mat y);
		ColorUniformityRe getColorUniformity(cv::Mat X, cv::Mat Y, cv::Mat Z);
		map<string, cv::Point2f>get13RectCenterSort(cv::Mat img, cv::Mat& imgdraw);
		vector<cv::Point2f>get13RectCenter(cv::Mat img, cv::Mat& imgdraw, int binNum);
	private:
		vector<double>xdeg = { -6,-6,-6,0,0,0,6,6,6,-10.05,-10.05,10.05,10.05 };
		vector<double>ydeg = { -5 ,0,5,-5,0,5,-5,0,5,-7.6,7.6,-7.6,7.6 };
		vector<double>xpixel = { -1703,-1703,-1703,0,0,0,1703,1703,1703,-2260,-2260,2260,2260 };
		vector<double>ypixel = { -1311,0,1311,-1311,0,1311,-1311,0,1311,-1628,1628,-1628,1628 };


	};
}

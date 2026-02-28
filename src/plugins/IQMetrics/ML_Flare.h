#pragma once
//#include "ml_image_public.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
namespace MLIQMetrics
{
	struct StrightXsecRe
	{
		double glowMedian = 0;
		double flareMedian = 0;
	};
	struct FlareSecRe
	{
		double glowMedianL = 0;
		double flareMedianL = 0;
		double glowMedianR = 0;
		double flareMedianR = 0;
	};
	struct FlareRe
	{
		map<string, vector<FlareSecRe>>flareMap;
		cv::Mat imgdraw;
		bool flag = true;
		string errMsg = "";
	};
	struct XSECRe
	{
		vector<double> xs_left;
		vector<double> ys_left;
		vector<double> xs_right;
		vector<double> ys_right;
		cv::Mat avg_xsec_left;
		cv::Mat avg_xsec_right;

	};
	class IQMETRICS_API MLFlare :public MLImageDetection::MLimagePublic
	{
	public:
		MLFlare();
		~MLFlare();
		FlareRe getFlare(const cv::Mat imgHDR);
		FlareRe getFlare(const cv::Mat imgAuto, const cv::Mat imgOver);
		vector<cv::Rect>detectFlareROI(cv::Mat img);
		vector<cv::Rect> detectCenterFlareROI(cv::Mat img);
		vector<cv::Rect>getSortedRect(vector<cv::Rect>rectVec);
	
		XSECRe straightXsec(cv::Mat imgOver, double pix2deg, cv::Point2f center,double xSecDist,cv::Mat &imgdraw);
		StrightXsecRe processStrightXsec(cv::Mat xSec,vector<double>flarePeaks, int baselineBuffer);
		cv::Mat calculateHDRImage(cv::Mat imgAuto, cv::Mat imgOver, vector<cv::Rect>rectVec);

	private:
		vector<cv::Scalar>m_color;
	};
}
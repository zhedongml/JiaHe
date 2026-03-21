#pragma once
#include "ml_image_public.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
namespace MLIQMetrics
{
	struct mtfRectRe
	{
		vector<double*>mtfCurve;
		vector<double*>freqCurve;
		vector<double>mtfVec;
		bool flag = true;
		string errMsg = "";
		int mtflen=0;
		cv::Mat imgdraw;
	};
	struct CrossMTFRe
	{
		map<string, vector<double>>mtfMap;
		map<string, mtfRectRe>mtfResult;
		cv::Mat mtfMat;
		cv::Mat imgdraw;
		bool flag = true;
		string errMsg = "";
	};
	struct CenterCrossHairMTFROIRe
	{
		vector<cv::Point>cenVec;
		cv::Mat imgdraw;
		bool flag = true;
		string errMsg = "";
	};

	class IQMETRICS_API MLCrossMTF : public MLImageDetection::MLimagePublic
	{
	public:
		MLCrossMTF();
		~MLCrossMTF();
	public:
		CenterCrossHairMTFROIRe getCenterCrossMTFROICenter(cv::Mat img, int offset);
		CrossMTFRe getCrossMTF(cv::Mat img);
		mtfRectRe getSingleCrossMTF(cv::Mat img);
		mtfRectRe calMtfRect(cv::Mat img,cv::Rect rect,cv::Mat &imgdraw);
		
	private:
		int m_resizeNum = 1;
		int m_binNum = 1;
		vector<string> strVec = { "h1","h2","v1","v2" };
	};
}


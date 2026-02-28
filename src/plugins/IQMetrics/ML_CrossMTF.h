#pragma once
#include "ml_image_public.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
#include"pipeline.h"
namespace MLIQMetrics
{

	struct mtfRectRe
	{
		vector<double*>mtfCurve;
		vector<double*>freqCurve;
		vector<double>mtfVecFreq1;
		vector<double>mtfVecFreq2;
		bool flag = true;
		string errMsg = "";
		int mtflen=0;
		cv::Mat imgdraw;
	};
	struct CrossMTFRe
	{
		map<string, vector<double>>mtfMap;
		map<string, vector<double>>mtfMapFreq2;
		map<string, mtfRectRe>mtfResult;
		double meanH = 0;
		double meanV = 0;
		double minH = 0;
		double minV = 0;
		double meanH2 = 0;
		double meanV2 = 0;
		double minH2 = 0;
		double minV2 = 0;
		cv::Mat mtfMapH;
		cv::Mat mtfMapV;
		cv::Mat mtfMapH2;
		cv::Mat mtfMapV2;		
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
		void setFOVType(FOVTYPE type);
		CenterCrossHairMTFROIRe getCenterCrossMTFROICenter(cv::Mat img, int offset);
		CrossMTFRe getCrossMTF(cv::Mat img);
		CrossMTFRe getCrossMTF(cv::Mat img, FOVTYPE crossType);
		mtfRectRe getSingleCrossMTF(cv::Mat img);
		mtfRectRe calMtfRect(cv::Mat img,cv::Rect rect,cv::Mat &imgdraw);
	private:
		cv::Mat convertVecToMat(const vector <double>mtfVec);
	private:
		int m_resizeNum = 1;
		int m_binNum = 1;
		vector<string> strVec = { "h1","h2","v1","v2" };
		//PipeLine* m_mtfPipeline= new PipeLine();
		FOVTYPE m_fovType;
	};
}


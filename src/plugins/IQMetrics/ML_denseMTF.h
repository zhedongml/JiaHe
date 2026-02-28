#pragma once
//#include "ml_image_public.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
//#include"pipeline.h"
#include"mtfpipeline/pipeline.h"
namespace MLIQMetrics
{
	struct DenseMTFRe
	{
		double medianH = 0;
		double p20H=0;
		double p75H = 0;
		double p25H = 0;
		double p75_25H = 0;
		double medianV = 0;
		double p20V = 0;
		double p75V = 0;
		double p25V = 0;
		double p75_25V = 0;				
		cv::Mat mtfMapH;
		cv::Mat mtfMapV;
		cv::Mat mtfMapH2;
		cv::Mat mtfMapV2;
		cv::Mat imgdraw;
		cv::Mat xPos;
		cv::Mat yPos;
		bool flag = true;
		string errMsg = "";
	};
	struct DenseMTFGridRe
	{
		double meanH = 0;
		double meanV = 0;
		double minH = 0;
		double minV = 0;
		cv::Mat mtfMapH;
		cv::Mat mtfMapV;
		cv::Mat mtfMapH2;
		cv::Mat mtfMapV2;
		cv::Mat imgdraw;
		cv::Mat xPos;
		cv::Mat yPos;
		bool flag = true;
		string errMsg = "";
	};
	struct CenterRects
	{
		cv::Rect rectTop;
		cv::Rect rectBottom;
		cv::Rect rectLeft;
		cv::Rect rectRight;	
		vector<cv::Rect>rectVec;
		cv::Mat imgdraw;
		bool flag = true;
		string errMsg = "";
	};

	class IQMETRICS_API MLdenseMTF :public MLImageDetection::MLimagePublic
	{
	public:
		MLdenseMTF();
		~MLdenseMTF();
	public:
		void setCheckerDegree(int deg);
		void setFOVType(FOVTYPE type);
		DenseMTFRe getDenseMTFChecker(cv::Mat img,int flag);
		DenseMTFGridRe getDenseMTFGrid(const cv::Mat img);
		CenterRects getGridCenterRects(cv::Mat img);
		cv::Mat getMtfMat(cv::Mat xlocmat, cv::Mat ylocmat, cv::Mat& img_draw,cv::Mat rawImg,int flag, MTF_TYPE type,int typeFlag=0);
		void getMtfMat(cv::Mat xlocmat, cv::Mat ylocmat, cv::Mat&imgdraw, cv::Mat rawImg, MTF_TYPE type,cv::Mat &mtfH,cv::Mat &mtfV);
		double calculateMtf(cv::Mat roi, MTF_TYPE type=SLANT);
		vector<double>calculateMtf1(cv::Mat roi, MTF_TYPE type);
		vector<double>calculateMtf2(cv::Mat roi, MTF_TYPE type,cv::Point2f c1);
		double calculateFOV(cv::Point2f pt);
		void getDenseMTFData(cv::Mat mtfH, cv::Mat mask, DenseMTFRe&re,int flag);
		double calculateMatMean(cv::Mat mat);
		double calculateMatMin(cv::Mat mat);

	private:
		cv::Mat calculateMTFHor(cv::Mat mtfh);
		cv::Mat calculateMTFVer(cv::Mat mtfv);

	private:
		cv::Mat m_maskH;
		cv::Mat m_maskV;
		cv::Mat m_mtfmapH2;
		cv::Mat m_mtfmapV2;
		cv::Point2f m_center;
		int m_binNum = 1;
		FOVTYPE m_fovType;
		//int  m_flag = 0;//0 grid, 1 2deg checker, 2 0.5deg checker
	};
}


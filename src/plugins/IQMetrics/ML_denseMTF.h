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
		double meanH_freq2 = 0;
		double meanV_freq2 = 0;
		double minH_freq2 = 0;
		double minV_freq2 = 0;

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
		void setIsSLB(bool flag);
		void setIsDisparityEyebox(bool flag);
		void setPatternCenter(cv::Point2f cen);
		DenseMTFRe getDenseMTFChecker(cv::Mat img);
		DenseMTFGridRe getDenseMTFGrid(const cv::Mat img);
		CenterRects getGridCenterRects(cv::Mat img);

		//TODO: test
		DenseMTFGridRe getDenseMTFGrid_CSV(cv::Mat img);
		CenterRects getGridRects(cv::Mat img, Point rowCol);
		CenterRects getRectMat(cv::Mat xlocMat, cv::Mat ylocMat, int flag, Point rowCol);

		cv::Mat getMtfMat(cv::Mat xlocmat, cv::Mat ylocmat, cv::Mat& img_draw,cv::Mat rawImg,int flag, MTF_TYPE type);
		void getMtfMat(cv::Mat xlocmat, cv::Mat ylocmat, cv::Mat&imgdraw, cv::Mat rawImg, MTF_TYPE type,cv::Mat &mtfH,cv::Mat &mtfV);
		double calculateMtf(cv::Mat roi, MTF_TYPE type=SLANT);
		vector<double>calculateMtf1(cv::Mat roi, MTF_TYPE type);

		double calculateFOV(cv::Point2f pt);
		void getDenseMTFData(cv::Mat mtfH, cv::Mat mask, DenseMTFRe&re,int flag);
		double calculateMatMean(cv::Mat mat);
		double calculateMatMin(cv::Mat mat);

	private:
		cv::Mat calculateMTFHor(cv::Mat mtfh);
		cv::Mat calculateMTFVer(cv::Mat mtfv);
		vector<double> getMTFRect(cv::Mat mtfMap,cv::Mat xloc, cv::Mat yloc, cv::Rect rect,vector<double>&mtfOutMask);
		void calculateMtfValue(cv::Mat mtfmapH, double &min, double &mean);
	private:
		//PipeLine* mtfPipeline = new PipeLine();
		cv::Mat m_maskH;
		cv::Mat m_maskV;
		cv::Mat m_mtfmapH2;
		cv::Mat m_mtfmapV2;
		cv::Point2f m_center;
		int m_binNum = 1;
		int m_len = 6000;
		string m_filepathx = "./config/ALGConfig/slbInfo/gridXLocMat.csv";
		string m_filepathy = "./config/ALGConfig/slbInfo/gridYLocMat.csv";
		bool m_IsSLB = true;
		bool m_isDisparityEyebox = false;
	};
}


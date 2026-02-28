#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
//#include"ml_image_public.h"
#include"ImageDetection/ml_image_public.h"
//using namespace algorithm;
namespace MLIQMetrics {
	class IQMETRICS_API IQMetricsParameters
	{
	public:
		static double magnification;
		static double pixel_size;
		static double FocalLength;
		static cv::Point2f opticalCenter;
		static double crArea;
        static double LuminaceActive;
        static double RolloffLength;
        static double RolloffLengthH;
        static double RolloffLengthV;
        static int RolloffWidth ;
		static double RolloffAreaRatio;
        static cv::Rect ROIRect;
        static double CTFROIWidth;
        static double CTFROIHeight;
		static int denseMTFWidth;
		static int denseMTFHeight;
		static int crossMTFWidth;
		static int crossMTFHeight;
		static int crossMTFWidthBig;
		static int crossMTFHeightBig;
		static int gridMTFWidth;
		static int gridMTFHeight;
		static int checkerMTFWidth_2;
		static int checkerMTFHeight_2;
		static int checkerMTFWidth_05;
		static int checkerMTFHeight_05;
		static int ROI_center_offset;
		static int ROI_center_offset_big;
		static double mtfFreq;
		static double mtfFreq2;
		static double zoneARadius;
		static vector<cv::Rect> ghostRectVec;
		static double CheckerDensity;
		static int crossBinNum;	
		static vector<double> xsecVec;
		static double xsecWidth;
		static int avg_width;
		static double flareAngle;
		static double distortonTheHor;
		static double distortonTheVer;
		static double distortonTheHorBig;
		static double distortonTheVerBig;
		static double CheckerRECTMaskHor;
		static double CheckerRECTMaskVer;
		static double CheckerCIRCLEMaskRadius;
		static int ResizeNum;
		static double DUTRotationAngle;
	};
	static struct ROIParaNew
	{
		float step = 0;
		float cenX = 0;
		float cenY = 0;
		float width = 30;
		float height = 30;
		int rotationAngle = 0;
		MLImageDetection::MirrorALG mirror = MLImageDetection::NO_OP;
	};
    //enum MirrorALG
    //{
    //    LEFT_RIGHT,
    //    UP_DOWN,
    //    NO_OP
    //};
	enum FOVTYPE
	{
		BIGFOV,
		SMALLFOV
	};
	class IQMETRICS_API IQMetricUtl
	{
	public:
		IQMetricUtl();
		~IQMetricUtl();
		static IQMetricUtl* instance();
	public:
		cv::Rect getRect(ROIParaNew para, cv::Point2f center);
		int getBinNum(cv::Size s);
		double getPix2Arcmin(cv::Size s);
		double getPix2Degree(cv::Size s);
		static bool isInitFromJson;
		void loadJsonConfig(const char* path);
		string fovTypeToString(FOVTYPE type);

	private:
		static IQMetricUtl* self;

	};
}


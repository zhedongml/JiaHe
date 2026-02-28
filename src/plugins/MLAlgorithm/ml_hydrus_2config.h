#pragma once
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<iostream>
#include "ml_image_public.h"
#define DEBUG 0
using namespace std;
using namespace cv;

 class ALGORITHM_API HydrusParameters
{
public:
    static double magnification;
    static double pixel_size;
    static double FocalLength;
    static cv::Point2f W_ND0_opticalcenter;
    static cv::Point2f W_ND1_opticalcenter;
    static cv::Point2f W_ND2_opticalcenter;
    static cv::Point2f W_ND3_opticalcenter;
    static cv::Point2f X_ND0_opticalcenter;
    static cv::Point2f X_ND1_opticalcenter;
    static cv::Point2f X_ND2_opticalcenter;
    static cv::Point2f X_ND3_opticalcenter;
    static cv::Point2f Y_ND0_opticalcenter;
    static cv::Point2f Y_ND1_opticalcenter;
    static cv::Point2f Y_ND2_opticalcenter;
    static cv::Point2f Y_ND3_opticalcenter;
    static cv::Point2f Z_ND0_opticalcenter;
    static cv::Point2f Z_ND1_opticalcenter;
    static cv::Point2f Z_ND2_opticalcenter;
    static cv::Point2f Z_ND3_opticalcenter;
    static cv::Rect ROI;
    static double roationAngle;
    static int roiStart;
    static int gridAlgorthm;// 1.houghline 2.lunkuo 3.corner 4.gradient
    static int deadPointsAlgorithm;
    static double crArea;
    static int chessAlgortithm;
    static int singlecheckerWidth;
    static int pointsClusters;
    static int xyClassification;
    static double LuminaceActive;
    static int downSampling;
    static int degrees;
    static int brightnesslevel;
    static int distortionAlgorithm;
    static int crosshairAlgortihm;
    static double W_ND0_baselineAngle;
    static double W_ND1_baselineAngle;
    static double W_ND2_baselineAngle;
    static double W_ND3_baselineAngle;
    static double X_ND0_baselineAngle;
    static double X_ND1_baselineAngle;
    static double X_ND2_baselineAngle;
    static double X_ND3_baselineAngle;
    static double Y_ND0_baselineAngle;
    static double Y_ND1_baselineAngle;
    static double Y_ND2_baselineAngle;
    static double Y_ND3_baselineAngle;
    static double Z_ND0_baselineAngle;
    static double Z_ND1_baselineAngle;
    static double Z_ND2_baselineAngle;
    static double Z_ND3_baselineAngle;
    static int roioffset;
    static int roiwidth;
    static int roiheight;
    static double RolloffLength;
    static int RolloffWidth;
    static double kET;
    static double bET;
 };

namespace algorithm
{
    struct Newpara
    {
        cv::Rect rect;
        int binNum = 1;
        double pixel_size = 4.6e-3;
    };
    class ALGORITHM_API MLHydrusConfig
    {
    public:
        MLHydrusConfig();
      ~MLHydrusConfig();
    public:
        static bool isInitFromJson;
        void loadJsonConfig(const char* path);
        Newpara GetNewPara(cv::Mat img);
        Newpara m_para;
    };
}


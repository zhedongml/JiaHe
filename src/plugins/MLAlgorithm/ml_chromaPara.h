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
#include "CrossCenter.h"
#include "ml_hydrus_2config.h"
using namespace std;
using namespace cv;
namespace algorithm {
    struct WhitePointRe
    {
        double dCmin = 0;
        double dCmax = 0;
        double dCavg = 0;
        double dCmean = 0;
        double aMean = 0;
        double bMean = 0;
        double LMean = 0;
        double color_uniformity = 0;
        cv::Mat dCmn;
        cv::Mat lMat;
        cv::Mat aMat;
        cv::Mat bMat;

    };
    struct LuminanceRe
    {
        vector<double> MinMaxAvg;
        vector<double> MinMaxPer;
        double rectMean = 0;
        double rectCov = 0;
        double uniformity = 0;
    };
    struct LumiEfficencyRe
    {
        double efficicncy = -1;
        cv::Mat efficiencyMat;
        cv::Mat slb_draw;
        ALGResult flag;
    };
    struct SeqCRRe
    {
        cv::Mat Roicr;
        double seqCR = 0;
        cv::Mat seq90Mat;
        cv::Mat ColorROIMat;
        cv::Mat BlackROIMat;
    };
    struct RGBXYZ
    {
        std::vector<cv::Mat>m_xy_R;
        std::vector<cv::Mat>m_xy_G;
        std::vector<cv::Mat>m_xy_B;
    };
    struct ChromaRe
    {
        cv::Mat Chromax;
        cv::Mat Chromay;
    };
    //struct FOVInf
    //{
    //    float centerX = 0;
    //    float centerY = 0;
    //    std::vector<double> Vline;
    //    std::vector<double> Hline;
    //};

    class ALGORITHM_API MLChromaPara:public MLimagePublic
    {
    public:
        MLChromaPara();
        ~MLChromaPara();
    public:
        // white color
        void SetFovInform(float centerX, float centerY, std::vector<double> rotateH, std::vector<double> rotateV);
        void SetRotationDeg(float degree);
        void SetXYZImage(cv::Mat X, cv::Mat Y, cv::Mat Z);
        WhitePointRe GetCIELABCoordinates();
        double GetColorDistance();
        double GetColorDistanceAvg();
        double GetColorDistanceMax();
        void changeWhitepointBrightnesslevel(double val);
    //private:
      public:
        WhitePointRe m_whitePointRe;
        // seqcr
        void SetWRGBHImage(Mat img_WRGB, Mat img_black);
        SeqCRRe GetSequentialContrast();
        double GetSequentialContrastAverage();
        cv::Mat GetColorROIMat();
        cv::Mat GetBlackROIMat();

    //private:
      public:
        cv::Mat m_img_WRGB;
        cv::Mat m_img_black;
        SeqCRRe m_Secr;
        //luminance 
        void SetcolorIMG(cv::Mat img_color);
        LuminanceRe GetLuminanceUniformity();
        LumiEfficencyRe GetLuminanceEfficiency(cv::Mat img,string color,float angle);
        std::vector<double> GetLuminanceMinMaxPer();
        std::vector<double> GetLuminanceMinMaxAvg();
        std::vector<double> GetLuminanceRolloff();
        void changeRollOffFOVLength(double val);

        ALGResult getSLBFilePath(string &filePath, string color);
        cv::Rect getLumiEfficiencyROI(cv::Mat img,float angle);


    //private:
      public:
        cv::Mat m_img_color;
        LuminanceRe m_luminance;
        // colorGamut
        void SetcolorMat(std::vector<cv::Mat>R, std::vector<cv::Mat>G, std::vector<cv::Mat>B);
        void SetChromaMat(cv::Mat Colorx, cv::Mat Colory);
        double GetColorGamut();
        double ColorGamut(RGBXYZ m_rgbXYZ);
        ChromaRe ChromaMat(cv::Mat ColorX, cv::Mat ColorY);
        cv::Mat GetChromax();
        cv::Mat GetChromay();
// get imgdraw
        cv::Mat getImgDraw();
        cv::Mat getColorDraw();
        cv::Mat getBlackDraw();
        cv::Mat getChromaxDraw();
        cv::Mat getChromayDraw();
    private:
       cv::Mat m_Colorx, m_Colory;
       RGBXYZ m_rgbxy;
       ChromaRe m_Chroma;
       cv::Mat m_Chromax, m_Chromay, m_color_draw, m_black_draw;
    private:
        WhitePointRe whitePoint(cv::Mat X, cv::Mat Y, cv::Mat Z);
        SeqCRRe SequentialContrast(Mat img_WRGB, Mat img_black);
        cv::Mat getROIsMeanMat(cv::Mat img, cv::Mat& Area90Mat, double bin, cv::Rect rect = cv::Rect(0, 0, -1, -1));
        std::vector<cv::Rect> Findrois(Mat img, cv::Rect& Area90Rect, double bin,cv::Rect rect = cv::Rect(0, 0, -1, -1));
        std::vector<cv::Rect> FindroisBorder(Mat img, cv::Rect &Area90Mat, double bin);
        LuminanceRe LuminanceUniformity(Mat img);
        cv::Point2f getcoordinate(std::vector<cv::Mat>A, cv::Point2f center, double bin, cv::Rect rect);
        std::vector<double> LuminanceRolloff(cv::Mat img);
        std::vector<double> LuminanceRolloffRotation(cv::Mat img);
        cv::Rect findborder(cv::Mat img, cv::Rect rect = cv::Rect(0, 0, -1, -1));
        cv::RotatedRect getSolidBorder(cv::Mat img, cv::Rect &rect);

    public:
        cv::Mat getSolidImgRotated(cv::Mat img);

    private:
        cv::Mat m_X, m_Y, m_Z;
      //  WhitePointRe m_whitePointRe;
        cv::Mat m_img_draw;
        Newpara m_para;
        MLHydrusConfig hydrusConfig;
        cv::Point2f m_center;
        std::vector<double> m_Vline;
        std::vector<double> m_Hline;      
        float m_rotationAngle = 0;
        vector<cv::Rect> m_RoisRect;
        cv::Rect m_Area90Rect;
    };
}


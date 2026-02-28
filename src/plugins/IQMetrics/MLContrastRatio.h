#pragma once

#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
#include<opencv2/opencv.hpp>
#include<iostream>
#include"IQMetricUtl.h"
#include"ImageDetection\MLCherkerboardDetect.h"
namespace MLIQMetrics {
    enum MaskType {
        NOMASK,
        RECT,
        CIRCLE
    };
    struct ContrastRatioRe
    {
        double minCR = 0;
        double maxCR = 0;
        double meanCR = 0;
        cv::Mat crMat;
        cv::Mat greyLevelP;
        cv::Mat greyLevelN;
        vector<double>crMask;
        vector<double>crOutMask;
        cv::Mat imgdrawP;
        cv::Mat imgdrawN;
        MLImageDetection::CheckerboardRe checkerRe;
        bool flag = true;
        std::string errMsg = "";
    };
    struct ContrastRatioReGuSu
    {
        double meanZoneA = 0;
        double minZoneA = 0;
        double maxZoneA = 0;
        double meanZoneB = 0;
        cv::Mat greyLevelP;
        cv::Mat greyLevelN;
        cv::Mat crMat;
        cv::Mat imgdrawP;
        cv::Mat imgdrawN;
        bool flag = true;
        std::string errMsg = "";
    };
    struct SingleCheckerCRRe
    {
        double cr;
        cv::Mat grayMat;
        cv::Mat imgdraw;
        cv::Size boardSize;
        bool flag = true;
        string errMsg = "";
    };

    class IQMETRICS_API MLContrastRatio:public MLImageDetection::MLimagePublic 
    {
    public:
        MLContrastRatio();
        ~MLContrastRatio();
    public:
        void setFOVType(FOVTYPE type);
        void setIsUpdateSLB(bool flag);
        ContrastRatioRe getContrastRatio(const cv::Mat imgP, const cv::Mat imgN,bool isSLB=true, MaskType type=NOMASK);
        ContrastRatioReGuSu getContrastRatioGuSu(cv::Mat imgP, cv::Mat imgN);
        SingleCheckerCRRe getSingleCheckerCR(cv::Mat img);

    private:
        std::vector<double> CalculateROIValue(cv::Mat img, vector<cv::Rect> roiVec);
        std::vector<double> CalculateCR(vector<double> posValue, vector<double> negValue);
        double CalculateSingleCR(vector<double>posValue);

        cv::Rect updateRectByBinNum(cv::Rect rect, int binNum);
        cv::Mat updateMatValue(vector<double>crVec,cv::Size boardSize);
        void updateImgdraw(vector<cv::Rect>rectVec,vector<double>posV,vector<double>negV,
            vector<double>crV,cv::Mat &img_drawP,cv::Mat &img_drawN);
        void updateImgdraw(cv::Rect rect, cv::Mat &img_drawP, cv::Mat &img_drawN);
        void updateImgdraw(vector<cv::Rect>rectVec, vector<double>posV, cv::Mat& imgdraw);
        vector<double>getROICR(cv::Rect, vector<cv::Rect>rectVec, vector<double>crVec);
        vector<double>getROICR(vector<cv::Rect>rectVec, cv::Point2f cen, vector<double>crVec,vector<double>&crVecZoneB);
        vector<double>getROICR(vector<cv::Rect>rectVec, cv::Rect rect, vector<double>crVec, vector<double>& crVecZoneB);

    public:
        double getHarmonicMean(vector<double>crVec);
        // dut cr calibrated by slb
        void setColor(string color);
        void setEyeboxLocation(string eyebox);
        void readCRMatFromCSV(cv::Mat& crmat, string color, string id);
        void readCRMatFromCSV(cv::Mat& crmat, string color, string id,FOVTYPE type);
        void saveSLBCRMatToCSV(const cv::Mat crmat, string color, string id, FOVTYPE type);

        void readCRMatFromCSV(cv::Mat& crmat, string color);

        void readROICRFromCSV(vector<double>& crVec, string color, string id);
        cv::Mat updateDutCRBySlbCR(cv::Mat dutcr, cv::Mat slbcr);
        vector<double> updateDutCRBySlbCR(vector<double>dutcr, vector<double>slbcr);
        cv::RotatedRect getCherkerBorder(cv::Mat img, cv::Rect &rect); 
        cv::Mat preProcess(cv::Mat img);
        

    public:
        int m_binNum = 1;
        string m_slbCRDir = "//JsonFile//AlgJson//SLBCR//";
        string m_color="w";
        string m_eyebox="Left";
        FOVTYPE m_fovType;
        bool m_IsUpdateSLB = false;
    };
}


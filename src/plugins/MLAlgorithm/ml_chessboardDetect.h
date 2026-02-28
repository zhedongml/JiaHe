#pragma once
#ifndef MLALGORITHM_MLCHESSBOARDDETECT_H_
#define MLALGORITHM_MLCHESSBOARDDETECT_H_
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <vector>
#include "ml_image_public.h"
#include "ml_hydrus_2config.h"
using namespace cv;
using namespace std;
namespace algorithm
{
    struct ChessboardcrRe
    {
        double cr = 0;
        cv::Mat CRAll;
        cv::Mat greyLevelP;
        cv::Mat greyLevelN;
        cv::Size boardSize;
        ALGResult result;
    };
    class ALGORITHM_API MLOptParaChessboard : public MLimagePublic
    {

    public:
        MLOptParaChessboard();
        ~MLOptParaChessboard();
    public:
        void SetChessboardImage(cv::Mat imgP, cv::Mat imgN);
        void SetExposureTime(float time);
        void SetChessboardSize(cv::Size size);
        void SetRotationDeg(float degree);
        void SetROIRect(cv::Rect ROIRect);
        ChessboardcrRe GetContrastChessboard();
        double GetChessboradCr();
        void setChessboardCr(ChessboardcrRe re);
        cv::Mat GetRoiCR();
        cv::Mat getImgDrawPos();
        cv::Mat getImgDrawNeg();
        cv::Mat GetGreyLevelP();
        cv::Mat GetGreyLevelN();
        cv::Mat getImgDraw();
    public:
        ChessboardcrRe ContrastChessboard(cv::Mat imgP, cv::Mat imgN);

    public:
        vector<cv::Point2f> detectChessboardCorner(cv::Mat img, cv::Mat& indexMap,int binNum=4);
        vector<cv::Point2f> detectChessboardHoughLine(cv::Mat img, cv::Mat& indexMap);
        vector<cv::Point2f> detectChessboardTemplate(cv::Mat img, cv::Mat& indexMap);
        vector<cv::Point2f> detectChessboardContour(cv::Mat img, cv::Mat& indexMap,int binNum=4);
        vector<cv::Point2f> detectChessboardTemplate1(cv::Mat img, cv::Mat &indexMap);
        vector<cv::Point> matchTemplateMaxLocs(cv::Mat img, cv::Mat templ, double score = 0.7);

        vector<cv::Rect> getCRROI(vector<cv::Point2f>corners, cv::Mat indexMap);
    private:
        bool getNewROIRect(vector<Rect> roiVecP, vector<Rect> roiVecN, vector<Rect>& NewroiVecP, vector<Rect>& NewroiVecN);
        bool ReoderROIVec(vector<Rect> roiVecP, vector<Rect> roiVecN, vector<Rect>& NewroiVecP,
            vector<Rect>& NewroiVecN);
        int findNegtiveNum(vector<int> list);
        int findMapNegNum(cv::Mat list);
        bool searchColIndex(cv::Mat indexMap, int& start, int& end);
        bool searchRowIndex(cv::Mat indexMap, int& start, int& end);
        cv::Mat updateIndexMap(cv::Mat indexMap, vector<cv::Point2f> cornersPoint,
            vector<cv::Point2f>& cornersPointNew);
    public:
        cv::Mat updateIndexMapKL(cv::Mat indexMap, vector<cv::Point2f> cornersPoint,
            vector<cv::Point2f>& cornersPointNew);
    private:
        bool getXYcoordinate(cv::Mat indexMap, vector<cv::Point2f> cornersPoint, cv::Mat& x, cv::Mat& y);
        bool getCenter(cv::Mat ip, cv::Mat& p, cv::Mat& px1, cv::Mat& px2, cv::Mat& px3, cv::Mat& px4);
        cv::Mat getArea(cv::Mat x1, cv::Mat x2, cv::Mat x3, cv::Mat x4, cv::Mat y1, cv::Mat y2, cv::Mat y3, cv::Mat y4);
        int findNearstList(vector<cv::Rect> roiVecN, cv::Rect temp);
        std::vector<double> CalculateROIValue(cv::Mat img, vector<cv::Rect> roiVec);
        std::vector<double> CalculateCR(vector<double> posValue, vector<double> negValue);
        cv::Rect getChessboardRect(cv::Mat roi,cv::RotatedRect&rectR);
        cv::Mat getCheckboardImgRotated(cv::Mat img);
      private:
        cv::Mat m_imgP;
        cv::Mat m_imgN;
        ChessboardcrRe m_ChessboardCR;
        cv::Rect m_rect = cv::Rect(2636, 1600, 2868, 2868);
        cv::Mat m_img_drawP;
        cv::Mat m_img_drawN;
        double crAreaPercent = 0.8;
        MLHydrusConfig hydrusConfig;
        Newpara m_para;
        cv::Size m_boardsize = cv::Size(7, 7);
        cv::Mat m_img_draw;
        float m_rotationAngle=0;
        float m_exposureTime = -1;
    };
} // namespace algorithm
#endif
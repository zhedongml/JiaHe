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
namespace algorithm
{
	struct  CheckerboardRe
	{
        vector<cv::Point2f>pts;
        vector<cv::Rect>rectVec;
        cv::Point2f center;
        cv::Size boardSize;
        cv::Mat img_draw;
        bool flag = true;
        string errMsg = "";
	};
    class ALGORITHM_API MLCherkerboardDetect :MLimagePublic
	{

	public:
		MLCherkerboardDetect();
		~MLCherkerboardDetect();
	public:
		void SetChessboardSize(cv::Size size);
		void SetCRAreaPercent(double percent);

        CheckerboardRe detectChessboardCorner(cv::Mat img,double craRatio,   int binNum = 1);
        CheckerboardRe detectChessboardCorner1(cv::Mat img, double crRatio,   int binNum = 1);
        CheckerboardRe detectChessboardHoughLine(cv::Mat img, double crRatio, int binNum = 1);
        CheckerboardRe detectChessboardTemplate(cv::Mat img, double crRatio, int binNum = 1);
        CheckerboardRe detectChessboardContour(cv::Mat img, double crRatio, int binNum = 1);
        CheckerboardRe detectChessboardTemplate1(cv::Mat img, double crRatio, int binNum = 1);
		vector<cv::Point2f> matchTemplateMaxLocs(cv::Mat img, cv::Mat templ, double score = 0.7, int bin = 1);
		vector<cv::Rect> getCRROI(vector<cv::Point2f>corners, cv::Mat indexMap, vector<cv::Point2f>& centerVec);
    private:
        bool getNewROIRect(vector<cv::Rect> roiVecP, vector<cv::Rect> roiVecN, vector<cv::Rect>& NewroiVecP, vector<cv::Rect>& NewroiVecN);
        bool ReoderROIVec(vector<cv::Rect> roiVecP, vector<cv::Rect> roiVecN, vector<cv::Rect>& NewroiVecP,
            vector<cv::Rect>& NewroiVecN);
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
        double getFOVAngle(double pixel, int binNum);
        void updatePts(vector<cv::Point2f>& pts, cv::Point2f c0);
        void updateImgdraw(cv::Mat &imgdraw, vector<cv::Point2f>pts);
        cv::Point2f getPtsCenter(vector<cv::Point2f>pts);

    private:
        cv::Rect m_rect = cv::Rect(0, 0, -1, -1);
        double m_CRAreaPercent = 0.1;
        int m_chessAlgortithm = 1;
        int m_pointsClusters = 50;
        int m_xyClassification = 150;
        int m_singlecheckerWidth = 100;
        cv::Size m_boardsize = cv::Size(5, 5);
        cv::Mat m_img_draw;
        int m_binNum = 1;
	};
}
#endif
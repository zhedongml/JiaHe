#pragma once

#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "ml_image_public.h"
#include"CrossCenter.h"
//#include "ml_luminance.h"
#include"IQMetricUtl.h"

//using namespace cv;
using namespace std;

namespace MLIQMetrics {
    typedef struct CRARE
    {
        double angleX = 0;
        double angleY = 0;
        double angle = 0;
        cv::Point2f center;
        bool flag = true;
        string errMsg = "";
    }CRARE;
    typedef struct NineCRARE
    {
        map<string, CRARE> mapCRA;
        vector<cv::Point2f>cenVec;
        bool flag = true;
        string errorMsg = "";
    }NineCRARE;

    class IQMETRICS_API MLCRA :public MLImageDetection::MLimagePublic
    {
    public:
        MLCRA();
        ~MLCRA();

    public:
        CRARE getCRA(cv::Mat img);
        CRARE getCRA(cv::Mat img, int roationAngle, MLImageDetection::MirrorALG mirror = MLImageDetection::NO_OP);
        CRARE getMultiCrossCRA(cv::Mat img, int roationAngle, int eyeLoc, MLImageDetection::MirrorALG mirror = MLImageDetection::NO_OP);

        NineCRARE getNineCRA(cv::Mat img, int roationAngle, MLImageDetection::MirrorALG mirror = MLImageDetection::NO_OP);
        CRARE calculateCRA(cv::Mat img, cv::Rect rect, cv::Point2f opticalCenter);

        cv::Mat getImgDraw();
        double getCrossHaireRotationAngle();
        double getAngleX();
        double getAngleY();
        double getAngle();
        double getXdistance();
        double getYdistance();
        double getdistance();

        int m_binNum = 1;
    private:
        cv::Rect getTheCenterCrossRect(cv::Mat img);

    private:
        double m_angleX = 0;
        double m_angleY = 0;
        double m_angle = 0;
        double m_H = 0;
        double m_V = 0;
        double m_distance = 0;
        double m_RotationAngle = 0;
        cv::Mat m_img_draw;
        double m_pixel = 4.6e-3;
        double m_focallength = 24.97;
        cv::Point2f m_basePoint = cv::Point2f(3962, 3083);
    };
}



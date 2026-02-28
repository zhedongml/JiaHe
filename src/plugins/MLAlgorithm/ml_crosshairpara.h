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
//using namespace cv;
 namespace algorithm 
{
     struct FovAngleRe
     {
         double FovH = 0;
         double FovV = 0;
         double FovDiag = 0;
         std::vector<cv::Point2f> EdgePoint;
         cv::Rect roiRect;
     };
     struct FovOffestRe
     {
         double V = 0;
         double H = 0;
         double D = 0;
         double pixelx = 0;
         double pixely = 0;
         vector<cv::Point2f> crossCenter;
     };
     struct FOVInf
     {
         float centerX = 0;
         float centerY = 0;
         std::vector<double> Vline;
         std::vector<double> Hline;
     };
     struct imgRGB
     {
         cv::Mat imgR;
         cv::Mat imgG;
         cv::Mat imgB;
     };
     struct LaterColorRe
     {
         cv::Mat RGmat;
         cv::Mat BGmat;
         cv::Mat RBmat;
         double avgRG = -1;
         double avgBG = -1;
         double avgRB = -1;
         double maxRG = -1;
         double maxBG = -1;
         double maxRB = -1;
     };

    class ALGORITHM_API MLCrosshairPara:public MLimagePublic
    {
    public:
        MLCrosshairPara();
        ~MLCrosshairPara();
    public:
        void SetMultCrosshairImage(Mat img_multcross);
        FovOffestRe GetBoresight();
        FovAngleRe GetFOVAngle();
        double GetcrossRotation();

        cv::Mat getImgDraw();
        float getCrosshairRotationAngle();
        vector<cv::Point2f> getNineCrosshairMTFROICenter(cv::Mat img);


        void BoresightBatchTest();
       
        void SetRGBImg(cv::Mat imgR, cv::Mat imgG, cv::Mat imgB);
        LaterColorRe GetLateralColorAvg();
        std::vector<cv::Mat> GetLateralColorMat();
        cv::Mat GetLateralColorMax();
        cv::Mat getImgRDraw();
        cv::Mat getImgGDraw();
        cv::Mat getImgBDraw();
        vector<cv::Point2f> getRotationPoints();

        // not sure,need or not
        void SetFovInform(float centerX, float centerY, std::vector<double> rotateH, std::vector<double> rotateV);
        FOVInf GetFovInform();
        vector<cv::Point2f> GetFovEdgePoints();
        cv::Rect findborder(cv::Mat img, cv::Rect rect = Rect(0, 0, -1, -1));
        cv::Rect findborderHoughLine(cv::Mat img, cv::Rect rect = Rect(0, 0, -1, -1));
    public:
        FovOffestRe Boresight(cv::Mat img);
        FovOffestRe Boresight5(cv::Mat img);
        FovOffestRe BoresightNoBorder(cv::Mat img);
        FovOffestRe BoresightNoBorder(cv::Mat img, int roationAngle, MirrorALG mirror);
        void setIsSLB(bool flag);

        FovAngleRe FOVAngle(cv::Mat img);
        FovAngleRe FOVAngleSolid(cv::Mat img);
        FovAngleRe FOVAngleCross(cv::Mat img);
        bool findEdgePts(cv::Mat data, int flag, cv::Point2f &p1, cv::Point2f &p2);
        vector<int>findPeaks(cv::Mat data, double minHeight, int minWidth,double minProminence);

        double crossRotation(cv::Mat img);
        double crossRotationNoBorder(cv::Mat img);
        LaterColorRe LateralColor(imgRGB img);
        //std::vector<cv::Mat> ImageSegmentation(cv::Mat img);
        cv::Point getendpointTemplate(cv::Mat img, int i);
        cv::Point2f getendpointTest(cv::Mat img);
        cv::Point2f findPointEnd(cv::Mat img);

        cv::Point getendpointHoughLine(cv::Mat img, int i);
        std::vector<cv::Point2f>Calculatecrosscenter(cv::Mat img, cv::Rect rect = cv::Rect(0, 0, -1, -1));
        cv::Mat CalculateCrosshairAngDis(vector<cv::Point2f> cenVecR, vector<cv::Point2f> cenVecG, double& avgRG,
            double& maxRG);

      public:
        std::vector<cv::Mat> ImageSegmentation(cv::Mat img, vector<cv::Rect> &rectVec);
        std::vector<cv::Mat> ImageSegmentationTemplate(cv::Mat img,vector<cv::Rect>&rectVec);
        std::vector<cv::Mat> ImageSegmentationTemplate1(cv::Mat img, vector<cv::Rect> &rectVec);

    private:
        cv::Mat m_imgCrosshair;
        cv::Mat m_img_draw;
        cv::Mat m_imgR_draw;
        cv::Mat m_imgG_draw;
        cv::Mat m_imgB_draw;
        MLHydrusConfig hydrusConfig;
    private:
        imgRGB m_imgRGB;
        cv::Point2f m_center = { 0, 0 };
        std::vector<Point> m_roi_center;
        std::vector<double> m_Vline;
        std::vector<double> m_Hline;
        double m_sensorFovH = 1;
        double m_sensorFovV = 1;
        Newpara m_para;
        FovAngleRe m_fov;
        FovOffestRe m_fovoff;
        LaterColorRe m_laterColarRe;
        float m_rotationAngle = 0;
        double m_dis = 900;
        bool isSLB = false;
        vector<cv::Point2f> m_rotaionPts;
    };

}

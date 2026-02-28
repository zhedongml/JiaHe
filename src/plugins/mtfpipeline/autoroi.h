#pragma once
#if defined(MTFPIPELINE_EXPORTS)
#  define MTFPIPELINE_EXPORT __declspec(dllexport)
#else
#  define MTFPIPELINE_EXPORT __declspec(dllimport)
#endif
#include "MLAlgorithm/ml_cross_center.h"
#include "harris.h"
#include <vector>
struct ClusterPoint {
    cv::Point2d pt;
    int count = 0;
};
struct ChromaticAberration {
    int id = 0;
    int pointx = 0;
    int pointy = 0;
    int diffx = 0;
    int diffy = 0;
};
struct AutoROIOption {
    int type = 1;
    int count = 4;
    bool applytoallImg = false;
    int width = 90;
    int height = 90;
    int offset = 30;
    bool closeCenter = true;
    int AutoROIDir = 1; //1:vertical  0:horizontal 
};
class MTFPIPELINE_EXPORT AutoROI
{
public:
    double cal_distance(cv::Point2d pt1, cv::Point2d pt2);
    std::vector<ClusterPoint> clusterpoint(std::vector<cv::Point2d> pts, int thresh = 50);
    int findShortestDis(cv::Point2d pt, std::vector<cv::Point2d> pts, int type = 0); //type: x direct 0,y direct 1
    int findShortestDis2(cv::Point2d pt, std::vector<ClusterPoint> pts, int type = 0); //type: x direct 0,y direct 1
    std::vector<cv::Rect> chessboardAutoROI(cv::Mat& img, AutoROIOption& opt);
    std::vector<cv::Rect> crosscenterAutoROI(cv::Mat &img, AutoROIOption& opt);
    std::vector<cv::Rect> dashlinechessboardAutoROI(cv::Mat& img, AutoROIOption& opt);
    std::vector<cv::Rect> getAutoROIs(cv::Mat &img,cv::Point2d pt, AutoROIOption& opt,int xthreshhold = INT16_MAX, int ythreshhlod = INT16_MAX);
    std::vector<cv::Rect> findAutoROIs(cv::Mat img,int type=0, int count = 4, int roiwidth = 80, int roiheight = 80, int offset = 30,bool isThroFocus=false,int roiDir=1);

    int findClosestCorner(cv::Point2d centerPt, std::vector<cv::Point2d>& pts);
    int findClosestCorner2(cv::Point2d centerPt, std::vector<ClusterPoint>& pts);
    std::vector<cv::Rect> getClosestCenterROI(cv::Mat& img, cv::Point2d pt, AutoROIOption& opt, int xthreshhold = INT16_MAX, int ythreshhlod = INT16_MAX);

    ChromaticAberration* autoChromaticAberration(cv::Mat img1, cv::Mat img2, cv::Mat img3);
    int findClosestCornerIdx(cv::Point2d centerPt, std::vector<ClusterPoint>& pts);
    cv::Point2d getChroAberratCornnerPoint(cv::Mat cvImg, cv::Point2d nearPt = cv::Point2d());
    cv::Mat convet16bitTo8bit(cv::Mat& rawimg);
private:
    double scaled = 0.5;
    int roiwidth = 200;
    int roiheight = 200;
};


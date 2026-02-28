#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
//#include "ml_luminance.h"
#include"IQMetricUtl.h"
namespace MLIQMetrics
{
struct FovRe
{
    double FovH = 0;
    double FovV = 0;
    double FovDiag = 0;
    std::vector<cv::Point2f> EdgePoint;
    cv::Mat imgdraw;
    bool flag = true;
    string errMsg = "";
};
class IQMETRICS_API MLFOV:public MLImageDetection::MLimagePublic
{
  public:
    MLFOV();
    ~MLFOV();

  public:
      void setFOVType(FOVTYPE type);
      FovRe getFOVSolid(const cv::Mat img);
    void setRotaionxAngle(double angle);

  private:
    double calculateFOV(cv::Point2f pt1, cv::Point2f pt2,int binNum);
    bool findEdgePts(cv::Mat data, int flag, cv::Point2f &p1, cv::Point2f &p2);
    bool findEdgePts1(cv::Mat data, int flag, cv::Point2f& p1, cv::Point2f& p2);

    vector<int> findPeaks(cv::Mat data, double minHeight, int minWidth, double minProminence);
    void updateImgdraw(cv::Mat &imgdraw, cv::Point2f pt1, int binNum);
  private:
    double m_rotationAngle = 0;
    int m_len = 200;
    FOVTYPE m_fovType;
};
} // namespace MLIQMetrics

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
struct RotationRe
{
    double angle;
    vector<cv::Point2f> rotaionPts;
    cv::Mat imgdraw;
    bool flag = true;
    string errMsg = "Successfully";
};
class IQMETRICS_API MLRotation:public MLImageDetection::MLimagePublic
{
  public:
    MLRotation();
    ~MLRotation();

  public:
      void setIsUpdateSLB(bool flag);
      void setIsSLB(bool flag);
    void setFOVType(FOVTYPE type);
    void setColor(string color);
    RotationRe getGridRotation(cv::Mat img);
    RotationRe getCrossRotationGusu(cv::Mat img);
    RotationRe getNineCrossRotation(const cv::Mat img);
    RotationRe getMultiCrossRotation(cv::Mat img, int eyeLoc);

  private:
    void updateGridPoint(cv::Point2f &cen, cv::Mat img);
    void updateImgdraw(cv::Mat &imgdraw, cv::Point2f pt,int binNum);
    void updateImgdraw(cv::Mat& imgdraw, cv::Point2f pts1, string str, int binNum);
    void updateRotationRe(double& rotation, bool isSLB);

  private:
    cv::Mat m_imgdraw;
    bool m_IsSLB = true;
    int m_crossROI = 1000;
    int m_gridROI = 200;
    bool accurateFlag = true;
    int m_binNum = 1;
    FOVTYPE m_FOVType;
    string m_color;
    bool m_IsUpdateSLB = false;
};
} // namespace MLIQMetrics

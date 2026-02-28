#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
namespace MLIQMetrics
{
struct LumiEfficencyRe
{
    double efficicncy = -1;
    double p5=0;
    double p50 = 0;
    double p95 = 0;
    cv::Mat efficiencyMat;
    cv::Mat slb_draw;
    bool flag = true;
    string errMsg = "";
    cv::Mat imgdraw;
};
class IQMETRICS_API MLEfficiency:public MLImageDetection::MLimagePublic
{
  public:
    MLEfficiency();
    ~MLEfficiency();
    static MLEfficiency *instance();

  public:
      void setFOVType(FOVTYPE type);
    LumiEfficencyRe GetLuminanceEfficiency(cv::Mat img, string color, float angle, int eyeLoc);
    LumiEfficencyRe GetLuminanceEfficiency(cv::Mat img, string color);
    LumiEfficencyRe getLuminanceEfficiency(cv::Mat img, string color);

    cv::Mat getSolidImgRotated(cv::Mat img,cv::Rect&rect);
    cv::RotatedRect getSolidBorder(cv::Mat img, cv::Rect &rect);
    cv::Rect getLumiEfficiencyROI(cv::Mat img, float angle);

  private:
    cv::Mat preProcess(cv::Mat img);
    double readSLBLuminance(string color);

  private:
    double m_rotationAngle = 0;
    static MLEfficiency *effSelf;
    FOVTYPE m_fovType;
};
} // namespace MLIQMetrics

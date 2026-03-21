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

    //double p5_disp = 0;
    //double p95_disp = 0;
    //double p50_disp = 0;
    //double efficicncy_disp = -1;
    double efficicncy = -1;
    double p5=0;
    double p50 = 0;
    double p95 = 0;
    cv::Mat efficiencyMat;
    double efficicncyLum = -1;
    double p5Lum = 0;
    double p50Lum = 0;
    double p95Lum = 0;


    cv::Mat efficiencyMatLum;
    cv::Mat slb_draw;
    bool flag = true;
    string errMsg = "";
    MLImageDetection::ALGResult flag1;
    cv::Mat imgdraw;
};
class IQMETRICS_API MLEfficiency:public MLImageDetection::MLimagePublic
{
  public:
    MLEfficiency();
    ~MLEfficiency();
    static MLEfficiency *instance();
    void setIsDisparityEyebox(bool flag);
  public:
    LumiEfficencyRe GetLuminanceEfficiency(cv::Mat img, string color, float angle, int eyeLoc);
    LumiEfficencyRe GetLuminanceEfficiency(const cv::Mat img, string color);

    cv::Mat getSolidImgRotated(cv::Mat img, cv::Rect& rect, bool isSLB = false);
    cv::RotatedRect getSolidBorder(cv::Mat img, cv::Rect &rect);
    cv::Rect getLumiEfficiencyROI(cv::Mat img, float angle);

  private:
    cv::Mat preProcess(cv::Mat img);

  private:
    double m_rotationAngle = 0;
    static MLEfficiency *effSelf;
    bool m_isDisparityEyebox = false;
};
} // namespace MLIQMetrics

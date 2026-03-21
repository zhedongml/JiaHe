#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
#include "IQMetricUtl.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
namespace MLIQMetrics
{
struct RolloffRe_NotUsed
{
    double rollOffH;
    double rollOffV;
    cv::Mat imgdraw;
    bool flag = true;
    bool errMsg = "";
};
struct RolloffRe
{
    double p50=0;
    double p5=0;
    double p95=0;
    double ralativeBrightnessLow = 0;
    double ralativeBrightnessHigh = 0;
    //double p50_disp = 0;
    //double p5_disp = 0;
    //double p95_disp = 0;
    //double ralativeBrightnessHigh_disp = 0;
    cv::Mat imgdraw;
    bool flag = true;
    string errMsg = "";
};


class IQMETRICS_API MLLuminanceRolloff : public MLImageDetection::MLimagePublic
{
  public:
    MLLuminanceRolloff();
    ~MLLuminanceRolloff();
  public:
    void setIsSLB(bool flag);
    void setIsDisparityEyebox(bool flag);
    RolloffRe getRelativeBrightness(cv::Mat img);
    RolloffRe_NotUsed LuminanceRolloffRotation(cv::Mat img);
  private:
      void calculateRollOff(cv::Mat& imgdraw, cv::Rect RECT, cv::Mat img, double &p5, double &p50, double &p95,int binNum);

    double m_rotationAngle = 0;
    bool m_isSLB = true;
    bool m_isDisparityEyebox = false;
};
} // namespace MLIQMetrics
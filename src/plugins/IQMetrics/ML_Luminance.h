#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
//#include "ml_image_public.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
namespace MLIQMetrics
{
struct LuminanceRe
{
    double minLum = 0;
    double maxLum = 0;
    double rectMean = 0;
    double rectCov = 0;
    cv::Mat imgdraw;
    bool flag = true;
    string errMsg = "";
};
struct LuminanceGuSuRe
{
    double mean_zoneA=0;
    double mean_zoneB=0;
    cv::Mat imgdraw;
    bool flag = true;
    string errMsg = "";
};

class IQMETRICS_API MLLuminance : public MLImageDetection :: MLimagePublic
{
  public:
    MLLuminance();
    ~MLLuminance();

  public:
      void setColor(string color);
      void setIsSLB(bool flag);
      void setFOVType(FOVTYPE type);
    LuminanceGuSuRe getLuminanceGusu(cv::Mat img);
    LuminanceRe getLuminance(cv::Mat img);
private:
    cv::Mat getZoneAMask(cv::Size size, int radius);
    void saveSLBLuminane(double lumi);
    
  private:
    double m_rotationAngle = 0;
    bool m_isSLB = false;
    FOVTYPE m_fovType;
    string m_color;
};
} // namespace MLIQMetrics
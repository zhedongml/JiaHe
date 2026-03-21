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
    LuminanceGuSuRe getLuminanceGusu(cv::Mat img);
    LuminanceRe getLuminance(cv::Mat img);
private:
    cv::Mat getZoneAMask(cv::Size size, int radius);
  private:
    double m_rotationAngle = 0;
};
} // namespace MLIQMetrics
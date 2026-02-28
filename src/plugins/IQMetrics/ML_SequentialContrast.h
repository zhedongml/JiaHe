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
struct SeqCRRe
{
    cv::Mat Roicr;
    double seqCR = 0;
    cv::Mat seq90Mat;
    cv::Mat ColorROIMat;
    cv::Mat BlackROIMat;
    cv::Mat imgdrawColor;
    cv::Mat imgdrawBlack;
    bool flag = true;
    string errMsg = "";
};
struct SeqCRGuSuRe
{
    double meanZoneA;
    double p5ZoneA;
    double meanZoneB;
    double p5ZoneB;
    cv::Mat imgdraw;
    bool flag = true;
    string errMsg = "";
};
class IQMETRICS_API MLSequentialContrast : public MLImageDetection::MLimagePublic
{
  public:
    MLSequentialContrast();
    ~MLSequentialContrast();

  public:
    SeqCRRe getSequentialContrast(cv::Mat img_WRGB, cv::Mat img_black);
    SeqCRGuSuRe getSequentialContrastGuSu(cv::Mat img_WRGB, cv::Mat img_black);
    double getPercentile(cv::Mat seqMat, cv::Mat mask, double p);
  private:
    
};
} // namespace MLIQMetrics
#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
//#include "ml_image_public.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
//#include "ml_luminance.h"
#include "IQMetricUtl.h"
namespace MLIQMetrics
{
struct FovOffsetRe
{
    double V = 0;
    double H = 0;
    double D = 0;
    double deltxPixel = 0;
    double deltyPixel = 0;
    vector<cv::Point2f> crossCenter;
    cv::Mat imgdraw;
    bool flag = true;
    string errMsg = "";
};
class IQMETRICS_API MLFOVOffset:public MLImageDetection::MLimagePublic
{
  public:
    MLFOVOffset();
    ~MLFOVOffset();
  public:
      void setIsUpdateSLB(bool flag);
      void setIsSLB(bool flag);
      void setFOVType(FOVTYPE type);
      void setColor(string color);
    FovOffsetRe getBoresightGrid(cv::Mat img);
    FovOffsetRe getBoresightGuSu(cv::Mat img);
    FovOffsetRe getBoresightNineCross(const cv::Mat img);

    FovOffsetRe BoresightNoBorder(cv::Mat img);
    FovOffsetRe BoresightNoBorder(cv::Mat img, int roationAngle, MLImageDetection::MirrorALG mirror);
    FovOffsetRe getMultiCrossBoresight(cv::Mat img, int roationAngle, int eyeLoc, MLImageDetection::MirrorALG mirror);

  private:
    void updateImgdraw(cv::Mat &imgdraw, cv::Point2f pt1, int binNum);
    void updateImgdraw(cv::Mat& imgdraw, cv::Point2f pts1, string str, int binNum);
    cv::Point2f getExactLoc(cv::Point2f cen, cv::Mat gray);
    void upateFOVOffset(FovOffsetRe &re,bool IsSLB);

    private:
      bool m_IsSLB = true;
      int m_binNum = 1;
      int m_crossROI = 1200;
      int m_gridROI = 300;
      bool accurateFlag = true;
      FOVTYPE m_FOVType;
      string m_color;
      bool m_IsUpdateSLB = false;
};
} // namespace MLIQMetrics

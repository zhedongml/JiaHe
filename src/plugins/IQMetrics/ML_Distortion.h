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
struct DistortionRe
{
    double disAvg = -1;
    double disMax = 0;
    std::vector<double> disCorner;
    cv::Mat xPos;
    cv::Mat yPos;
    cv::Mat xPosDeg;
    cv::Mat yPosDeg;
    cv::Mat disMat;
    cv::Mat imgdraw;
    bool flag = true;
    string errMsg = "";
};

class IQMETRICS_API MLDistortion : public MLImageDetection::MLimagePublic
{
  public:
    MLDistortion();
    ~MLDistortion();
  public:
      void setIsUpdateSLB(bool flag);
      void setIsSLB(bool flag);
      void setFOVType(FOVTYPE type);
      void setColor(string color);
      DistortionRe GridDistortion(cv::Mat img, cv::Rect rect = cv::Rect(0, 0, -1, -1));
      DistortionRe GridDistortionFourCorner(const cv::Mat img, cv::Rect rect = cv::Rect(0, 0, -1, -1));
      DistortionRe GridDistortionFourCornerBigFOV(const cv::Mat img);

      DistortionRe CheckerDistortion(cv::Mat img);
    void setRotationAngle(double angle);
    void updateImgdraw(cv::Point2f cen, cv::Mat &imgdraw,int binNum);
private:
    cv::Rect getGridRect(cv::Mat roi, cv::RotatedRect& rectR, int binNum = 1);
    cv::Point2f getCornerByHist(cv::Mat img, int binNum);
    cv::Point2f getCorner(cv::Mat img);
    double getCornerDistortionVal(cv::Point2f corner, cv::Point2f center, int binNum = 1);
    double getCornerDistortionVal(cv::Point2f corner, cv::Point2f center, double pix2deg);
    cv::Point2f getAccucateCenter(cv::Point2f c0, cv::Mat img);
    cv::Point2f getCenter(vector<cv::Point2f> corns, cv::Mat indexMap);
    void updateDistortionBySLB(vector<double>&disvec,bool isSLB);

  private:
    double m_rotationAngle = 0;
    int m_len = 600;
    bool accurateFlag = true;
    int m_binNum = 1;
    bool m_isSLB = true;
    FOVTYPE m_fovType;
    string m_color;
    bool m_IsUpdateSLB = false;
};


} // namespace MLIQMetrics
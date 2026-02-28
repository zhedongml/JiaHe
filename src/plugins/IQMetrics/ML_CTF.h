#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
#include "ml_image_public.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
namespace MLIQMetrics
{
    enum CTFType
    {
        CTF_VERTICAL,
        CTF_HORIZONTAL
    };
struct CTFRe
{
    cv::Mat ctfMap;
    bool flag = true;
    string errMsg = "";
};
struct AmazonCTFRe
{
    map <string, double>ctfMap;
    cv::Mat imgdraw;
    bool flag = true;
    string errMsg = "";
};
class IQMETRICS_API MLCTF : public MLImageDetection::MLimagePublic{
  public:
    MLCTF();
    ~MLCTF();

  public:
    CTFRe getCTF(cv::Mat img, CTFType type);
    AmazonCTFRe getCTFAmazon(cv::Mat img, CTFType type);
    double calculateCTF(cv::Mat roi, CTFType type);

  private:
    cv::Rect getCTFRect(cv::Mat img, cv::RotatedRect &rectR,cv::Mat &imgdraw);
    map<string, cv::Rect>getCTFRectMap(cv::Mat img, cv::Mat& imgdra, map<string, cv::Point2f>& cenMap, int binNum);
    map<string, cv::Rect>getCTFRectMapByDistance(cv::Mat img, cv::Mat& imgdra, int binNum);
    map<string, cv::Rect>sortRects(vector<cv::Rect>rectVec, vector<cv::Point2f>cenVec, cv::Mat& imgdraw, map<string, cv::Point2f>& cenMap);
    vector<double> m_disx = { 0, };
    vector<double>m_disy = {};
    vector<double>m_dis = { 0,971,1614,2255,2903 };
};
} // namespace MLIQMetrics

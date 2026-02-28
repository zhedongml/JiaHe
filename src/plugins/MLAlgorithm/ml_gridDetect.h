#pragma once
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <vector>
#include "ml_image_public.h"

#ifndef MLALGORITHM_MLGRIDDETECT_H_
#define MLALGORITHM_MLGRIDDETECT_H_
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
using namespace cv;
using namespace std;
namespace algorithm
{
class ALGORITHM_API MLGridDetect : public MLimagePublic
{
  public:
    MLGridDetect();
    ~MLGridDetect();
  public:
    std::vector<cv::Point2f> GridCorners(cv::Mat img, cv::Mat & indexMap);
    vector<cv::Point2f> GridHoughLine(cv::Mat roi, cv::Mat &ordNew, cv::Size boadsize = cv::Size(11, 11));
    vector<cv::Point2f> GridContour(cv::Mat img, cv::Mat& indexMap);
    vector<cv::Point2f> GridTemplateGaussian(cv::Mat img, cv::Mat& indexMap);    
    cv::Mat getImgDraw();

    void SetbinNum(int bin);
  private:
    cv::Mat m_img_draw;
    int m_binNum=1;
};

} // namespace algorithm
#endif // !MLALGORITHM_MLGRIDDETECT_H_
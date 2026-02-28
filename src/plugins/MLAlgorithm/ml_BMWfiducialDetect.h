#pragma once
#ifndef MLALGORITHM_MLBMWFIDUCIALDETECT_H_
#define MLALGORITHM_MLBMWFIDUCIALDETECT_H_
#include "CrossCenter.h"
#include "ml_cross_center.h"
#include "ml_image_public.h"
#include <iostream>
#include <vector>
using namespace std;
namespace algorithm
{
class ALGORITHM_API MLBMWFiducialDetect : public MLimagePublic
{
  public:
    MLBMWFiducialDetect();
    ~MLBMWFiducialDetect();

  private:
    int flag = 1;
    double sim = 0.7;
    cv::Mat m_img_draw;

  public:

    vector<cv::Point2f> BMWFiducialDetect(cv::Mat img);

    std::vector<cv::Point2f> BMWHoughLineDetect(cv::Mat img);
    std::vector<cv::Point2f> BMWContoursDetect(cv::Mat img);
    std::vector<cv::Point2f> BMWCornersDetect(cv::Mat img);

    cv::Mat getImgDraw();

  private:
    cv::Rect GetValidROI(cv::Mat img);
    cv::Mat GetProcessedImage(cv::Mat img);
};

} // namespace algorithm
#endif // !MLALGORITHM_FIDUCIALDETECT_H_

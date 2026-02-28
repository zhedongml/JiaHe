#pragma once
#ifndef MLALGORITHM_MLRECTANGLEFIDUCIALDETECT_H_
#define MLALGORITHM_MLRECTANGLEFIDUCIALDETECT_H_
#include "CrossCenter.h"
#include "ml_cross_center.h"
#include "ml_image_public.h"
#include <iostream>
#include <vector>
using namespace std;
namespace algorithm
{
class ALGORITHM_API MLRectangleFiducialDetect : public MLimagePublic
{
  public:
    MLRectangleFiducialDetect();
    ~MLRectangleFiducialDetect();

    private:
    cv::Mat m_img_draw;
  public:
    vector<cv::Point2f> RectangleFiducialDetect(cv::Mat img);

    cv::Mat getImgDraw();
};

} // namespace algorithm
#endif // !MLALGORITHM_FIDUCIALDETECT_H_

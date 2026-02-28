#pragma once
#ifndef MLALGORITHM_MLLINESFIDUCIALDETECT_H_
#define MLALGORITHM_MLLINESFIDUCIALDETECT_H_
#include "CrossCenter.h"
//#include "ml_image_public.h"
#include <iostream>
#include <vector>
using namespace std;
namespace algorithm
{
class ALGORITHM_API MLLinesFiducialDetect : public MLimagePublic
{
  public:
    MLLinesFiducialDetect();
    ~MLLinesFiducialDetect();

  private:
    cv::Mat m_img_draw;
    int flag = 2;

  public:
    std::vector<cv::Point2f> LineFiducialDetect(cv::Mat img);

    cv::Mat getImgDraw();

};

} // namespace algorithm
#endif // !MLALGORITHM_FIDUCIALDETECT_H_

#pragma once
#include "ml_image_public.h"
#include <algorithm>
#include <functional>
#include <numeric>
#include <string>
using namespace std;
using namespace cv;
namespace algorithm
{
class ALGORITHM_API RectangleDetection:public MLimagePublic
{
  public:
    RectangleDetection();
    ~RectangleDetection();

  public:
    cv::RotatedRect getRectangleBorder(cv::Mat img);
    cv::Mat getImgdraw();

  private:
    cv::Mat m_img_draw;
};
} // namespace algorithm

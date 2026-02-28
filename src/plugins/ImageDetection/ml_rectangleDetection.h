#pragma once
#include "ml_image_public.h"
//#include <MLImageDetection>
#include <algorithm>
#include <functional>
#include <numeric>
#include <string>
using namespace std;
using namespace cv;
namespace MLImageDetection
{
class ALGORITHM_API RectangleDetection:public MLimagePublic
{
    struct SolidRect
    {
        cv::Rect rect;
        cv::Point2f h1, h2;
        cv::Point2f v1, v2;
    };
  public:
    RectangleDetection();
    ~RectangleDetection();

  public:
      void setBinNum(int num);
    cv::RotatedRect getRectangleBorder(cv::Mat img);
    cv::Rect getSolidExactRect(cv::Mat img8, cv::Rect rect);
    cv::Mat getImgdraw();
  private:
      int m_len = 300;
    cv::Mat m_img_draw;
    int m_binNum = 1;
};
} // namespace MLImageDetection

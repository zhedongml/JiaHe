#pragma once
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "ml_image_public.h"
using namespace std;

static cv::Rect m_rect;
namespace algorithm
{
enum IQmethod
{
    BRENNER=1,
    TENENGARD,
    LAPLACIAN,
    STANDARD
};
class ALGORITHM_API MLImageQuality:public MLimagePublic
{
  public:
    MLImageQuality();
    ~MLImageQuality();

  public:
    double getImgQualtiy(cv::Mat &image, IQmethod method);
    double brenner(cv::Mat &image);
    double tenengard(cv::Mat &image);
    double laplacian(cv::Mat &image);
    double standarddeviation(cv::Mat &image);
    cv::Rect getCircleROI(cv::Mat img);
};
} // namespace algorithm

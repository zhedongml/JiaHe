#pragma once
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
#include <opencv2/imgproc/types_c.h>
#include <opencv2\opencv.hpp>
namespace algorithm
{
class ALGORITHM_API AutoFocus
{
  public:
    AutoFocus();
    ~AutoFocus();

  public:
    double brenner(cv::Mat &image);
    double tenengard(cv::Mat &image);
    double smd(cv::Mat &image);
    double smd2(cv::Mat &image);
    double blur_noise_IQA(cv::Mat &image);
};
}

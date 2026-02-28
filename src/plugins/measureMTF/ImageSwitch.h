#pragma once

#include <color-util/XYZ_to_RGB.hpp>

#include <QImage>
#include "Result.h"
#include <opencv2\opencv.hpp>

#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif

using namespace cv;
using namespace std;

class ALGORITHM_API ImageSwitch
{
public:
    ImageSwitch();
    ~ImageSwitch();

    Result xyz2rgb(QImage &rgbImg, const cv::Mat &xImage, const cv::Mat &yImage, const cv::Mat &zImage);
    Result xyz2rgbMat(cv::Mat &rgbMat, const cv::Mat &xImage, const cv::Mat &yImage, const cv::Mat &zImage);

  private:
    bool cvMat2QImage(const cv::Mat &mat, QImage &image);
    cv::Mat getRMatrix();

    Result xyz2rgbUtil(cv::Mat &rgbMat, const cv::Mat &xImage, const cv::Mat &yImage, const cv::Mat &zImage);
    Result xyz2rgbDiy(cv::Mat &rgbMat, const cv::Mat &xImage, const cv::Mat &yImage, const cv::Mat &zImage);
    Result xyz2rgbCvt(cv::Mat &rgbMat, const cv::Mat &xImage, const cv::Mat &yImage, const cv::Mat &zImage);
};

#pragma once
#ifndef MLALGORITHM_MLCIRCLEFIDUCIALDETECT_H_
#define MLALGORITHM_MLCIRCLEFIDUCIALDETECT_H_
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
#include "ml_image_public.h"
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;

namespace algorithm
{
class ALGORITHM_API MLCircleFiducialDetect : public MLimagePublic
{
  public:
    MLCircleFiducialDetect();
    ~MLCircleFiducialDetect();

  private:
    bool isCircleFit = false;
    bool isHoughCircles = true;
    bool isCircleMatch = false;
    int point_number = 3;
    double sim = 0.9;
    cv::Mat m_img_draw;

  public:
    std::vector<cv::Point2f> CircleFiducialDetect(cv::Mat img);
    std::vector<cv::Vec3f> CircleFiducialRecognition(cv::Mat img);

    std::vector<cv::Vec3f> CircleFiducialDetectFit(Mat img);
    std::vector<cv::Vec3f> CircleFiducialDetectHough(Mat img);
    std::vector<cv::Vec3f> CircleFiducialDetectMatch(cv::Mat img);

    cv::Mat getImgDraw();

    bool circleFiducailTest(cv::Mat src, Rect rect, cv::Point2f &center);

  private:
    void circleSort(vector<cv::Vec3f> circles, vector<cv::Vec3f> &newcircles);

    bool circleFiducailTest(cv::Mat src, Rect rect, int circleRadius, cv::Point2f &center);
    bool circleRadius(string fileName, float &radius, Rect &rect);
};

} // namespace algorithm
#endif // !MLALGORITHM_FIDUCIALDETECT_H_
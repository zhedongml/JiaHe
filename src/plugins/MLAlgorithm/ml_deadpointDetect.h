#pragma once
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <vector>
#include "ml_image_public.h"

#ifndef MLALGORITHM_MLDEADPOINTDETECT_H_
#define MLALGORITHM_MLDEADPOINTDETECT_H_
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
using namespace cv;
using namespace std;

namespace algorithm
{
struct DeadPointRe
{
    int partAreaNum = 0;
    vector<cv::Point> locPart;
    int allAreaNum = 0;
    vector<cv::Point> locALL;
    cv::Mat MarkerImg;
};
class ALGORITHM_API MLDeadPointDetect : public MLimagePublic
{
 
  public:
    MLDeadPointDetect();
    ~MLDeadPointDetect();

  public:
    DeadPointRe DeadPoints(cv::Mat img,double a=0);
    DeadPointRe DeadPointsDetectHist(cv::Mat img);
    DeadPointRe DeadPointsDetectThreshold(cv::Mat img);
    DeadPointRe DeadPointsDetectPinto(cv::Mat img);
    DeadPointRe DeadPointsDetectMedian(cv::Mat img);
    void DeanPointsAvg(cv::Mat img);
    cv::Mat ExpandMat(cv::Mat img, int r);

};
} // namespace algorithm

#endif

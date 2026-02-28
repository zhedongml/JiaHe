#pragma once
#ifndef MLALGORITHM_FIDUCIALDETECT_H_
#define MLALGORITHM_FIDUCIALDETECT_H_
#include<vector>
#include "ml_circlefiducialDetect.h"
#include "ml_linefiducialDetect.h"
#include "ml_BMWfiducialDetect.h"
#include "ml_rectanglefiducialDetect.h"
#include"ml_matchTemplate.h"
#include<iostream>
#include"ml_image_public.h"
using namespace std;
namespace algorithm
{
enum FiducialType
{
    CIRCLE = 1,
    CROSSHAIR,
    BMW,
    RECTANGLE,
    PICO
};

class ALGORITHM_API FiducialDetect: public MLimagePublic
{
  public:
    FiducialDetect();
    ~FiducialDetect();
  private:
    cv::Mat m_img_draw;

  public:
    std::vector<cv::Point2f> FiducialCoordinate(cv::Mat img,int flag);
    // the match template interfase
    ALGResult FiducialCoordinate(cv::Mat img, FiducialType ftype, std::vector<cv::Point2f> &);
    ALGResult FiducialContour(cv::Mat img,vector<cv::Point2f>&loc);
    ALGResult FiducialMatchTemplate(cv::Mat img, vector<cv::Point2f> &loc);

    cv::Mat getImgDraw();
};

} // namespace algorithm
#endif // !MLALGORITHM_FIDUCIALDETECT_H_

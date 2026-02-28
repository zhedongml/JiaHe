#pragma once
#ifndef MLALGORITHM_FIDUCIALDETECT_H_
#define MLALGORITHM_FIDUCIALDETECT_H_
#include<vector>
#include<iostream>
#include"ml_image_public.h"
using namespace std;
namespace MLImageDetection
{
enum FiducialType
{
    CIRCLE = 1,
    CROSSHAIR,
    BMW,
    RECTANGLE,
    PICO
};
struct FiducialRe
{
    cv::Point2f loc;
    cv::Mat imgdraw;
    bool flag = true;
    string errMsg = "";
};

class ALGORITHM_API FiducialDetect: public MLimagePublic
{
  public:
    FiducialDetect();
    ~FiducialDetect();
  private:
    cv::Mat m_img_draw;
  public:
     FiducialRe getFiducialCoordinate(cv::Mat img, cv::Rect rect=cv::Rect(0,0,-1,-1));
    std::vector<cv::Point2f> FiducialCoordinate(cv::Mat img,int flag);
    // the match template interfase
    ALGResult FiducialCoordinate(cv::Mat img, FiducialType ftype, std::vector<cv::Point2f> &);
    ALGResult FiducialContour(cv::Mat img,vector<cv::Point2f>&loc);
    ALGResult FiducialMatchTemplate(cv::Mat img, vector<cv::Point2f> &loc);
    cv::Mat getImgDraw();

private:
    cv::Point2f getFiducialCoordinateByContour(cv::Mat gray,cv::Mat &imgdraw);
    cv::Point2f getFiducialCoordinateByHough(cv::Mat gray, cv::Mat& imgdraw);
    cv::Point2f getFiducialCoordinateByTemplate(cv::Mat gray, cv::Mat& imgdraw);
    cv::Point2f getExactLocation(cv::Mat gray, cv::Point2f c0);
    vector<cv::Point2f>findCircleExactPoints(cv::Mat mag, int step, cv::Vec3f circle, cv::Mat& imgdraw);

};

} // namespace algorithm
#endif // !MLALGORITHM_FIDUCIALDETECT_H_

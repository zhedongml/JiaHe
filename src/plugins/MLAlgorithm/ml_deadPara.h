#pragma once
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<iostream>
#include "ml_image_public.h"
#include "ml_hydrus_2config.h"
#include "ml_deadpointDetect.h"
using namespace std;
using namespace cv;
namespace algorithm{

class MLDeadPara:public MLimagePublic
{
  public:
    MLDeadPara();
    ~MLDeadPara();
public:
    void SetDeadImg(Mat img);
    DeadPointRe GetDeadPoints();
    DeadPointRe GetDeadPointsInf();
    cv::Mat getImgDraw();

private:
    DeadPointRe DeadPointsInf(cv::Mat img, cv::Rect rect = cv::Rect(0, 0, -1, -1));

private:
    cv::Mat m_imgDead;
    cv::Mat m_img_draw;
    Newpara m_para;
    MLHydrusConfig hydrusConfig;

};
}
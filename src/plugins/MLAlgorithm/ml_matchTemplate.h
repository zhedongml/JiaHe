#pragma once

#ifndef MLALGORITHM_MLMATCHTEMPLATE_H_
#define MLALGORITHM_MLMATCHTEMPLATE_H_

#ifdef MLALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif

#include "ml_image_public.h"
namespace algorithm
{

class ALGORITHM_API MatchTemplate:public MLimagePublic
{
  public:
    MatchTemplate();
    ~MatchTemplate();

  public:
    ALGResult getMatchTemplateResult(cv::Mat src, cv::Mat templ, double score, vector<cv::Point2f> &loc1);
    cv::Mat getImgDraw();

  private:
    void matchImageProcess(cv::Mat &img);
    cv::Mat m_img_draw;
    vector<cv::Point2f> angle_test(string mode = "test", bool use_rot = true);
    cv::Mat m_src;
    cv::Mat m_template;
    double m_score = 60;
    double m_angleStep = 180;
    string prefix = "./config/templateImg/";
};
} // namespace algorithm

#endif


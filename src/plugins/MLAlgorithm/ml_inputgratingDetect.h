#pragma once
#ifndef MLALGORITHM_MLINPUTGRATINGDETECT_H_
#define MLALGORITHM_MLINPUTGRATINGDETECT_H_

#ifdef MLALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
#include "ml_distortion_correction.h"
#include "libcbdetect/boards_from_corners.h"
#include "libcbdetect/config.h"
#include "libcbdetect/find_corners.h"
#include "libcbdetect/plot_boards.h"
#include "libcbdetect/plot_corners.h"
#include <dmtx.h>
#include "ml_image_public.h"
namespace algorithm
{
struct InputRe
{
    cv::Point2f center;
    cv::Mat imgdraw;
    bool flag = false;
    string errMsg = "";
};
    class ALGORITHM_API MLInputgratingDetect:public MLimagePublic
    {
    public:
        MLInputgratingDetect();
        ~MLInputgratingDetect();
    public:

        ALGResult getInputAndProjectPixelDistance(cv::Mat projectImg, cv::Mat inputImg, cv::Point2f &dis);
        cv::Point2f getInputImgCoordinate(cv::Mat inputImg);
        InputRe getInputImgCoordinateRectangle(cv::Mat input);
        cv::Point2f getProjectImgCoordinate(cv::Mat projectImg);
        
        cv::Mat getImgDraw();
    public:
        vector<cv::Point> circleDetectHough(cv::Mat img);
        vector<cv::Point> circleDetectContour(cv::Mat img);
        vector<cv::Point> circleMatchTemplate(cv::Mat img);
        cv::Mat m_img_draw;
    };
}

#endif

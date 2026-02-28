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
using namespace std;
using namespace cv;
namespace algorithm {
    class MLGhostPara :public MLimagePublic
    {
    public:
        MLGhostPara();
        ~MLGhostPara();
    public:
        void SetGhostImage(cv::Mat img_ghost);
        std::vector<double> GetStrengthRatio();
        cv::Mat getImgDraw();

    private:
        std::vector<double> StrengthRatio(cv::Mat img);
    private:
        cv::Mat m_img_ghost;
        cv::Mat m_img_draw;
        MLHydrusConfig hydrusConfig;
        Newpara m_para;
    };
}

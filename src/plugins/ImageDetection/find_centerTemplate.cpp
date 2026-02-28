#pragma once
//#include "pch.h"
#include "CrossCenter.h"
//#include <MLAlgorithm/ml_optpara_kunlun.h>
//#include"qlog.h"
using namespace std;
using namespace cv;
using namespace MLImageDetection;

cv::Point2f CrossCenter::find_centerTemplate(cv::Mat img, cv::Mat tmpl)
{
    cv::Point2f center;
    cv::Mat img_draw = img.clone();
    // void GetAllMinLoc(Mat image, Mat templ, double sim, Scalar mask, vector<cv::Point> *all_min_loc, int num = 1); 
    vector<cv::Point> P;
    GetAllMinLoc(img, tmpl, 0.36, Scalar(), &P, 1);
    if (P.size() > 0)
    {
        double w = min(tmpl.cols, img.cols - P[0].x);
        double h = min(tmpl.rows, img.rows - P[0].y);
        cv::Rect rect = cv::Rect(P[0].x, P[0].y, w, h);
        cv::Mat roi = img(rect);
        center =cv::Point(0,0) ;
        //center = find_centerGaussian(roi);
        if(center.x==0||center.y==0)
        {
            center.x = tmpl.rows / 2;
            center.y = tmpl.cols / 2;

        }
        center.x = center.x+rect.x;
        center.y = center.y + rect.y;
        circle(img_draw, center, 1, cv::Scalar(255, 0, 255), -1);
        m_center = center;
    }
    else
    {
        center.x = img.rows / 2;
        center.y = img.cols / 2;
        circle(img_draw, center, 1, cv::Scalar(255, 0, 255), -1);
        m_center = center;
    
    }

    return m_center;
}

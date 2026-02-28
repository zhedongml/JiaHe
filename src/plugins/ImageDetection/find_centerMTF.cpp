#pragma once
//#include "pch.h"
#include "CrossCenter.h"
//#include <MLAlgorithm/ml_optpara_kunlun.h>
//#include"qlog.h"
using namespace std;
using namespace cv;
using namespace MLImageDetection;

cv::Point2f CrossCenter::find_centerMTF(cv::Mat img)
{
    img = convert12bitTo8bit(img);
    cv::Mat img_draw = img.clone();//this->img.clone();
    cv::Point2f center;// 
    // gradient method
    cv::Point2f c0;
    c0 = cv::Point2f(img.rows / 2, img.cols / 2);
    //= find_centerGrad(img);
    if (abs(c0.x) < 1e-6 || abs(c0.y) < 1e-6)
    {
        //cv::Mat tmpl = cv::imread("E:\\project\\mtfiq1\\src\\app\\config\\ALGConfig\\template3.tif");
        cv::Mat tmpl;
        if (DEBUG==1)
           tmpl = cv::imread("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\ALGConfig\\template42.tif");
            else
             tmpl = cv::imread("./config/ALGConfig/template42.tif");


         //cv::Mat tmpl = cv::imread("D:\\GitProject\\mtfiq2\\src\\app\\config\\ALGConfig\\template3.tif");
        c0 = find_centerTemplate(img, tmpl);
         //cv::circle(img_draw, c0, 2, Scalar(255, 0, 255), -1);
    }
    // TODO: add the template matching method
    // template method

    if (c0.x != 0 & c0.y != 0)
    {
        int w = CrossCenterParameter::roilength/m_binNum;
        int top = cv::max((int)(c0.y - w / 2), 0);
        int bottom = cv::min((int)c0.y + w / 2, img.rows);
        int left = cv::max(0, (int)c0.x - w / 2);
        int right = cv::min(img.cols, (int)c0.x + w / 2);
        Rect rect = Rect(left, top, right - left, bottom - top);
        cv::Mat roi = img(rect);
        cv::Point2f c1 = find_centerGaussian(roi);//
                                                   // add
        if (c1.x==0||c1.y==0)
        {
            c1 = find_centerLINES(roi);
        }
        if (c1.x != 0 && c1.y != 0)
        {
            center = c1 + cv::Point2f(rect.x, rect.y);
        }
        cv::circle(img_draw, center, 2, Scalar(255, 0, 255), -1);
        m_img_draw = img_draw.clone();
        if (roi_center.size() == 4)
        {
            for (int i = 0; i < roi_center.size(); i++)
            {
                new_roi_center.push_back(roi_center[i] + cv::Point(rect.x, rect.y));

                cv::circle(img_draw, new_roi_center[i], 2, Scalar(255, 0, 255), -1);
            }
        }
        //TODO:
        roi_center.clear();
        roi_center = new_roi_center;
    }
    return center;
}

cv::Point2f CrossCenter::find_centerMTF_manual(cv::Mat img)
{    
        img = convert12bitTo8bit(img);
        cv::Mat img_draw = img.clone(); // this->img.clone();
        cv::Point2f center;             //
        // gradient method
        cv::Point2f c0;
        // c0 = cv::Point2f(img.rows / 2, img.cols / 2);
        c0 = find_centerGrad(img);
        // template method
        if (abs(c0.x) < 1e-6 || abs(c0.y) < 1e-6)
        {
            cv::Mat tmpl;
            if (DEBUG == 1)
                tmpl = cv::imread("E:\\project\\hydrus3\\mtfiq\\src\\app\\config\\ALGConfig\\template42.tif");
            else
                tmpl = cv::imread("./config/ALGConfig/template42.tif");
            c0 = find_centerTemplate(img, tmpl);
            // cv::circle(img_draw, c0, 2, Scalar(255, 0, 255), -1);
        }

        if (c0.x != 0 & c0.y != 0)
        {
            int w = CrossCenterParameter::roilength / m_binNum;
            int top = cv::max((int)(c0.y - w / 2), 0);
            int bottom = cv::min((int)c0.y + w / 2, img.rows);
            int left = cv::max(0, (int)c0.x - w / 2);
            int right = cv::min(img.cols, (int)c0.x + w / 2);
            Rect rect = Rect(left, top, right - left, bottom - top);
            cv::Mat roi = img(rect);
            cv::Point2f c1 = find_centerGaussian(roi); //
            if (c1.x != 0 && c1.y != 0)
            {
                center = c1 + cv::Point2f(rect.x, rect.y);
            }
            cv::circle(img_draw, center, 2, Scalar(255, 0, 255), -1);
            m_img_draw = img_draw.clone();
            if (roi_center.size() == 4)
            {
                for (int i = 0; i < roi_center.size(); i++)
                {
                    new_roi_center.push_back(roi_center[i] + cv::Point(rect.x, rect.y));

                    cv::circle(img_draw, new_roi_center[i], 2, Scalar(255, 0, 255), -1);
                }
            }
            // TODO:
            roi_center.clear();
            roi_center = new_roi_center;
        }
        return center;
    
}

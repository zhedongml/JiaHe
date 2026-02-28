#pragma once
//#include "pch.h"
#include "CrossCenter.h"
//#include <MLAlgorithm/ml_optpara_kunlun.h>
//#include"qlog.h"
using namespace std;
using namespace cv;

cv::Point2f CrossCenter::find_centerGrad(cv::Mat img)
{
    cv::Point2f c0{ 0, 0 };
    cv::Mat img_draw;
    cv::Mat img_gray1, img_gray;
    if (img.channels() == 3)
        cvtColor(img, img_gray1, CV_BGR2GRAY);
    else
        img_gray1 = img.clone();

    if (img_gray1.depth() == CV_8U)
    {
        img_gray = img_gray1.clone();
        img_draw = img.clone();
    }
    else {
        img_gray = convertToUint8(img_gray1);
        img_draw = convertToUint8(img);
    }
    cv::medianBlur(img_gray, img_gray, 3);
    cv::Scalar meanAll;
    meanAll = cv::mean(img_gray);
    double min = 0, max = 0;
    cv::Point minloc, maxloc;
    minMaxLoc(img_gray, &min, &max, &minloc,&maxloc);
    int left = cv::max(maxloc.x - 10, 0);
    int right = cv::min(maxloc.x + 10, img_gray.cols);
    int bottom = cv::max(maxloc.y - 10, 0);
    int top = cv::min(maxloc.y + 10, img_gray.rows);
    cv::Rect rec = cv::Rect(left, bottom, right - left, top - bottom);
    cv::Mat roi = img_gray(rec); // img_gray(cv::Range(left,right), cv::Range(bottom,top));
    cv::Scalar meanRoi = cv::mean(roi);
    if (meanRoi(0) > meanAll(0))
    {
        cv::Mat roi = img_gray.clone();
        //GaussianBlur(roi, roi, Size(5, 5), 5, 5);
        double minVal;
        double maxVal;
        Point minLoc;
        Point maxLoc;
        // grad
        cv::Mat xG, yG;
        //Mat element1 = getStructuringElement(MORPH_RECT, Size(size, size));
        //morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);
        cv::Sobel(roi, xG, CV_32FC1, 1, 0, 3);
        cv::Sobel(roi, yG, CV_32FC1, 0, 1, 3);
        convertScaleAbs(xG, xG); //
        convertScaleAbs(yG, yG);
        cv::Mat sum = xG + yG;
        cv::Mat sobelxy;
        cv::addWeighted(xG, 0.5, yG, 0.5, 0, sobelxy);
        cv::Mat xGth, yGth;
        cv::threshold(xG, xGth, 0, 255, CV_THRESH_OTSU);
        cv::threshold(yG, yGth, 0, 255, CV_THRESH_OTSU);
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
      //  morphologyEx(xGth, xGth, MORPH_OPEN, element);
        //morphologyEx(yGth, yGth, MORPH_OPEN, element);
        Clear_MicroConnected_Areas(xGth, xGth, 30);
        Clear_MicroConnected_Areas(yGth, yGth, 30);
        cv::Mat xGcan, yGcan;
        cv::Canny(xGth, xGcan, 30, 90);
        cv::Canny(yGth, yGcan, 30, 90);
        //NaiveRemoveNoise();
        vector<cv::Point2f> centers;
        for (int i = 1; i < xGcan.rows; i++)
        {
            for (int j = 0; j < xGcan.cols; j++)
            {
                int x = xGcan.at<uchar>(i, j);
                int y = yGcan.at<uchar>(i, j);
                if (x == 255 & y == 255)
                {
                    centers.push_back(cv::Point2f(j, i));
                    //cv::circle(roi, cv::Point2f(j, i), 3, cv::Scalar(255, 0, 255), -1);
                    // cout << cv::Point2f(j, i) << endl;
                }
            }
        }
        vector<double> dis1;
        double k[50] = { 0 };
        cv::Mat kMat = cv::Mat::zeros(cv::Size(1, 50), CV_8UC1);
        cv::Mat dis = cv::Mat::zeros(cv::Size(50, 50), CV_32FC1);
        cv::Mat index = cv::Mat::zeros(cv::Size(50, 50), CV_8UC1);
        if (centers.size() > 0 & centers.size() < 50)
        {
            double tmp = Getdistance(centers[0], cv::Point2f(0, 0));
            dis.at<float>(0, k[0]) = tmp;
            index.at<uchar>(0, 0) = 0;
            k[0]++;
            kMat.at<uchar>(0, 0) = k[0];
            for (int i = 1; i < centers.size(); i++)
            {
                double tmp = Getdistance(centers[i], cv::Point2f(0, 0));
                int flag = 0;
                int j = 0;
                for (j = 0; j < dis.rows; j++)
                {
                    if (dis.at<float>(j, 0) > 0)
                    {
                        double d = abs(tmp - dis.at<float>(j, 0));
                        int in = index.at<uchar>(j, 0);
                        double dx = abs(centers[i].x - centers[index.at<uchar>(j, 0)].x);
                        double dy = abs(centers[i].y - centers[index.at<uchar>(j, 0)].y);
                        int distance = CrossCenterParameter::pointdistance;
                        if (dx < distance && dy < distance)
                        {
                            dis.at<float>(j, k[j]) = tmp;
                            index.at<uchar>(j, k[j]) = i;
                            k[j]++;
                            kMat.at<uchar>(j, 0) = k[j];
                            flag = 1;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                if (flag == 0)
                {
                    dis.at<float>(j, k[j]) = tmp;
                    index.at<uchar>(j, k[j]) = i;
                    k[j]++;
                    kMat.at<uchar>(j, 0) = k[j];
                }
            }
            cv::minMaxLoc(kMat, &minVal, &maxVal, &minLoc, &maxLoc);
            int num = maxVal;
            int in = maxLoc.y;
            // cout << "test" << endl;
            double sumx = 0, sumy = 0;
            for (int i = 0; i < num; i++)
            {
                int in1 = index.at<uchar>(in, i);
                sumx = sumx + centers[in1].x;
                sumy = sumy + centers[in1].y;
                //  cout << centers[in1] << endl;
            }
            c0 = cv::Point2f(sumx / num, sumy / num);
           // c0.x = c0.x + maxloc.x;
          //  c0.y = c0.y + maxloc.y;
            circle(img_draw, c0, 5, cv::Scalar(255, 0, 255), -1);
        }

    }
    return c0;
}

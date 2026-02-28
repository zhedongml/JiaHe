#include "ml_rectanglefiducialDetect.h"
#include <opencv2/imgproc/types_c.h>
#include <QDebug>

using namespace algorithm;
using namespace cv;

algorithm::MLRectangleFiducialDetect::MLRectangleFiducialDetect()
{
}
algorithm::MLRectangleFiducialDetect::~MLRectangleFiducialDetect()
{
}
vector<cv::Point2f> algorithm::MLRectangleFiducialDetect::RectangleFiducialDetect(cv::Mat img)
{
    vector<cv::Point2f> c;
    if (img.data != NULL)
    {
        int len = 1200;
        int left = max(img.cols / 2 - len / 2, 0);
        int width = min(len, img.cols - left);
        int top = max(img.rows / 2 - len / 2, 0);
        int height = min(len, img.rows - top);
        cv::Mat roi = img(cv::Rect(left, top, width, height));
        cv::Mat img_draw = roi.clone();

        if (img.channels() == 3)
        {
            cvtColor(roi, roi, cv::COLOR_BGR2GRAY);
        }
        cv::GaussianBlur(roi, roi, cv::Size(5, 5), 5, 5);
        cv::Scalar mean = cv::mean(roi);
        double m = mean(0);
        for (int i = 0; i < roi.rows; i++)
        {
            for (int j = 0; j < roi.cols; j++)
            {
                if (roi.at<uchar>(i, j) < m)
                {
                    roi.at<uchar>(i, j) = m;
                }
            }
        }
        cv::Mat imggradient;
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20, 20));
        cv::morphologyEx(roi, roi, cv::MORPH_CLOSE, element);
        cv::morphologyEx(roi, imggradient, cv::MORPH_GRADIENT, element);
        imggradient = imggradient(Range(30, imggradient.rows - 30), Range(30, imggradient.cols - 30));
        cv::Mat imgbinary;
        cv::threshold(imggradient, imgbinary, 0, 255, cv::THRESH_OTSU);
        // cv::morphologyEx(imgbinary, imgbinary, cv::MORPH_CLOSE, element);
        cv::Mat imgcanny;
        cv::Canny(imgbinary, imgcanny, 20, 60);
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(imgbinary, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
        for (int i = 0; i < contours.size(); i++)
        {
            if (contourArea(contours[i], false) > 1e4 && contourArea(contours[i], false) < 2e5)
            {
                cout << contourArea(contours[i], false) << endl;
                drawContours(img_draw, contours, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 1, 8, hierarchy);
                RotatedRect rect = minAreaRect(contours[i]);
                Point2f P[4];
                rect.points(P);
                double w = Getdistance(P[0], P[1]); // abs(P[0].x - P[2].x);
                double h = Getdistance(P[0], P[3]); // abs(P[0].y - P[2].y);
                double asp = max(w, h) / min(w, h);
                if (w > 350 & w<500 & asp> 0.9 & asp < 1.4)
                {
                    for (int j = 0; j <= 3; j++)
                    {
                        line(img_draw, P[j], P[(j + 1) % 4], Scalar(0, 0, 255), 2);
                    }
                    cv::Point2f center = (P[0] + P[1] + P[2] + P[3]) / 4;
                    circle(img_draw, center, 5, cv::Scalar(255, 0, 0), -1);
                    c.push_back(center + cv::Point2f(left + 30, top + 30));
                }
            }
        }
        if (c.size() == 2)
        {
            if (abs(c[0].x - c[1].x) < 30 && abs(c[0].y - c[1].y) < 30)
            {
                cv::Point2f temp = (c[0] + c[1]) / 2;
                c.clear();
                c.push_back(temp);
            }
        }
        else if (c.size() < 1)
        {
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));
            cv::morphologyEx(imgbinary, imgbinary, cv::MORPH_OPEN, element);
            vector<vector<Point>> contours;
            vector<Vec4i> hierarchy;
            findContours(imgbinary, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
            // vector<cv::Point2f>c;
            for (int i = 0; i < contours.size(); i++)
            {
                if (contourArea(contours[i], false) > 1e3 && contourArea(contours[i], false) < 2e5)
                {
                    // cout << contourArea(contours[i], false) << endl;
                    drawContours(img_draw, contours, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 1, 8,
                                 hierarchy);
                    RotatedRect rect = minAreaRect(contours[i]);
                    Point2f P[4];
                    rect.points(P);
                    double w = Getdistance(P[0], P[1]); // abs(P[0].x - P[2].x);
                    double h = Getdistance(P[0], P[3]); // abs(P[0].y - P[2].y);
                    double asp = max(w, h) / min(w, h);

                    if (w > 350 & w<500 & asp> 0.9 & asp < 1.4)
                    {
                        for (int j = 0; j <= 3; j++)
                        {
                            line(img_draw, P[j], P[(j + 1) % 4], Scalar(0, 255, 0), 2);
                        }
                        cv::Point2f center = (P[0] + P[1] + P[2] + P[3]) / 4;
                        cv::circle(img_draw, center, 5, cv::Scalar(0, 255, 0), -1);
                        c.push_back(center + cv::Point2f(left + 30, top + 30));
                    }
                }
            }
            m_img_draw = img_draw.clone();
        }
    }
    else{
        c.push_back(cv::Point2f{0, 0});
        qCritical() << "input image is empty !!";
    }
    return c;
}
cv::Mat algorithm::MLRectangleFiducialDetect::getImgDraw()
{
    return m_img_draw;
}
#include "ml_rectangleDetection.h"
#include"LogPlus.h"
algorithm::RectangleDetection::RectangleDetection()
{
}

algorithm::RectangleDetection::~RectangleDetection()
{
}

cv::RotatedRect algorithm::RectangleDetection::getRectangleBorder(cv::Mat img)
{
    RotatedRect re;
    string infoHeader = "---Algorithm log---RectangleDetectionALG---";
    if (img.data!=NULL)
    {
        cv::Mat img_draw,gray;
        cv::Mat img8 = convertToUint8(img);
        if (img8.channels() == 3)
        {
            img_draw = img.clone();
            cv::cvtColor(img8, gray, COLOR_BGR2GRAY);
        }
        else if (img8.channels() == 1)
        {
            gray = img8.clone();
            cv::cvtColor(img8, img_draw, COLOR_GRAY2BGR);
        }
        cv::medianBlur(gray, gray, 5);
        cv::Mat imgth;
        cv::threshold(gray, imgth, 0, 255, THRESH_OTSU);
        vector<vector<Point>> contours;
        vector<Vec4i> hierachy;
        findContours(imgth, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        vector<cv::Point2f> centers;
      //  drawContours(img_draw, contours, -1, Scalar(0, 0, 255), 10);
        for (int i = 0; i < contours.size(); i++)
        {
            double area = cv::contourArea(contours[i]);
            cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
            Point2f P[4];
            rectR.points(P);
            double w = Getdistance(P[0], P[1]);
            double h = Getdistance(P[1], P[2]);
            double ratio = min(w, h) / max(w, h);
            if (area>5e4 /*&& ratio>0.6 && ratio<1.2*/)
            {
                re = rectR;
                cv::line(img_draw,P[0],P[1],Scalar(0,0,255),1);
                cv::line(img_draw, P[1], P[2], Scalar(0, 0, 255), 1);
                cv::line(img_draw, P[2], P[3], Scalar(0, 0, 255), 1);
                cv::line(img_draw, P[0], P[3], Scalar(0, 0, 255), 1);
                m_img_draw = img_draw.clone();
                break;
            }          
        }
       
    }
    else
    {
        string message = infoHeader +"The input image is NULL";
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
    }
    return re;
}

cv::Mat algorithm::RectangleDetection::getImgdraw()
{
    return m_img_draw;
}

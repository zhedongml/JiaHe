#include "ml_BMWfiducialDetect.h"
#include "libcbdetect/boards_from_corners.h"
#include "libcbdetect/config.h"
#include "libcbdetect/find_corners.h"
#include "libcbdetect/plot_boards.h"
#include "libcbdetect/plot_corners.h"
#include <opencv2/imgproc/types_c.h>
#include <QDebug>
using namespace algorithm;
using namespace cv;

algorithm::MLBMWFiducialDetect::MLBMWFiducialDetect()
{
}
algorithm::MLBMWFiducialDetect::~MLBMWFiducialDetect()
{
}
vector<cv::Point2f> algorithm::MLBMWFiducialDetect::BMWFiducialDetect(cv::Mat img)
{
    std::vector<cv::Point2f> location;
  
     location = BMWContoursDetect(img);
    if (location.size()<1)
    {
        location = BMWHoughLineDetect(img);
    }
    else if (location.size()<1)
    {
        location = BMWCornersDetect(img);
    }
       
    return location;
}
cv::Rect algorithm::MLBMWFiducialDetect::GetValidROI(cv::Mat img)
{
    cv::Rect rect = Rect(0, 0, -1, -1);
    cv::Mat temp = cv::imread("./config_iqt/IQT Template/fiducalTemplate.tif");
    if (img.data != NULL && temp.data != NULL)
    {
        vector<Point> P;
        GetAllMinLoc(img, temp, sim, Scalar(0, 0, 0), &P, 1);
        cv::Rect rect;
        if (P.size() > 0)
        {
            rect = cv::Rect(P[0].x, P[0].y, temp.cols, temp.rows);
            cv::Mat roi = img(rect);
            cv::Mat roi8 = convertToUint8(roi);
        }
        return rect;
    }
    else
        qCritical() << "Template is empty !!";
        return rect;
}
cv::Mat algorithm::MLBMWFiducialDetect::GetProcessedImage(cv::Mat img)
{
    cv::Mat ProcessedImg;
    if (img.data != NULL)
    {
        cv::Mat img8 = convertToUint8(img);
        cv::GaussianBlur(img8, img8, cv::Size(3, 3), 0, 0);
        Mat srcbinary;
        threshold(img8, srcbinary, 0, 255, THRESH_TRIANGLE);
        cv::Mat element = getStructuringElement(MORPH_RECT, Size(12, 12));
        cv::Mat srcbinary1;
        cv::morphologyEx(srcbinary, srcbinary1, MORPH_CLOSE, element);
        MLimagePublic pu;
        pu.ContoursRemoveNoise(srcbinary1, 10);
        ProcessedImg = srcbinary1.clone();
    }
    return ProcessedImg;
    // int errCode = 0;
    // cbdetect::Corner corners;
    // cbdetect::Params params;
    // params.corner_type = cbdetect::SaddlePoint;
    // params.show_debug_image = false;
    // params.find_corners_reiszed = false;
    // params.detect_method = cbdetect::LocalizedRadonTransform;
}
std::vector<cv::Point2f> algorithm::MLBMWFiducialDetect::BMWHoughLineDetect(cv::Mat img)
{
    std::vector<cv::Point2f> center_cross;
    cv::Point2f pt = cv::Point2f(0, 0);
    if (img.data != NULL)
    {
        cv::Mat img_draw = convertToUint8(img);
        cv::Rect rect = GetValidROI(img);
        cv::Mat roi = GetROIMat(img, rect);
        cv::Mat roiimg = GetProcessedImage(roi);
        if (roiimg.data){
            cv::Mat roi_draw = convertToUint8(roiimg);
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));
            Mat img_grad;
            cv::morphologyEx(roiimg, roiimg, cv::MORPH_CLOSE, element);
            cv::morphologyEx(roiimg, img_grad, cv::MORPH_GRADIENT, element);
            Mat c_img;
            cv::Canny(img_grad, c_img, 50, 150, 3);
            cv::Mat m;
            int img_rows_ = roiimg.rows;
            int img_cols_ = roiimg.cols;
            c_img.convertTo(m, CV_64FC1);
            cv::Mat m_row(1, img_cols_, CV_64FC1, cv::Scalar(0));
            cv::Mat m_col(img_rows_, 1, CV_64FC1, cv::Scalar(0));
            cv::reduce(m, m_row, 0, cv::REDUCE_SUM);
            cv::reduce(m, m_col, 1, cv::REDUCE_SUM);
            int numR = countNonZero(m_row);
            int numC = countNonZero(m_col);
            int l = (numR < numC ? numR : numC) / 6;
            std::vector<cv::Vec4i> lines;
            cv::HoughLinesP(c_img, lines, 1, CV_PI / 180.0, l, l, l);
            for (int i = 0; i < lines.size(); i++)
            {
                line(roi_draw, cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3]),
                     cv::Scalar(0, 0, 255));
            }
            algorithm::MLCrossCenter my_cross;
            int width1, width2;
            pt = my_cross.CalculateCross(lines, width1, width2);
            if (width1 > 20 || width2 > 20)
            {
                lines.clear();
                l = l * 1.5;
                cv::HoughLinesP(c_img, lines, 1, CV_PI / 180.0, l, l, l);
                for (int i = 0; i < lines.size(); i++)
                {
                    line(roi_draw, cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3]),
                         cv::Scalar(255, 0, 255));
                }
                pt = my_cross.CalculateCross(lines, width1, width2);
            }
            if (pt.x != 0 && pt.y != 0)
            {
                pt.x = pt.x + rect.x;
                pt.y = pt.y + rect.y;
                center_cross.push_back(pt);
                circle(img_draw, pt, 2, cv::Scalar(255, 0, 0), -1);
                m_img_draw = img_draw.clone();
            }
        }
        else{
            center_cross.push_back(pt);
            qCritical() << "Processed image is empty !!";
        }
    }
    else{
        center_cross.push_back(pt);
        qCritical() << "input image is empty !!";
    }
    return center_cross;
}

std::vector<cv::Point2f> algorithm::MLBMWFiducialDetect::BMWContoursDetect(cv::Mat img)
{
    std::vector<cv::Point2f> center_cross;
    cv::Point2f pt = {0, 0};
    if (img.data!=NULL)
    {
        cv::Mat img_draw = convertToUint8(img);
        cv::Rect rect = GetValidROI(img);
        cv::Mat roi = GetROIMat(img, rect);
        cv::Mat roiimg = GetProcessedImage(roi);
        if (roiimg.data!=NULL){
            cv::Mat roi_draw = roiimg.clone();
            vector<vector<Point>> contours;
            vector<Vec4i> hierachy;
            findContours(roiimg, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
            vector<cv::Point2f> centers;
            for (int i = 0; i < contours.size(); i++)
            {
                double area = contourArea(contours[i], false);
                cout << area << endl;
                if (area > 5e3 && area < 1e5)
                {
                    drawContours(roi_draw, contours, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 1, 8,
                                 hierachy);
                    RotatedRect rect = minAreaRect(contours[i]);
                    Point2f P[4];
                    rect.points(P);
                    double w = Getdistance(P[0], P[1]); // abs(P[0].x - P[2].x);
                    double h = Getdistance(P[0], P[3]); // abs(P[0].y - P[2].y);
                    double asp = max(w, h) / min(w, h);
                    if (w > 50 & w<300 & asp> 0.8 & asp < 1.5)
                    {
                        for (int j = 0; j <= 3; j++)
                        {
                            line(roi_draw, P[j], P[(j + 1) % 4], Scalar(0, 0, 255), 2);
                        }
                        cv::Point2f c = (P[0] + P[1] + P[2] + P[3]) / 4;
                        centers.push_back(c);
                        circle(roi_draw, c, 5, cv::Scalar(255, 0, 0), -1);
                    }
                }
            }
            if (centers.size() == 2)
            {
                pt.x = (centers[0].x + centers[1].x) / 2.0;
                pt.y = (centers[0].y + centers[1].y) / 2.0;
            }
            else if (centers.size() == 1)
            {
                pt = centers[0];
            }
            if (pt.x != 0 && pt.y != 0)
            {
                center_cross.push_back(pt + cv::Point2f(rect.x, rect.y));
                circle(img_draw, pt + cv::Point2f(rect.x, rect.y), 2, cv::Scalar(255, 0, 0), -1);
                m_img_draw = img_draw.clone();
            }
        }
        else
        {
            center_cross.push_back(pt);
            qCritical() << "Processed image is empty !!";
        }
    }
    else{
        center_cross.push_back(pt);
        qCritical() << "input image is empty !!";
    }
    return center_cross;
}

std::vector<cv::Point2f> algorithm::MLBMWFiducialDetect::BMWCornersDetect(cv::Mat img)
{
    std::vector<cv::Point2f> center_cross;
    cv::Point2f pt = {0, 0};
    if (img.data!=NULL)
    {
        cv::Mat img_draw = convertToUint8(img);
        cv::Rect rect = GetValidROI(img);
        cv::Mat roi = GetROIMat(img, rect);
        cv::Mat roiimg = GetProcessedImage(roi);
        if (roiimg.data!=NULL)
        {
            std::vector<cv::Point2f> locations;
            std::vector<cv::Point2f> target;
            goodFeaturesToTrack(roiimg, locations, 5, 0.01, 10, Mat(), 3, false, 0.04);
            cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 40, 0.01);
            cv::cornerSubPix(roiimg, locations, cv::Size(5, 5), cv::Size(-1, -1), criteria);
            std::vector<float> vx, vy;
            for (int i = 0; i < 5; ++i)
            {
                vx.push_back(locations[i].x);
                vy.push_back(locations[i].y);
            }
            int x1 = max_element(vx.begin(), vx.end()) - vx.begin();
            int x2 = min_element(vx.begin(), vx.end()) - vx.begin();
            int y1 = max_element(vy.begin(), vy.end()) - vy.begin();
            int y2 = min_element(vy.begin(), vy.end()) - vy.begin();

            for (int i = 0; i < 5; ++i)
            {
                if (i != x1 && i != x2 && i != y1 && i != y2)
                    target.push_back(locations[i]);
            }
            if (target.size() == 1)
            {
                pt = target[0];
            }
            if (pt.x != 0 && pt.y != 0)
            {
                center_cross.push_back(pt + cv::Point2f(rect.x, rect.y));
                circle(img_draw, pt + cv::Point2f(rect.x, rect.y), 2, cv::Scalar(255, 0, 255), -1);
                m_img_draw = img_draw.clone();
            }
        }
        else{
            center_cross.push_back(pt);
            qCritical() << "Processed image is empty !!";
        }
    }
    else{
        center_cross.push_back(pt);
        qCritical() << "input image is empty !!";
    }
    return center_cross;
}
cv::Mat algorithm::MLBMWFiducialDetect::getImgDraw()
{
    return m_img_draw;
}
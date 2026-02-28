#pragma once

#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include <QMap>
#include "Result.h"
#include "maalignment_global.h"

using namespace std;
using namespace cv;

struct FiducialInfo{
    std::string fiducialType = "Circle";
    int fiducialNo;
    bool isCenter;

    FiducialInfo(){
    }

    FiducialInfo(std::string fiducialTypeT, int fiducialNoT, bool isCenterT)
    {
        fiducialType = fiducialTypeT;
        fiducialNo = fiducialNoT;
        isCenter = isCenterT;
    }
};

class MAALIGNMENT_EXPORT FiducialRecognize
{
public:
    ~FiducialRecognize();
    static FiducialRecognize *instance();
    
public:
    cv::Mat getImgDraw();
    Result circleDetect(cv::Mat src, cv::Point2f &center, FiducialInfo fiducialInfo);
    Result circleDetectInputFiducial(cv::Mat src, cv::Point2f &center, std::string fiducialType);

    Result circleProjectorDetect(cv::Mat src, cv::Point2f &center);

    Result circleDetectTemp(cv::Mat src, cv::Point2f& center, FiducialInfo fiducialInfo);

private:
    FiducialRecognize();

    Result circleDetect(cv::Mat src, Rect rect, int circleRadius, cv::Point2f &center, int circleRadiusMax = 0);

    Result circleRadius(string fileName, float &radius, Rect &rect);

    Result getTmplRadius(std::string fiducialType, int fiducialNo, float &radius);

    void myfilter(Mat& image_input, Mat& image_output); //4
    void myfilter2(Mat& image_input, Mat& image_output); //8

    void test();

    Result circleDetectMatchTemp(cv::Mat src, cv::Mat temp, Rect rect, int circleRadius, cv::Point2f& center, int circleRadiusMax = 0);
    Mat FiducialRecognize::createHollowCircleTemplate(int size, int thickness);
    Result readTemp(float &radius, cv::Mat& temp, string fiducialType, int fiducialNo);

private:
    //fiducialType(Circle), fiducial number, radius
    QMap<QString, QMap<int, float>> m_radiusMap;
    QMap<QString, QMap<int, cv::Mat>> m_tempMap;

    float m_radiusProjector = 0;
    cv::Mat m_img_draw;
    static FiducialRecognize *m_instance;
};
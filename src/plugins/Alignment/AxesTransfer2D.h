#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>
#include "Result.h"

using namespace std;
using namespace cv;

struct FiducialBase2D {
    vector<cv::Point2f> fiudcialWafer;
    vector<cv::Point2f> fiducialACS;
};

struct TransResult2D
{
    vector<cv::Point2f> eyeboxAcsList;
    float angleDeg;
    Result result;
};

struct RigidTrans2D {
    double angleRad = 0.0f;         
    cv::Point2f translation;
};

class AxesTransfer2D
{
public:
    AxesTransfer2D();
    ~AxesTransfer2D();

    Result rigidTransform(RigidTrans2D & rigidTrans2D, const vector<cv::Point2f>& fiudcialWafer, const vector<cv::Point2f>& fiducialACS);
    vector<cv::Point2f> applyTransformList(const vector<cv::Point2f>& ptList, RigidTrans2D rigidTrans2D);
    cv::Point2f applyTransform(const cv::Point2f &point, RigidTrans2D rigidTrans2D);

    TransResult2D getEyeboxAcs(const FiducialBase2D& fiducialBase2D, const vector<cv::Point2f> &eyeboxWafer);

    cv::Point2f relativeAcs(const cv::Point2f& waferRelPos, const RigidTrans2D &rigidTrans2D);

private:
    RigidTrans2D estimateRigidTransform(const vector<cv::Point2f>& pointsA, const vector<cv::Point2f>& pointsB);

    float normalizeAngleDeg(float angle);
    void computeTransform(
        const cv::Point2f& A1, const cv::Point2f& A2,
        const cv::Point2f& B1, const cv::Point2f& B2,
        float& angleRadRet, cv::Point2f& translation);

    cv::Point2f applyTransform(const cv::Point2f& pt, float angleRad, const cv::Point2f& translation);

    bool judgeTransform(const vector<cv::Point2f> &fiudcialWafer, const vector<cv::Point2f> &fiducialACS, RigidTrans2D rigidTrans2D);

    // test code
    void testTransform(const cv::Point2f& A1, const cv::Point2f& A2,
        const cv::Point2f& B1, const cv::Point2f& B2);
    void test();
};


#pragma once
#include <opencv2/opencv.hpp>
#include "piMotionActor/Motion3DModel.h"
#include "PrjCommon/service/ml.h"
#include "PrjCommon/service/ml_motion.h"
#include "piMotionActor/ConfigMAInfo.h"
#include "piMotionActor/Motion2DModel.h"
#include <QThread>

#define PIXEL_SIZE 3.45
#define MAGNIFICATION 0.5

struct AAlignConfig
{
    bool autoMode = false;
    cv::Point2f inView;
    cv::Point2f outView;
    DistanceLimit distanceLimit;
    float stepZ;
    int fiducialSn = 0;
    std::string fiducialTypeStr = "Circle";
};

class AlignUtils
{
  public:
    static Result acsAbsoluteMove(cv::Point3f &acsCoord);
    static Result acsAbsoluteMove(const cv::Point2f &acsCoord);
    static Result acsRelMove(const cv::Point3f &acsCoord);
    static Result acsRelMove(const cv::Point2f &acsCoord);

    static void acsMoveStop(bool stop);

    static Result getAlignConfig(AAlignConfig &config, DUT_EYE_TYPE eyeType);
    static Result controlLight(bool IS, bool coaxialLight);
};

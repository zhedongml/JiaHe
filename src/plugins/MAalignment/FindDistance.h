#pragma once
#include "windows.h"
#include <QObject>
#include <string>
#include "PrjCommon/service/ml_motion.h"
#include <json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include "piMotionActor\ConfigMAInfo.h"
#include "WaitPause.h"

using namespace CORE;
using namespace std;

//using json = nlohmann::ordered_json;

struct DutDistanceInfo
{
    cv::Point2f inView;
    DistanceLimit distanceLimit;
    float stepZ;

    DutDistanceInfo(){}
    DutDistanceInfo(cv::Point2f inViewT, DistanceLimit distanceLimitT, float stepZT)
    {
        inView = inViewT;
        distanceLimit = distanceLimitT;
        stepZ = stepZT;
    }
};

class FindDistance:public QObject
{
    Q_OBJECT
 public:
    FindDistance();
    ~FindDistance();
    Result getBestDistance(float &dutZ, const DutDistanceInfo &dutDistanceInfo);

    // continue or stop
    void continueRun(bool isContinue);
    void notifyPause(bool isPause);

  private:
    FindDistance *GetInstance();
    Result initMove(cv::Point2f inView, const DistanceLimit &config);
    Result waitPause(bool isPause);

    Result calculateByStep(float posMin, float posMax, float step, float &bestPos);
    Result alignBestDistance(float &dutZ, const DutDistanceInfo &dutDistanceInfo);

    // From algorithm::MLImageQuality().tenengard(img);
    double tenengard(cv::Mat& image);
  private:
    FindDistance* self = nullptr;

    vector<float> m_imageQuality;
    vector<float> m_distance;

    WaitPause m_waitPause;
};

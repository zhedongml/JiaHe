#pragma once
#include "pimotionactor_global.h"
#include "PrjCommon/service/ml.h"
#include "opencv2/opencv.hpp"
#include <QList>
#include <QMap>
#include <QString>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <vector>
#include "Result.h"

using namespace CORE;
using Json = nlohmann::json;

struct DutMcCollision{
    //DUT coordinate - measure camera coordinate, when MC lens alignment DUT holder center
    cv::Point3f centerRelative;

    // DUT holder center rectangle: LengthX WidthY
    cv::Point2f rectangle;

    float minDistanceZ;
};

struct EyeboxLimit
{
    cv::Point3f min;
    cv::Point3f max;

    cv::Point2f dutAngle;
    cv::Point2f projectorAngle;
};

struct DutProjectorCollision : public DutMcCollision
{
};

class PIMOTIONACTOR_EXPORT MoveLimitConfig
{
  public:
    static MoveLimitConfig *instance();
    ~MoveLimitConfig();

    Result load();

    CoordinateLimit getDutLimit();
    CoordinateLimit getMeasureCameraLimit();
    DutMcCollision getDutMcCollision();
    DutProjectorCollision getDutProjectorCollision();

    cv::Point3f getLoadDutPosition();
    cv::Point3f getLoadMeasureCameraPosition();

    cv::Point3f getMotion3DMoveInitSpeed();
    cv::Point3f getMotion3DMoveAlignSpeed();

    QMap<int, QList<EyeboxLimit>> getEyeboxLimit();

  private:
    MoveLimitConfig();
    void test();
    QList<EyeboxLimit> getEyeboxLimit(Json json);

  private:
    const QString FILE_NAME = "./config/AlignConfig/moveLimit.json";
    const QString FILE_NAME_EYEBOX_LIMIT = "./config/AlignConfig/eyeboxLimit.json";
    static MoveLimitConfig *m_instance;
    Json m_json;
    Json m_jsonEyeboxLimit;
};

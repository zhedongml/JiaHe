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

struct DutProjectorCollisionJX
{
    cv::Point2f proAngle;

    //DUT coordinate - measure camera coordinate, when MC lens alignment DUT holder center
    cv::Point2f centerRelative;

    // DUT holder center rectangle: LengthX WidthY
    cv::Point2f rectangle;
};

struct DutMcCollisionJX {
    cv::Point2f mcAngle;

    //DUT coordinate - measure camera coordinate, when MC lens alignment DUT holder center
    cv::Point2f centerRelative;

    // DUT holder center rectangle: LengthX WidthY
    cv::Point2f rectangle;

    float mcMaxZ;
};

struct McProjectorCollisionJX {
    cv::Point2f mcAngle;
    cv::Point2f proAngle;

    // Measure camera coordinate, when MC lens alignment Projector
    cv::Point3f centerRelative;

    // center rectangle: LengthX WidthY
    cv::Point3f rectangle;

    float mcMinX;
};

struct EyeboxLimitJX
{
    cv::Point3f min;
    cv::Point3f max;

    cv::Point2f measureAngle;
    cv::Point2f projectorAngle;
};

class PIMOTIONACTOR_EXPORT MoveLimitConfigJX
{
  public:
    static MoveLimitConfigJX *instance();
    ~MoveLimitConfigJX();

    Result load();

    Limit2D getDutLimit();
    CoordinateLimit getMeasureCameraLimit();
    Result getDutMcCollision(DutMcCollisionJX& collision);
    Result getDutProjectorCollision(DutProjectorCollisionJX& collision, cv::Point2f proAngle = cv::Point2f(100, 100));
    Result getMcProjectorCollision(McProjectorCollisionJX& collision, cv::Point2f proAngle = cv::Point2f(100, 100));

    cv::Point2f getLoadDutPosition();
    cv::Point3f getLoadMeasureCameraPosition();

    cv::Point2f getInitTestDutPosition();
    cv::Point3f getInitTestMcPosition();

    cv::Point3f getMotion3DMoveInitSpeed();
    cv::Point3f getMotion3DMoveAlignSpeed();

    QMap<int, QList<EyeboxLimitJX>> getEyeboxLimit();

  private:
    MoveLimitConfigJX();
    void test();
    QList<EyeboxLimitJX> getEyeboxLimit(Json json);

    void getDefaultAngle(cv::Point2f & mcAngle, cv::Point2f& proAngle);
    QList<DutMcCollisionJX> getDutMcCollisionList();
    QList<DutProjectorCollisionJX> getDutProjectorCollisionList();
    QList<McProjectorCollisionJX> getMcProjectorCollisionList();

  private:
    const QString FILE_NAME = "./config/AlignConfig/moveLimitJX.json";
    const QString FILE_NAME_EYEBOX_LIMIT = "./config/AlignConfig/eyeboxLimitJX.json";
    static MoveLimitConfigJX *m_instance;
    Json m_json;
    Json m_jsonEyeboxLimit;
    const float MAX_ERROR_VALUE = 0.05;
};

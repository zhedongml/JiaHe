#pragma once
#include "plugincore_global.h"
#include "../service/ml_motion.h"
#include <QString>
#include <fstream>
#include <iostream>
#include <json.hpp>

using Json = nlohmann::json;
using namespace std;

class PRJCOMMON_EXPORT AlignConfig
{
  public:
    static AlignConfig *instance();
    ~AlignConfig();

    QString getMotion2DIpAndOther(int type);
    QString getMotion3DIpAndOther(int type = 0);

    Limit2D getMotion2DLimits(int type);
    CoordinateLimit getMotion3DLimits(int type = 0);

    QString getCameraSn();
    
    cv::Point2f getMotion2DZeroPositionRaw(int type, DUT_EYE_TYPE eyeType);
    cv::Point2f getMotion2DZeroPosition(int type, DUT_EYE_TYPE eyeType);
    void updateMotion2DZeroPosition(int type, DUT_EYE_TYPE eyeType, cv::Point2f tilt, bool forced = false);
    void setRealTimeUpdate(bool realTime);
    bool getRealTimeUpdate();

    Result writeDutZeroPosition(DUT_EYE_TYPE eyeType, cv::Point2f tilt);

  private:
    AlignConfig();
    Result loadFile();

    cv::Point2f getMotion2DZeroPositionOrigin(int type, DUT_EYE_TYPE eyeType = DUT_EYE_LEFT);
    QString getMotionType(Json motion, QString type);

  private:
    const QString FILE_MOTION = "./config/device/MotionJX.json";
    const QString FILE_CAMERA = "./config/device/camera.json";
    static AlignConfig *self;
    Json m_cameraJson;
    Json m_motionJson;

    // int type, int eyeType, zero pos
    QMap<int, QMap<int, cv::Point2f>> m_2DZeroPos;
    bool m_realTimeUpdate = false;
};
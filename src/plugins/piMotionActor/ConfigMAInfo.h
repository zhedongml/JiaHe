#pragma once

#include "PrjCommon/service/ml.h"
#include "pimotionactor_global.h"
#include <atomic>
#include <json.hpp>
#include <mutex>
#include <string>
#include <qstring.h>
#include "opencv2/opencv.hpp"
#include "PrjCommon/service/ml_motion.h"

using namespace CORE;
using json = nlohmann::json;

typedef struct
{
    float stepZ;
} MAInfo;

class PIMOTIONACTOR_EXPORT ConfigMAInfo
{
  public:
    static ConfigMAInfo *GetInstance()
    {
        static ConfigMAInfo _config;
        return &_config;
    }

  private:
    ConfigMAInfo()
    {
    }
    ~ConfigMAInfo()
    {
    }
    ConfigMAInfo(ConfigMAInfo const &) = delete;
    ConfigMAInfo &operator=(ConfigMAInfo const &) = delete;

  public:
    bool Load();
    MAInfo ReadMAInfo();
    MAInfo GetMAInfo();

    Result getMoveOutInputgratingOffset(cv::Point2f &offset);
    QString getReticle();
    void getIntergratingSphere(QString &sulution, float &scale);

    // SLB alignment config
    Result getSafePositonSLB(cv::Point3f &pos);
    Result getAlignPositonSLB(cv::Point3f &pos);
    Result getProjectorTiltSLB(cv::Point2f &pos);
    Result getSafeTiltSLB(cv::Point2f& pos);
    Result getReticleSLB(QString &reticle);
    Result getIntergratingSphereSLB(QString &sulution, float &scale);
    Result updateSLB(const cv::Point3f &mcPosition, const cv::Point2f &projectorTilt);

    bool isDisplayUI();

  private:
    const QString FILE_NAME = "./config/AlignConfig/inputAlign.json";
    MAInfo m_MAInfo;
    json m_JsonData;
    std::mutex m_mutex;

    const QString SLB_FILE_NAME = "./config/AlignConfig/SLBAlign.json";
    json m_jsonSLB;
};

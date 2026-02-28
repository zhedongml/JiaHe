#pragma once
#include "opencv2/opencv.hpp"
#include <QList>
#include <QMap>
#include <QString>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <vector>
#include "pimotionactor_global.h"
#include "Result.h"
#include "calibrationconfig.h"

using Json = nlohmann::json;
using namespace std;

class PIMOTIONACTOR_EXPORT FiducialConfig
{
public:
    static FiducialConfig *instance();

    // fiducialNumber: 1,2,3
    std::string getTempleteFilePath(std::string fiducialType, int fiducialNumber);
    std::string getInputFiducialFilePath(std::string fiducialType);
    int getInputFiducialNumber(std::string fiducialType);

    bool fiduciualMotionEnabled();

    Result getFiduciualMotionPosList(std::string fiducialType, vector<cv::Point3f> &fidMotionList);
    Result writeFiduciualMotionPosRelativeDut(std::string fiducialType, const cv::Point3f &dutMotion,
                                              const vector<cv::Point3f> &mcMotions);

    void setFiducialCenter(bool isCenter);
    bool getFiducialCenter();

  private:
    FiducialConfig();
    ~FiducialConfig();

    void load();

    Result getFiduciualMotionPosList_old(std::string fiducialType, vector<cv::Point3f> &fidMotionList);
    Result writeFiduciualMotionPosRelativeDut_old(std::string fiducialType, const cv::Point3f &dutMotion,
                                              const vector<cv::Point3f> &mcMotions);

    Result getFiduciualMotionPosRelativeDut_old(std::string fiducialType, vector<cv::Point3f> &fidMotionList);
    Result getFiduciualMotionPosRelativeDut(std::string fiducialType, vector<cv::Point3f> &fidMotionList);
    
    Result getFiduciualMotionPosRelativeDut_old(std::string fiducialType, int eyeType, cv::Point2f dutTilt,
                                            vector<cv::Point3f> &fidMotionList);
    Result getFiduciualMotionPosRelativeDut(std::string fiducialType, int eyeType,
                                            cv::Point2f dutTilt, vector<cv::Point3f> &fidMotionList);

    bool judgeDutInfoSame(const DutConfig &dutConfig, Json eyeTypeOne);

  private:
    static FiducialConfig *m_instance;
    const QString FILE_NAME = "./config/AlignConfig/FiducialConfig.json";
    Json m_json;

    bool m_isFiducialCenter = false;
};

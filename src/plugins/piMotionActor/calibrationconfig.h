#pragma once
#include "PrjCommon/service/ml.h"
#include "opencv2/opencv.hpp"
#include <QList>
#include <QMap>
#include <QString>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <vector>
#include "pimotionactor_global.h"

#define CC_MOTION_3D_X "motion3DX"
#define CC_MOTION_3D_Y "motion3DY"
#define CC_DUT_X "dutX"
#define CC_DUT_Y "dutY"
#define CC_MV_X "mvX"
#define CC_MV_Y "mvY"

using Json = nlohmann::json;
using namespace CORE;

struct MoveOutInfo
{
    bool enable = false;
    cv::Point2f offset;

    MoveOutInfo(){}
    MoveOutInfo(bool enableT, cv::Point2f offsetT)
    {
        enable = enableT;
        offset = offsetT;
    }
};

struct DutConfig
{
    QString sn;
    int eyeType = 0;
    float eyeRelief = 0.0f;
    std::vector<cv::Point3f> fiducials;
    std::vector<cv::Point3f> eyeboxs;
    cv::Point3f eyeboxCenter;
    cv::Point3f inputGrating;
    std::string exeboxOffset;
    std::vector<cv::Point3f> exeboxOffsetAll;
    MoveOutInfo moveOutInfo;
    cv::Point3f mvRelativeDutInputGrating;
    bool calculateMvFidsByInputFlag;
    std::vector<cv::Point3f> motPosOfFids;

    void clear()
    {
        eyeType = 0;
        eyeRelief = 0.0f;
        fiducials.clear();
        eyeboxs.clear();
    }
};

struct MvExpose
{
    bool isAuto = true;
    int manualExposeTime = 500;

    MvExpose()
    {
    }
    MvExpose(bool isAutoT, int manualExposeTimeT)
    {
        isAuto = isAutoT;
        manualExposeTime = manualExposeTimeT;
    }
};

struct AlignConfig
{
    bool autoMode = 0;
    int fiducialType = CORE::FiducialType::CIRCLE;
    std::string fiducialTypeStr = "Circle";
    int fiducialCount = 2;
    bool moveOut = false;
    MvExpose mvExpose;
    bool calculateMvFidsByInputFlag = false;

    AlignConfig()
    {
    }
    AlignConfig(bool autoModeT)
    {
        autoMode = autoModeT;
    }

    void setInfo(AlignConfig *info)
    {
        autoMode = info->autoMode;
        fiducialType = info->fiducialType;
        fiducialCount = info->fiducialCount;
    }
};

struct CalibrateConfig
{
    // motion3DX=dutY, motion3DY=-dutX
    QMap<QString, QString> motion3DAndDutCoordRelate;
    QMap<QString, QString> motion3DAndMVCoordRelate;
    cv::Point2f measureCameraAngle = cv::Point2f(0, 0);
    cv::Point2f dutAngleXY = cv::Point2f(0, 0);
    QMap<int, cv::Point3f> acsMvCoordinateOfFiducials;

    float mvFocus;

    void clear()
    {
        motion3DAndDutCoordRelate.clear();
        motion3DAndMVCoordRelate.clear();
        acsMvCoordinateOfFiducials.clear();
        measureCameraAngle = cv::Point2f(0, 0);
    }

    void setAcsMvCoordinateOfFiducials(std::vector<cv::Point3f> vector)
    {
        for (int i = 0; i < vector.size(); ++i)
        {
            acsMvCoordinateOfFiducials[i] = vector.at(i);
        }
    }
};

struct AlignBaseInfo
{
    QString sn;

    // fiducial relative coordinates in dut config file
    std::vector<cv::Point3f> fiducials;

    // eyebox relative coordinates in dut config file
    std::vector<cv::Point3f> eyeboxs;

    // input grating relative coordinates in dut config file
    cv::Point3f inputGrating;

    // ACS coordinates when the MV camera can capture fiducial
    std::vector<cv::Point3f> acsFiducials;

    // i29 coordinates relative to mv camera, MV - lens coordinate
    cv::Point3f i29RelativeMvCoord;

    CoordinateLimit acsCoordLimit;

    QList<CoordinateLimit> acsCoordLimitList;

public:
    AlignBaseInfo() {}

    ~AlignBaseInfo() {}

    bool judgeDataValidity() const
    {
        if (fiducials.size() == 0 || eyeboxs.size() == 0 || acsFiducials.size() == 0)
        {
            // return Result(false, "Align base info is null.");
            return false;
        }
        return true;
    }
};

struct EyeReliefBaseInfo
{
    double dueDistance;        // Eye-Relief due distance
    cv::Point3f acsCoordinate; // Acs coordinates convenient for keyence measurement
    CoordinateLimit acsCoordLimit;
    MinMax keyenceRange;
    double keyenceZeroDistance; // when keyence shows 0 value, real distance between keyence and dut

    cv::Point3f keyenceRelativeMvCoord;
    cv::Point3f i29RelativeMvCoord;
    double i29RelativeMvCoordZ; // coordinate Z difference

    EyeReliefBaseInfo()
    {
        dueDistance = 0.0f;
        keyenceZeroDistance = 0.0f;
    }
    ~EyeReliefBaseInfo()
    {
    }

    bool judgeDataValidity() const
    {
        if (qFuzzyIsNull(dueDistance) || qFuzzyIsNull(keyenceZeroDistance))
        {
            // return Result(false, "Eye relief base info is null.");
            return false;
        }
        return true;
    }
};

struct AlignBaseInfoOneFiducial : public AlignBaseInfo
{
    cv::Point2f measureCameraTiltAngle = cv::Point2f(0, 0);
    QMap<QString, QString> motion3DAndDutCoordRelateOfOneFid; // motion3DX=dutY, motion3DY=-dutX

    AlignBaseInfoOneFiducial()
    {
    }
    AlignBaseInfoOneFiducial(AlignBaseInfo info)
    {
        fiducials = info.fiducials;
        eyeboxs = info.eyeboxs;
        acsFiducials = info.acsFiducials;
        i29RelativeMvCoord = info.i29RelativeMvCoord;
        acsCoordLimit = info.acsCoordLimit;
        acsCoordLimitList = info.acsCoordLimitList;
    }
};

struct AlignBaseInfoOneFiducialTilt : public AlignBaseInfoOneFiducial
{
    float measureNearDutAcsZ; // ACS Z value when measure camera is near dut
    float eyeRelief;

    AlignBaseInfoOneFiducialTilt()
    {
    }
    AlignBaseInfoOneFiducialTilt(AlignBaseInfo info)
    {
        fiducials = info.fiducials;
        eyeboxs = info.eyeboxs;
        acsFiducials = info.acsFiducials;
        i29RelativeMvCoord = info.i29RelativeMvCoord;
        acsCoordLimit = info.acsCoordLimit;
        acsCoordLimitList = info.acsCoordLimitList;
    }
};

struct AlignBaseInfo2D : public AlignBaseInfo
{
    // Mition3D X/Y when MV center align Projector aperture center 
    cv::Point2f mvAlignProjectorCoord;

    //TODO: MV Miotion3D and DUT Miotion2D relative X/Y coordinate when MV center and DUT input grating align
    cv::Point3f mvRelativeDutInputGrating;

    // motion3DX=dutY, motion3DY=-dutX
    QMap<QString, QString> motion3DAndDutCoordRelate;

    // motion3DX=mvY, motion3DY=-mvX 
    QMap<QString, QString> motion3DAndMVRelate;

    float eyeRelief = 30;

    AlignBaseInfo2D()
    {
    }
    AlignBaseInfo2D(AlignBaseInfo info)
    {
        sn = info.sn;
        fiducials = info.fiducials;
        eyeboxs = info.eyeboxs;
        inputGrating = info.inputGrating;
        acsFiducials = info.acsFiducials;
        i29RelativeMvCoord = info.i29RelativeMvCoord;
        acsCoordLimit = info.acsCoordLimit;
        acsCoordLimitList = info.acsCoordLimitList;
    }

    bool judgeDataValidity() const
    {
        if (fiducials.size() != 2 || eyeboxs.size() < 1)
        {
            // return Result(false, "Align base info is error.");
            return false;
        }

        if (motion3DAndMVRelate.size() == 0 || motion3DAndDutCoordRelate.size() == 0)
        {
            return false;
        }
        return true;
    }
};

struct AlignBaseInfo3D : public AlignBaseInfo
{
    float mvFocus;
    cv::Point2f dutAngleXY;

    // Move DUT away from projector, easy to capture fidicial image
    cv::Point2f dutOutOffset;
    CoordinateLimit dutCoordLimit;

    // motion3DX=dutY, motion3DY=-dutX
    QMap<QString, QString> motion3DAndDutCoordRelate;

    AlignBaseInfo3D()
    {
    }
    AlignBaseInfo3D(AlignBaseInfo info)
    {
        sn = info.sn;
        fiducials = info.fiducials;
        eyeboxs = info.eyeboxs;
        acsFiducials = info.acsFiducials;
        i29RelativeMvCoord = info.i29RelativeMvCoord;
        acsCoordLimit = info.acsCoordLimit;
        acsCoordLimitList = info.acsCoordLimitList;
    }

    bool judgeDataValidity() const
    {
        //if (fiducials.size() == 0 || eyeboxs.size() == 0 || acsFiducials.size() == 0)
        if (fiducials.size() == 0 || eyeboxs.size() == 0)
        {
            // return Result(false, "Align base info is null.");
            return false;
        }

        if (qFuzzyIsNull(mvFocus) || motion3DAndDutCoordRelate.size() == 0)
        {
            return false;
        }
        return true;
    }
};

struct AlignBaseInfoTilt : public AlignBaseInfo
{
    cv::Point2f measureCameraTiltAngle = cv::Point2f(0, 0);
    float measureNearDutAcsZ; // ACS Z value when measure camera is near dut
    float eyeRelief;

    AlignBaseInfoTilt()
    {
    }
    AlignBaseInfoTilt(AlignBaseInfo info)
    {
        fiducials = info.fiducials;
        eyeboxs = info.eyeboxs;
        acsFiducials = info.acsFiducials;
        i29RelativeMvCoord = info.i29RelativeMvCoord;
        acsCoordLimit = info.acsCoordLimit;
        acsCoordLimitList = info.acsCoordLimitList;
    }

  public:
    ~AlignBaseInfoTilt()
    {
    }

    bool judgeDataValidity() const
    {
        if (fiducials.size() == 0 || eyeboxs.size() == 0 || acsFiducials.size() == 0)
        {
            // return Result(false, "Align base info is null.");
            return false;
        }
        return true;
    }
};

struct DutRotationAxisInfo
{
    cv::Point2f axisLength;
    cv::Point2f initAngle;
};

class PIMOTIONACTOR_EXPORT CalibrationConfig
{
  public:
    static CalibrationConfig *instance();
    void load();

    cv::Point3f getMVCameraRelPos();
    cv::Point3f getKeyenceRelPos();
    cv::Point3f getMeasureRelPos();
    cv::Point3f getInputGratingForBaseZ();

    DutConfig loadDutConfig(const QString &sn);
    cv::Point3f loadEyeTypeConfig(CORE::DUT_EYE_TYPE type);
    bool loadDutBase();

    DutConfig &getDutConfigReference();
    DutConfig getDutConfig(const QString &sn = "");
    void updateDutEyeboxs();
    void updateDutEyeboxs(DutConfig &dutConfig);
    bool writeDutConfig(DutConfig config);
    Result deleteDutConfig(const QString &sn);
    bool writeMotPosOfFids(QString sn, const std::vector<cv::Point3f> &mvFiducials);
    bool writeEyeboxAcsCoords(QString sn, std::vector<cv::Point3f> eyeboxAcsCoords);
    std::vector<cv::Point3f> readEyeboxAcsCoords();

    QString getDefaultDutName();
    Result setDefaultDutName(QString sn);
    bool addSnName(QString sn);
    bool updateSnName(QString sn, QString snOld);
    bool deleteSnName(QString sn);

    AlignConfig &getAlignConfigReference();
    AlignConfig getAlignConfig();

    CalibrateConfig &getCalibrateConfig();
    std::vector<cv::Point3f> getFiducialMotion3DCoords();
    QMap<QString, QString> getMotion3DAndDutCoordRelate();
    QMap<QString, QString> getMotion3DAndMVCoordRelate();
    cv::Point2f getMeasureCameraAngle();
    cv::Point2f getDutAngleXY();
    float getMvFocus();
    QList<QString> getDutSns();

    float getDutRotationAxisLengthY(CORE::DUT_EYE_TYPE eyeType);
    DutRotationAxisInfo getDutRotationAxisLength(CORE::DUT_EYE_TYPE eyeType);

    // Measure camera lens infinitely close DUT, motion3D Z coordinate
    float getLensInfinitelyCloseDutMotionZ();
    cv::Point2f getMvAlignProjectorCoord();

  private:
    CalibrationConfig();
    ~CalibrationConfig();

    QMap<int, cv::Point3f> readFiducialMotion3DPos(Json jsonObj);
    std::vector<cv::Point3f > readEyeboxOffsetAll(Json jsonObj);
    std::vector<cv::Point3f> readFiducialMotion3DPosList(Json jsonObj);
    cv::Point3f readMVCameraRelPos(Json jsonObj);
    cv::Point3f readKeyenceRelPos(Json jsonObj);
    cv::Point3f readMeasureRelPos(Json jsonObj);
    QMap<QString, QString> readMotion3DAndDutCoordRelate(Json jsonObj);
    QMap<QString, QString> readMotion3DAndMVRelate(Json jsonObj);

    float readMvFocus(Json jsonObj);
    QMap<CORE::DUT_EYE_TYPE, cv::Point3f> readMvRelativeDutInputGrating(Json jsonObj);
    bool readCalculateMvFidsByInputFlag(Json jsonObj);
    MoveOutInfo getMoveOutInfo(Json jsonObj);

    QString dutFileName(QString sn = "");
    QString dutFileName(const std::string &sn);
    void refreshDut();
    Json dutJson(const std::string &sn);

    Json updateDutConfig(DutConfig config);
    Json addDutConfig(DutConfig config);

  private:
    static CalibrationConfig *self;

    CalibrateConfig m_calibrateConfig;
    DutConfig m_dutConfig;
    AlignConfig m_alignConfig;

    const QString FILE_NAME = "./config/AlignConfig/eyeboxAlignJX.json";
    const QString dutfilePath_name = "./config/AlignConfig/dutInfo/";
    const QString dutBasefile_name = "./config/AlignConfig/dutInfo/dutBase.json";
    Json m_JsonDutConfig;
    Json m_JsonCalibrateConfig;
    Json m_JsonDutBase;

};

#pragma once
#include "piMotionActor/calibrationconfig.h"
#include "PrjCommon/service/ml.h"
#include "PrjCommon/service/ml_motion.h"
#include <QMap>
#include <opencv2/opencv.hpp>
#include "piMotionActor/Motion3DModel.h"
#include "piMotionActor/configItem.h"

namespace Align
{
    enum SELECT_FIDUCIAL_CONTINUE
    {
        AUTO_TURN_MANUAL,
        MANUAL,
        END
    };
    
    struct MvImage
    {
        static cv::Point2f center;
        static cv::Point2f pixelConvert(cv::Point2f pixelCoord)
        {
            float pixelSize;
            float magnification;
            Result ret = ConfigItem::instance()->getCameraCoefficient(pixelSize, magnification);
            if(!ret.success){
                qCritical() << QString::fromStdString(ret.errorMsg);
                return pixelCoord * 3.45 / 0.5 / 1000.0;
            }
            return pixelCoord * pixelSize / magnification / 1000.0;
        }
    };
}

enum EYEBOX_INDEX
{
    EYEBOX_INDEX_CENTER = 0, // center
    EYEBOX_INDEX_UP,         // Y+
    EYEBOX_INDEX_DOWN,       // Y-
    EYEBOX_INDEX_LEFT,       // X-
    EYEBOX_INDEX_RIGHT       // X+
};

class AlignUtils
{
  public:
    static Result acsAbsoluteMove(const cv::Point3f &acsCoord);
    static Result acsAbsoluteMove(cv::Point3f &acsCoord, const CORE::CoordinateLimit &acsCoordLimit);
    static Result acsAbsoluteMove(cv::Point3f &acsCoord, const QList<CoordinateLimit> acsCoordLimitList);

    static Result acsRelativeMove(const cv::Point3f &acsCoord);

    static Result acsAbsoluteMoveWhenMoveOut(cv::Point3f acsCoord, const CORE::CoordinateLimit &limit);
    static Result acsAbsoluteMoveWhenMoveOut(cv::Point3f acsCoord, const QList<CORE::CoordinateLimit> &limitList);
    static Result acsRelativeMoveZ(double relativeZ, double min, double max);
    static Result getAcsCoordinate(cv::Point3f &acsCoord);
    static void acsMoveStop(bool stop);

    static AlignBaseInfo getAlignBaseInfo(int eyeType = 0);
    static AlignBaseInfoOneFiducial getAlignBaseInfoOneFiducial(int eyeType = 0);
    static AlignBaseInfoTilt getAlignBaseInfoTilt(int eyeType = 0);
    static AlignBaseInfoOneFiducialTilt getAlignBaseInfoOneFiducialTilt(int eyeType = 0);
    static Result getAlignBaseInfo3D(AlignBaseInfo3D &alignBaseInfo3D, int eyeType = 0);
    static Result getAlignBaseInfo2D(AlignBaseInfo2D& alignBaseInfo2D, int eyeType = 1);
    static EyeReliefBaseInfo getEyeReliefBaseInfo(int eyeType = 0);

    static std::vector<cv::Point3f> acsMvRelMoveCoordByFidToEyeboxOfOneFid(
        const cv::Point3f mvAlignFid, const cv::Point3f &fiducial, const QList<cv::Point3f> &eyeboxs,
        QMap<QString, QString> motion3DAndDutCoordRelate);

    static Result measureCameraTiltOffset(cv::Point2f &tiltOffset, float eyeRelief, cv::Point2f angle);
    static void relMoveAcsWhenMoveOut(cv::Point3f &acsCoord);

    static Result dutMoveOutInputgratingOffset(bool isOut, const cv::Point2f &offset, const CORE::CoordinateLimit &limit = CoordinateLimit());
    static Result dutMoveOutInputgratingOffset(bool isOut);
    static std::vector<cv::Point3f> eyeboxAddDutOffset(const std::vector<cv::Point3f> &eyeboxs, const cv::Point2f &offset);

    static cv::Point2f relMoveCoordDutToAcs(cv::Point2f coord, const QMap<QString, QString> &motion3DAndDutCoordRelate);
    static cv::Point2f relMoveCoordMvToAcs(cv::Point2f coord, const QMap<QString, QString>& motion3DAndMvCoordRelate);

    static CORE::FiducialType analyzeFiducialType(const QString &typeStr);
    static Result fiducialLight(bool isOpen, bool isCoaxialLight);
    static Result fiducialLight(bool isOpen);
    static std::vector<cv::Point3f> calculateEyeboxs(const cv::Point3f &center, float offset);
    static std::vector<cv::Point3f> calculateEyeboxs(const cv::Point3f &center, QList<float> offsets);
    static std::vector<cv::Point3f> calculateEyeboxs(const cv::Point3f &center, QList<cv::Point3f> offsets);

    static std::vector<cv::Point3f> mvToMeasureCameraCoordXY(const std::vector<cv::Point3f> &mvCoords, cv::Point3f i29RelativeMvCoord);
    static void dutMcSameDirection(DutConfig &dutconfig);

    static Result getMotion2DAngle(int type, cv::Point2f &angle);

    static Result eyeboxIndexFromName(const QString &eyeboxName, EYEBOX_INDEX &index);

  private:
    static Result getDutPosition(cv::Point3f &coordinate);
    static void dutMcSameDirection(DutConfig &dutconfig, const QMap<QString, QString> &motion3DAndDutCoordRelate);
};

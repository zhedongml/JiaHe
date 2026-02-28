#include "AlignUtils.h"
#include "piMotionActor\ConfigMAInfo.h"
#include <corecrt_math.h>
#include <piMotionActor\Motion2DModel.h>
#include <QDebug>
#include <plugins/PrjCommon/metricsdata.h>
#include "CaptureFiducialImage.h"
#include "PLCControl/PLCController.h"
#include "piMotionActor/MoveLimitJX.h"
#include "piMotionActor/ConfigItem.h"

cv::Point2f Align::MvImage::center = cv::Point2f(1232, 1028);

Result AlignUtils::acsAbsoluteMove(const cv::Point3f &acsCoord)
{
    return MoveLimitJX::mcMoveAbs(acsCoord);
}

Result AlignUtils::acsAbsoluteMove(cv::Point3f &acsCoord, const CORE::CoordinateLimit &acsCoordLimit)
{
    Result ret = MoveLimitJX::mcMoveAbs(acsCoord);
    if (!ret.success)
    {
        return ret;
    }

    ML_Point3D point = Motion3DModel::getInstance(withCamera)->getPosition();
    acsCoord = cv::Point3f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION, point.z / UNIT_CONVERSION);
    return Result();
}

Result AlignUtils::acsAbsoluteMove(cv::Point3f &acsCoord, const QList<CoordinateLimit> acsCoordLimitList)
{
    Result ret = MoveLimitJX::mcMoveAbs(acsCoord);
    if (!ret.success)
    {
        return ret;
    }

    ML_Point3D point = Motion3DModel::getInstance(withCamera)->getPosition();
    acsCoord = cv::Point3f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION, point.z / UNIT_CONVERSION);
    return Result();
}

Result AlignUtils::acsRelativeMove(const cv::Point3f &acsCoord)
{
    ML_Point3D point = Motion3DModel::getInstance(withCamera)->getPosition();
    cv::Point3f current = cv::Point3f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION, point.z / UNIT_CONVERSION);
    current = current + acsCoord;
    return MoveLimitJX::mcMoveAbs(current);
}

Result AlignUtils::acsAbsoluteMoveWhenMoveOut(cv::Point3f acsCoord, const CORE::CoordinateLimit &limit)
{
    // TODO:Write if needed
    return Result();
}

Result AlignUtils::acsAbsoluteMoveWhenMoveOut(cv::Point3f acsCoord, const QList<CORE::CoordinateLimit> &limitList)
{
    //TODO:Write if needed
    return Result();
}

Result AlignUtils::acsRelativeMoveZ(double relativeZ, double min, double max)
{
    return MoveLimitJX::moveRel(cv::Point3f(0, 0, relativeZ), withCamera);
}

Result AlignUtils::getAcsCoordinate(cv::Point3f &acsCoord)
{
    return MoveLimitJX::getCoordinate3D(acsCoord, withCamera);
}

void AlignUtils::acsMoveStop(bool stop)
{
    if (stop)
    {
        Motion3DModel::getInstance(withCamera)->setEStop(stop);
        Motion3DModel::getInstance(withCamera)->Motion3DMoveStop();
    }
}

AlignBaseInfo AlignUtils::getAlignBaseInfo(int eyeType)
{
    AlignBaseInfo baseInfo;
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    CalibrationConfig::instance()->updateDutEyeboxs(dutConfig);

    dutMcSameDirection(dutConfig, CalibrationConfig::instance()->getMotion3DAndDutCoordRelate());

    baseInfo.inputGrating = dutConfig.inputGrating;
    for (cv::Point3f fiducial : dutConfig.fiducials)
    {
        baseInfo.fiducials.push_back(fiducial);
    }

    for (cv::Point3f eyebox : dutConfig.eyeboxs)
    {
        baseInfo.eyeboxs.push_back(eyebox);
    }

    baseInfo.sn = dutConfig.sn;
    baseInfo.acsFiducials = CalibrationConfig::instance()->getFiducialMotion3DCoords();
    baseInfo.i29RelativeMvCoord = CalibrationConfig::instance()->getMeasureRelPos();
    return baseInfo;
}

AlignBaseInfoOneFiducial AlignUtils::getAlignBaseInfoOneFiducial(int eyeType)
{
    AlignBaseInfo info = getAlignBaseInfo(eyeType);
    AlignBaseInfoOneFiducial baseInfo(info);
    baseInfo.measureCameraTiltAngle = CalibrationConfig::instance()->getMeasureCameraAngle();
    baseInfo.motion3DAndDutCoordRelateOfOneFid = CalibrationConfig::instance()->getMotion3DAndDutCoordRelate();
    return baseInfo;
}

AlignBaseInfoTilt AlignUtils::getAlignBaseInfoTilt(int eyeType)
{
    AlignBaseInfo info = getAlignBaseInfo(eyeType);
    AlignBaseInfoTilt baseInfo(info);
    baseInfo.measureCameraTiltAngle = CalibrationConfig::instance()->getMeasureCameraAngle();
    baseInfo.measureNearDutAcsZ = CalibrationConfig::instance()->getMeasureRelPos().z;
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    baseInfo.eyeRelief = dutConfig.eyeRelief;
    return baseInfo;
}

AlignBaseInfoOneFiducialTilt AlignUtils::getAlignBaseInfoOneFiducialTilt(int eyeType)
{
    AlignBaseInfo info = getAlignBaseInfo(eyeType);
    AlignBaseInfoOneFiducialTilt baseInfo(info);
    baseInfo.measureCameraTiltAngle = CalibrationConfig::instance()->getMeasureCameraAngle();
    baseInfo.motion3DAndDutCoordRelateOfOneFid = CalibrationConfig::instance()->getMotion3DAndDutCoordRelate();
    baseInfo.measureNearDutAcsZ = CalibrationConfig::instance()->getMeasureRelPos().z;
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    baseInfo.eyeRelief = dutConfig.eyeRelief;
    return baseInfo;
}

Result AlignUtils::getAlignBaseInfo3D(AlignBaseInfo3D &alignBaseInfo3D, int eyeType)
{
    AlignBaseInfo info = getAlignBaseInfo(eyeType);
    ML_Point angleXY;
    AlignBaseInfo3D baseInfo(info);
    baseInfo.mvFocus = CalibrationConfig::instance()->getMvFocus();

   //baseInfo.dutAngleXY = CalibrationConfig::instance()->getDutAngleXY();
    Result ret = getMotion2DAngle(ACS2DDUT, baseInfo.dutAngleXY);
    if (!ret.success)
    {
        return ret;
    }

    baseInfo.dutOutOffset = CalibrationConfig::instance()->getDutConfig().moveOutInfo.offset;
    baseInfo.motion3DAndDutCoordRelate = CalibrationConfig::instance()->getMotion3DAndDutCoordRelate();

    alignBaseInfo3D = baseInfo;
    return Result();
}

Result AlignUtils::getAlignBaseInfo2D(AlignBaseInfo2D& alignBaseInfo2D, int eyeType)
{
    AlignBaseInfo info = getAlignBaseInfo(eyeType);
    AlignBaseInfo2D baseInfo(info);

    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    baseInfo.mvAlignProjectorCoord = CalibrationConfig::instance()->getMvAlignProjectorCoord();
    baseInfo.mvRelativeDutInputGrating = dutConfig.mvRelativeDutInputGrating;
    baseInfo.motion3DAndDutCoordRelate = CalibrationConfig::instance()->getMotion3DAndDutCoordRelate();
    baseInfo.motion3DAndMVRelate = CalibrationConfig::instance()->getMotion3DAndMVCoordRelate();
    baseInfo.eyeRelief = dutConfig.eyeRelief;

    alignBaseInfo2D = baseInfo;
    return Result();
}

EyeReliefBaseInfo AlignUtils::getEyeReliefBaseInfo(int eyeType)
{
    EyeReliefBaseInfo baseInfo;

    baseInfo.dueDistance = CalibrationConfig::instance()->getDutConfig().eyeRelief;
    baseInfo.acsCoordinate = CalibrationConfig::instance()->getInputGratingForBaseZ();

    KeyenceInfo keyence = ConfigItem::instance()->getKeyenceInfo();
    baseInfo.keyenceRange = MinMax(keyence.keyenceRangeMin, keyence.keyenceRangeMax);
    baseInfo.keyenceZeroDistance = keyence.keyenceZeroPos;

    baseInfo.keyenceRelativeMvCoord = CalibrationConfig::instance()->getKeyenceRelPos();
    baseInfo.keyenceRelativeMvCoord.z = CalibrationConfig::instance()->getMVCameraRelPos().z;

    cv::Point3f measurePos = CalibrationConfig::instance()->getMeasureRelPos();
    baseInfo.i29RelativeMvCoordZ = baseInfo.keyenceRelativeMvCoord.z - measurePos.z;
    baseInfo.i29RelativeMvCoord = CalibrationConfig::instance()->getMeasureRelPos();
    return baseInfo;
}

std::vector<cv::Point3f> AlignUtils::acsMvRelMoveCoordByFidToEyeboxOfOneFid(
    const cv::Point3f mvAlignFid, const cv::Point3f &fiducial, const QList<cv::Point3f> &eyeboxs,
    QMap<QString, QString> motion3DAndDutCoordRelate)
{
    // QMap<QString, QString> motion3DAndDutCoordRelate = CalibrationConfig::instance()->getMotion3DAndDutCoordRelate();
    std::vector<cv::Point3f> list;
    for (cv::Point3f eyebox : eyeboxs)
    {
        cv::Point2f offset = cv::Point2f(eyebox.x - fiducial.x, eyebox.y - fiducial.y);
        list.push_back(cv::Point3f(mvAlignFid.x + offset.x, mvAlignFid.y + offset.y, mvAlignFid.z));
    }
    return list;
}

Result AlignUtils::measureCameraTiltOffset(cv::Point2f &tiltOffset, float eyeRelief, cv::Point2f angle)
{
    if (angle.x == 90 || angle.y == 90 || angle.x == 270 || angle.y == 270)
    {
        return Result(
            false,
            QString("Measure camera tilt angle error, angle is (%1,%2)").arg(angle.x).arg(angle.y).toStdString());
    }

    // TODO: eyeRelief is vertical align is eyeRelief/tan(angle), else eyeRelief*cos(angle)
    float offsetX = eyeRelief * tan(angle.x * 3.14 / 180.0);
    float offsetY = eyeRelief * tan(angle.y * 3.14 / 180.0);
    // float offsetX = eyeRelief * sin(angle.x * 3.14 / 180.0);
    // float offsety = eyeRelief * sin(angle.y * 3.14 / 180.0);
    tiltOffset = cv::Point2f(offsetX, offsetY);
    return Result();
}

cv::Point2f AlignUtils::relMoveCoordDutToAcs(cv::Point2f coord, const QMap<QString, QString> &motion3DAndDutCoordRelate)
{
    cv::Point2f newPos = coord;
    QString acsX = motion3DAndDutCoordRelate[CC_MOTION_3D_X];
    QString acsY = motion3DAndDutCoordRelate[CC_MOTION_3D_Y];
    if (acsX == CC_DUT_X)
    {
        newPos.x = coord.x;
    }
    else if (acsX == QString("-") + CC_DUT_X)
    {
        newPos.x = -coord.x;
    }
    else if (acsX == CC_DUT_Y)
    {
        newPos.x = coord.y;
    }
    else if (acsX == QString("-") + CC_DUT_Y)
    {
        newPos.x = -coord.y;
    }

    if (acsY == CC_DUT_X)
    {
        newPos.y = coord.x;
    }
    else if (acsY == QString("-") + CC_DUT_X)
    {
        newPos.y = -coord.x;
    }
    else if (acsY == CC_DUT_Y)
    {
        newPos.y = coord.y;
    }
    else if (acsY == QString("-") + CC_DUT_Y)
    {
        newPos.y = -coord.y;
    }

    return newPos;
}

cv::Point2f AlignUtils::relMoveCoordMvToAcs(cv::Point2f coord, const QMap<QString, QString>& motion3DAndMvCoordRelate)
{
    cv::Point2f newPos = coord;
    QString acsX = motion3DAndMvCoordRelate[CC_MOTION_3D_X];
    QString acsY = motion3DAndMvCoordRelate[CC_MOTION_3D_Y];
    if (acsX == CC_MV_X)
    {
        newPos.x = coord.x;
    }
    else if (acsX == QString("-") + CC_MV_X)
    {
        newPos.x = -coord.x;
    }
    else if (acsX == CC_MV_Y)
    {
        newPos.x = coord.y;
    }
    else if (acsX == QString("-") + CC_MV_Y)
    {
        newPos.x = -coord.y;
    }

    if (acsY == CC_MV_X)
    {
        newPos.y = coord.x;
    }
    else if (acsY == QString("-") + CC_MV_X)
    {
        newPos.y = -coord.x;
    }
    else if (acsY == CC_MV_Y)
    {
        newPos.y = coord.y;
    }
    else if (acsY == QString("-") + CC_MV_Y)
    {
        newPos.y = -coord.y;
    }

    return newPos;
}

CORE::FiducialType AlignUtils::analyzeFiducialType(const QString &typeStr)
{
    CORE::FiducialType fiducialType = CORE::FiducialType::CIRCLE;
    if (typeStr.compare("Circle", Qt::CaseInsensitive) == 0)
    {
        fiducialType = CORE::FiducialType::CIRCLE;
    }
    else if (typeStr.compare("Crosshair", Qt::CaseInsensitive) == 0)
    {
        fiducialType = CORE::FiducialType::CROSSHAIR;
    }
    else if (typeStr.compare("BMW", Qt::CaseInsensitive) == 0)
    {
        fiducialType = CORE::FiducialType::BMW;
    }
    else if (typeStr.compare("Rectangle", Qt::CaseInsensitive) == 0)
    {
        fiducialType = CORE::FiducialType::RECTANGLE;
    }
    else if (typeStr.compare("PICO", Qt::CaseInsensitive) == 0)
    {
        fiducialType = CORE::FiducialType::PICO;
    }
    return fiducialType;
}

Result AlignUtils::fiducialLight(bool isOpen, bool isCoaxialLight)
{
    Result result;
    result = PLCController::instance()->coaxialLight(false);
    if (!result.success)
    {
        return Result(false, "Close coaxial light failed.");
    }

    if (isOpen)
    {
        if (isCoaxialLight)
        {
            result = PLCController::instance()->lightBoard(!isOpen);
            if (!result.success)
            {
                QString msg = !isOpen ? "Open light board light board source failed."
                                      : "Close light board light board source failed.";
                return Result(false, msg.toStdString());
            }

            result = PLCController::instance()->coaxialLight2(isOpen);
        }
        else
        {
            result = PLCController::instance()->coaxialLight2(!isOpen);
            if (!result.success)
            {
                QString msg = !isOpen ? "Open coaxial light source failed." 
                                      : "Close coaxial light source failed.";
                return Result(false, msg.toStdString());
            }

            result = PLCController::instance()->lightBoard(isOpen);
        }
    }
    else
    {
        result = PLCController::instance()->lightBoard(isOpen);
        if (!result.success)
        {
            QString msg = !isOpen ? "Open light board light board source failed."
                                  : "Close light board light board source failed.";
            return Result(false, msg.toStdString());
        }
        result = PLCController::instance()->coaxialLight2(isOpen);
    }
    
    if (!result.success)
    {
        QString msg = isOpen ? "Open fiducial light source failed.":"Close fiducial light source failed.";
        return Result(false, msg.toStdString());
    }
    return result;
}

Result AlignUtils::fiducialLight(bool isOpen)
{
    return PLCController::instance()->coaxialLight2(isOpen);
}

std::vector<cv::Point3f> AlignUtils::calculateEyeboxs(const cv::Point3f &center, float offset)
{
    std::vector<cv::Point3f> eyeboxs;
    eyeboxs.push_back(center);
    eyeboxs.push_back(cv::Point3f(center.x, center.y + offset, center.z));
    eyeboxs.push_back(cv::Point3f(center.x, center.y - offset, center.z));
    eyeboxs.push_back(cv::Point3f(center.x - offset, center.y, center.z));
    eyeboxs.push_back(cv::Point3f(center.x + offset, center.y, center.z));
    return eyeboxs;
}

std::vector<cv::Point3f> AlignUtils::calculateEyeboxs(const cv::Point3f &center, QList<float> offsets)
{
    std::vector<cv::Point3f> eyeboxs;
    if (offsets.size() == 0){
        eyeboxs.push_back(center);
    }
    else if (offsets.size() == 1){
        eyeboxs = calculateEyeboxs(center, offsets.at(0));
    }
    else
    {
        eyeboxs.push_back(center);
        eyeboxs.push_back(cv::Point3f(center.x, center.y + offsets[0], center.z));
        eyeboxs.push_back(cv::Point3f(center.x, center.y - offsets[1], center.z));
        eyeboxs.push_back(cv::Point3f(center.x - offsets[2], center.y, center.z));
        eyeboxs.push_back(cv::Point3f(center.x + offsets[3], center.y, center.z));
    }
    return eyeboxs;
}

std::vector<cv::Point3f> AlignUtils::calculateEyeboxs(const cv::Point3f &center, QList<cv::Point3f> offsets)
{
    std::vector<cv::Point3f> eyeboxs;
    for (cv::Point3f offset: offsets){
        eyeboxs.push_back(center + offset);
    }

    if (eyeboxs.size() == 0)
    {
        eyeboxs.push_back(center);
    }
    return eyeboxs;
}

std::vector<cv::Point3f> AlignUtils::mvToMeasureCameraCoordXY(const std::vector<cv::Point3f> &mvCoords, cv::Point3f i29RelativeMvCoord)
{
    std::vector<cv::Point3f> list;
    for (cv::Point3f mvCoord : mvCoords)
    {
        list.push_back(cv::Point3f(mvCoord.x + i29RelativeMvCoord.x,  mvCoord.y + i29RelativeMvCoord.y, mvCoord.z));
    }

    return list;
}

void AlignUtils::dutMcSameDirection(DutConfig &dutconfig)
{
    dutMcSameDirection(dutconfig, CalibrationConfig::instance()->getMotion3DAndDutCoordRelate());
}

Result AlignUtils::getMotion2DAngle(int type, cv::Point2f &angle)
{
    Result ret = Motion2DModel::getInstance(ACS2DDUT)->isHoming();
    if (!ret.success)
    {
        return ret;
    }
    CORE::ML_Point point = Motion2DModel::getInstance(ACS2DDUT)->getPosition();
    angle = cv::Point2f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION);
    return Result();
}

Result AlignUtils::eyeboxIndexFromName(const QString &eyeboxName, EYEBOX_INDEX &index)
{
    index = EYEBOX_INDEX_CENTER;
    if (eyeboxName.toLower() == "center"){
        index = EYEBOX_INDEX_CENTER;
    }
    else if (eyeboxName.toLower() == "up")
    {
        index = EYEBOX_INDEX_UP;
    }
    else if (eyeboxName.toLower() == "down")
    {
        index = EYEBOX_INDEX_DOWN;
    }
    else if (eyeboxName.toLower() == "left")
    {
        index = EYEBOX_INDEX_LEFT;
    }
    else if (eyeboxName.toLower() == "right")
    {
        index = EYEBOX_INDEX_RIGHT;
    }else{
        return Result(false,
                      QString("Eyebox name convert to index error, Eyebox name is \"%1\".").arg(eyeboxName).toStdString());
    }
    return Result();
}

Result AlignUtils::getDutPosition(cv::Point3f &coordinate)
{
    CORE::ML_Point3D currentCoord = Motion3DModel::getInstance(withDUT)->getPosition();
    if (currentCoord.x == 0 && currentCoord.y == 0 && currentCoord.z){
        return Result(false, "Get dut position failed.");
    }

    coordinate.x = (float)currentCoord.x / UNIT_CONVERSION;
    coordinate.y = (float)currentCoord.y / UNIT_CONVERSION;
    coordinate.z = (float)currentCoord.z / UNIT_CONVERSION;
    return Result();
}

void AlignUtils::dutMcSameDirection(DutConfig &dutconfig, const QMap<QString, QString> &motion3DAndDutCoordRelate)
{
    //TODO:
    return;

    for (cv::Point3f &point : dutconfig.fiducials)
    {
        cv::Point2f tmp = relMoveCoordDutToAcs(cv::Point2f(point.x, point.y), motion3DAndDutCoordRelate);
        point = cv::Point3f(tmp.x, tmp.y, point.z);
    }

    for (cv::Point3f &point : dutconfig.eyeboxs)
    {
        cv::Point2f tmp = relMoveCoordDutToAcs(cv::Point2f(point.x, point.y), motion3DAndDutCoordRelate);
        point = cv::Point3f(tmp.x, tmp.y, point.z);
    }

    {
        cv::Point3f &point = dutconfig.eyeboxCenter;
        cv::Point2f tmp = relMoveCoordDutToAcs(cv::Point2f(point.x, point.y), motion3DAndDutCoordRelate);
        point = cv::Point3f(tmp.x, tmp.y, point.z);
    }

    {
        cv::Point3f &point = dutconfig.inputGrating;
        cv::Point2f tmp = relMoveCoordDutToAcs(cv::Point2f(point.x, point.y), motion3DAndDutCoordRelate);
        point = cv::Point3f(tmp.x, tmp.y, point.z);
    }
}

//Result AlignUtils::motionAbsMove(Motion3DModel *model, cv::Point3f &acsCoord, const CORE::CoordinateLimit &limit)
//{
//    Result result = MoveLimitJX::judgeMeasureCamera(acsCoord);
//    if (!result.success)
//    {
//        return result;
//    }
//
//    ML_Point3D point = model->getPosition();
//    float dutTopZ = CalibrationConfig::instance()->getMeasureRelPos().z;
//    if (point.z > dutTopZ)
//    {
//        result = model->Motion3DMoveAbs(point.x, point.y, (dutTopZ - 20) * UNIT_CONVERSION);
//        if (!result.success)
//        {
//            return result;
//        }
//    }
//
//    result = model->Motion3DMoveAbs(acsCoord.x * UNIT_CONVERSION, acsCoord.y * UNIT_CONVERSION,
//                                    (dutTopZ - 20) * UNIT_CONVERSION);
//    if (!result.success)
//    {
//        return result;
//    }
//
//    if (qFuzzyCompare(acsCoord.z, 0))
//    {
//        acsCoord.z = point.z / UNIT_CONVERSION;
//    }
//
//    result = model->Motion3DMoveAbs(acsCoord.x * UNIT_CONVERSION, acsCoord.y * UNIT_CONVERSION,
//                                    acsCoord.z * UNIT_CONVERSION);
//    return result;
//}

//Result AlignUtils::motionAbsMove(Motion3DModel *model, cv::Point3f &acsCoord, const QList<CORE::CoordinateLimit> &limitList)
//{
//    CORE::CoordinateLimit limit;
//    calMoveLimit(acsCoord, limitList, limit);
//    return motionAbsMove(model, acsCoord, limit);
//}

void AlignUtils::relMoveAcsWhenMoveOut(cv::Point3f &acsCoord)
{
    // TODO:Write if needed
    return;
}

Result AlignUtils::dutMoveOutInputgratingOffset(bool isOut, const cv::Point2f &offset, const CORE::CoordinateLimit &limit)
{
    // TODO: to be completed
    cv::Point3f currentCoord;
    Result ret = getDutPosition(currentCoord);
    if (!ret.success)
    {
        return ret;
    }

    if (isOut)
    {
        currentCoord.x = currentCoord.x + offset.x;
        currentCoord.y = currentCoord.y + offset.y;
    }
    else
    {
        currentCoord.x = currentCoord.x - offset.x;
        currentCoord.y = currentCoord.y - offset.y;
    }

    return MoveLimitJX::dutMoveAbs(cv::Point2f(currentCoord.x, currentCoord.y));
}

Result AlignUtils::dutMoveOutInputgratingOffset(bool isOut)
{
    //TODO: to be done
    cv::Point2f offset = CalibrationConfig::instance()->getDutConfig().moveOutInfo.offset;
    return dutMoveOutInputgratingOffset(isOut, offset, CORE::CoordinateLimit());
}

std::vector<cv::Point3f> AlignUtils::eyeboxAddDutOffset(const std::vector<cv::Point3f> &eyeboxs, const cv::Point2f &offset)
{
    std::vector<cv::Point3f> list;
    for (cv::Point3f eyebox : eyeboxs)
    {
        list.push_back(cv::Point3f(eyebox.x - offset.x, eyebox.y - offset.y, eyebox.z));
    }
    return list;
}

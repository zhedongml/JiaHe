#include "MoveLimit.h"
#include <QMutex>
#include <QThread>
#include "Motion2DModel.h"
#include "DutProjectorCalibrate.h"
#include "Core/loggingwrapper.h"

MoveLimit *MoveLimit::m_instance = nullptr;

MoveLimit *MoveLimit::instance()
{
    if (!m_instance)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance)
        {
            m_instance = new MoveLimit();
        }
    }
    return m_instance;
}

MoveLimit::MoveLimit()
{
}

Result MoveLimit::dutMoveAbsNoCollision(const cv::Point3f &coordinate)
{
    if (!Motion3DModel::getInstance(withDUT)->Motion3DisConnected())
    {
        return Result(false, "DUT 3D motion is not connected, DUT abs move failed.");
    }

    cv::Point3f acsCoord = coordinate;
    Result ret = dutPreJudge(acsCoord);
    if (!ret.success)
    {
        return ret;
    }

    return dutMoveOrder(acsCoord);  
}

Result MoveLimit::mcMoveAbsNoCollision(const cv::Point3f &coordinate)
{
    if (!Motion3DModel::getInstance(withCamera)->Motion3DisConnected())
    {
        return Result(false, "Measure camera 3D motion is not connected, Measure camera abs move failed.");
    }

    cv::Point3f acsCoord = coordinate;
    ML_Point3D point = Motion3DModel::getInstance(withCamera)->getPosition();

    Result result = mcPreJudge(coordinate);
    if (!result.success)
    {
        return result;
    }

    return mcMoveOrder(acsCoord);
}

Result MoveLimit::dutMoveOrder(const cv::Point3f &coordinate)
{
    ML_Point3D point = Motion3DModel::getInstance(withDUT)->getPosition();
    cv::Point3f newCoord = coordinate * UNIT_CONVERSION;

    Result ret;
    if (newCoord.z > point.z)
    {
        ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveAbs(point.x, point.y, newCoord.z);
        if (!ret.success)
        {
            return ret;
        }

        ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveAbs(point.x, newCoord.y, newCoord.z);
        if (!ret.success)
        {
            return ret;
        }

        ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveAbs(newCoord.x, newCoord.y, newCoord.z);
    }
    else
    {
        ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveAbs(point.x, newCoord.y, point.z);
        if (!ret.success)
        {
            return ret;
        }

        ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveAbs(newCoord.x, newCoord.y, point.z);
        if (!ret.success)
        {
            return ret;
        }

        ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveAbs(newCoord.x, newCoord.y, newCoord.z);
    }

    return ret;
}

Result MoveLimit::mcMoveOrder(const cv::Point3f &coordinate)
{
    ML_Point3D point = Motion3DModel::getInstance(withCamera)->getPosition();
    cv::Point3f newCoord = coordinate * UNIT_CONVERSION;

    Result ret;
    if (newCoord.z < point.z)
    {
        ret = Motion3DModel::getInstance(withCamera)->Motion3DMoveAbs(point.x, point.y, newCoord.z);
        if (!ret.success)
        {
            return ret;
        }

        ret = Motion3DModel::getInstance(withCamera)->Motion3DMoveAbs(newCoord.x, point.y, newCoord.z);
        if (!ret.success)
        {
            return ret;
        }

        ret = Motion3DModel::getInstance(withCamera)->Motion3DMoveAbs(newCoord.x, newCoord.y, newCoord.z);
    }
    else
    {
        ret = Motion3DModel::getInstance(withCamera)->Motion3DMoveAbs(newCoord.x, point.y, point.z);
        if (!ret.success)
        {
            return ret;
        }

        ret = Motion3DModel::getInstance(withCamera)->Motion3DMoveAbs(newCoord.x, newCoord.y, point.z);
        if (!ret.success)
        {
            return ret;
        }

        ret = Motion3DModel::getInstance(withCamera)->Motion3DMoveAbs(newCoord.x, newCoord.y, newCoord.z);
    }
    return ret;
}

MoveLimit::~MoveLimit()
{
}

Result MoveLimit::setInputAlignState(bool isAlign)
{
    for (AlignCalibrateCallback *pCall : m_alignCalibrateCallbacks)
    {
        if (pCall != nullptr)
        {
            pCall->notifyInputAAStateChanged(isAlign);
        }
    }

    if (!isAlign){
        m_inputAlignState.isAlign = false;
        return Result();
    }

    Result ret = MoveLimit::getCoordinate(withDUT, m_inputAlignState.alignCoord);
    if (!ret.success)
    {
        m_inputAlignState.isAlign = false;
        return Result(false, "Set input align state error," + ret.errorMsg);
    }

    ret = getCoordinate2DTilt(ACS2DDUT, m_inputAlignState.dutAngle);
    if (!ret.success)
    {
        return Result(false, "Set input align state error," + ret.errorMsg);
    }

    ret =  getCoordinate2DTilt(ACS2DPro, m_inputAlignState.prAngle);
    if (!ret.success)
    {
        return Result(false, "Set input align state error," + ret.errorMsg);
    }

    m_inputAlignState.isAlign = true;
    m_inputAlignState.dutSN = CalibrationConfig::instance()->getDutConfig().sn;
    m_inputAlignState.eyeType = CalibrationConfig::instance()->getDutConfig().eyeType;
    return Result();
}

bool MoveLimit::completeInputAA()
{
    if (!m_inputAlignState.isAlign){
        return false;
    }

    Result ret = isDutChange(m_inputAlignState);
    if(!ret.success){
        LoggingWrapper::instance()->warn(QString::fromStdString(ret.errorMsg));
        return false;
    }

    cv::Point3f dutCurCoord;
    ret = MoveLimit::getCoordinate(withDUT, dutCurCoord);
    if(!ret.success){
        return false;
    }

    const cv::Point3f aaCoord = m_inputAlignState.alignCoord;
    if (abs(dutCurCoord.x - aaCoord.x) > 0.05 || abs(dutCurCoord.y - aaCoord.y) > 0.05 ||
        abs(dutCurCoord.z - aaCoord.z) > 0.05)
    {
        LoggingWrapper::instance()->info(
            QString("Input AA align coord offset > 0.05, alignCoord=(%1,%2,%3), currentCoord=(%4,%5,%6)").
            arg(aaCoord.x).arg(aaCoord.y).arg(aaCoord.z).
            arg(dutCurCoord.x).arg(dutCurCoord.y).arg(dutCurCoord.z));
    }

    if (abs(dutCurCoord.x - aaCoord.x) > 0.05 || 
        abs(dutCurCoord.y - aaCoord.y) > 0.05 || 
        abs(dutCurCoord.z - aaCoord.z) > 0.05)
    {
        return false;
    }

    return true;
}

Result MoveLimit::resetLastInputAA()
{
    if (!m_inputAlignState.isAlign)
    {
        return Result(false, "Please first auto or manual align input grating.");
    }

    Result ret = isDutChange(m_inputAlignState);
    if (!ret.success)
    {
        return Result(false, "Please first auto or manual align input grating, " + ret.errorMsg);
    }

    ret = moveBeforeInputAA();
    if (!ret.success)
    {
        return Result(false, QString("Reset last inputAA failed, %1.").arg(QString::fromStdString(ret.errorMsg)).toStdString());
    }

    return dutMoveAbs(m_inputAlignState.alignCoord);
}

Result MoveLimit::setMotion3DMoveAlignSpeed(bool flag)
{
    //TODO: to be test
    return Result();

    if (Motion3DModel::getInstance(withCamera)->Motion3DisConnected())
    {
        return Result(false, "Set Motion3D speed error, Measure camera motion3D is not connected.");
    }
    if (Motion3DModel::getInstance(withDUT)->Motion3DisConnected())
    {
        return Result(false, "Set Motion3D speed error, DUT motion3D is not connected.");
    }

    if (flag)
    {
        int cameraSpeed = Motion3DModel::getInstance(withCamera)->GetSpeed();
        m_motion3DSpeedMap[withCamera].x = cameraSpeed;
        m_motion3DSpeedMap[withCamera].y = cameraSpeed;
        m_motion3DSpeedMap[withCamera].z = cameraSpeed;

        int dutSpeed = Motion3DModel::getInstance(withDUT)->GetSpeed();
        m_motion3DSpeedMap[withDUT].x = dutSpeed;
        m_motion3DSpeedMap[withDUT].y = dutSpeed;
        m_motion3DSpeedMap[withDUT].z = dutSpeed;

        cv::Point3f alignSpeed = MoveLimitConfig::instance()->getMotion3DMoveAlignSpeed();
        Motion3DModel::getInstance(withCamera)->SetSpeedX(alignSpeed.x);
        Motion3DModel::getInstance(withCamera)->SetSpeedY(alignSpeed.y);
        Motion3DModel::getInstance(withCamera)->SetSpeedZ(alignSpeed.z);

        Motion3DModel::getInstance(withDUT)->SetSpeedX(alignSpeed.x);
        Motion3DModel::getInstance(withDUT)->SetSpeedY(alignSpeed.y);
        Motion3DModel::getInstance(withDUT)->SetSpeedZ(alignSpeed.z);
    }
    else
    {
        if (m_motion3DSpeedMap.contains(withCamera))
        {
            cv::Point3i speed = m_motion3DSpeedMap[withCamera];
            Motion3DModel::getInstance(withCamera)->SetSpeedX(speed.x);
            Motion3DModel::getInstance(withCamera)->SetSpeedY(speed.y);
            Motion3DModel::getInstance(withCamera)->SetSpeedZ(speed.z);
        }

        if (m_motion3DSpeedMap.contains(withDUT))
        {
            cv::Point3i speed = m_motion3DSpeedMap[withDUT];
            Motion3DModel::getInstance(withDUT)->SetSpeedX(speed.x);
            Motion3DModel::getInstance(withDUT)->SetSpeedY(speed.y);
            Motion3DModel::getInstance(withDUT)->SetSpeedZ(speed.z);
        }
    }
    return Result();
}

void MoveLimit::subscribe(AlignCalibrateCallback *callback)
{
    if (!m_alignCalibrateCallbacks.contains(callback))
    {
        m_alignCalibrateCallbacks.push_back(callback);
    }
}

void MoveLimit::unsubscribe(AlignCalibrateCallback *callback)
{
    if (m_alignCalibrateCallbacks.contains(callback))
    {
        m_alignCalibrateCallbacks.removeOne(callback);
    }
}

Result MoveLimit::moveAbs(const cv::Point3f &coordinate, motion3DType type)
{
    if (type == withDUT){
        return dutMoveAbs(coordinate);
    }else{
        return mcMoveAbs(coordinate);
    }
}

Result MoveLimit::moveRel(const cv::Point3f &coord, motion3DType type)
{
    ML_Point3D point = Motion3DModel::getInstance(type)->getPosition();
    cv::Point3f current = cv::Point3f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION, point.z / UNIT_CONVERSION);
    current = current + coord;

    if (type == withDUT)
    {
        return dutMoveAbs(current);
    }
    else
    {
        return mcMoveAbs(current);
    }
}

Result MoveLimit::moveAbsNoCollision(const cv::Point3f &coordinate, motion3DType type)
{
    if (type == withDUT)
    {
        return dutMoveAbsNoCollision(coordinate);
    }
    else
    {
        return mcMoveAbsNoCollision(coordinate);
    }
}

Result MoveLimit::moveRelNoCollision(const cv::Point3f &coord, motion3DType type)
{
    ML_Point3D point = Motion3DModel::getInstance(type)->getPosition();
    cv::Point3f current = cv::Point3f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION, point.z / UNIT_CONVERSION);
    current = current + coord;

    if (type == withDUT)
    {
        return dutMoveAbsNoCollision(current);
    }
    else
    {
        return mcMoveAbsNoCollision(current);
    }
}

Result MoveLimit::moveAbsToSafe(motion3DType type)
{
    Result ret;
    cv::Point3f pos;
    if (withDUT == type)
    {
         pos = MoveLimitConfig::instance()->getLoadDutPosition();
         ret = dutMoveToSafeBeforeMoveUp(pos);
         if (!ret.success)
         {
             return ret;
         }
    }
    else
    {
        pos = MoveLimitConfig::instance()->getLoadMeasureCameraPosition();
    }

    ret = moveAbsNoCollision(pos, type);
    return ret;
}

Result MoveLimit::dutMoveAbs(const cv::Point3f &coordinate)
{
    if(!Motion3DModel::getInstance(withDUT)->Motion3DisConnected()){
        return Result(false, "DUT 3D motion is not connected, DUT abs move failed.");
    }

    cv::Point3f acsCoord = coordinate;
    Result ret = MoveLimit::judgeDut(acsCoord);
    if (!ret.success)
    {
        return ret;
    }

    //ML_Point3D point = Motion3DModel::getInstance(withDUT)->getPosition();
    //if (qFuzzyCompare(acsCoord.z, 0))
    //{
    //    acsCoord.z = point.z / UNIT_CONVERSION;
    //}

    ret = dutProjectorCollisionMoveUp(coordinate);
    if (!ret.success)
    {
        return ret;
    }

    return dutMoveOrder(acsCoord);
}

Result MoveLimit::mcMoveAbs(const cv::Point3f &coordinate)
{
    if (!Motion3DModel::getInstance(withCamera)->Motion3DisConnected())
    {
        return Result(false, "Measure camera 3D motion is not connected, Measure camera abs move failed.");
    }

    cv::Point3f acsCoord = coordinate;
    //ML_Point3D point = Motion3DModel::getInstance(withCamera)->getPosition();
    //if (qFuzzyCompare(acsCoord.z, 0))
    //{
    //    acsCoord.z = point.z / UNIT_CONVERSION;
    //}

    Result result = MoveLimit::judgeMeasureCamera(acsCoord);
    if (!result.success)
    {
        return result;
    }

    return mcMoveOrder(acsCoord);
}

Result MoveLimit::judgeDut(const cv::Point3f &coordinate)
{
    Result ret = dutPreJudge(coordinate);
    if(!ret.success){
        return ret;
    }

    ret = dutProjectorCollision(coordinate);
    if (!ret.success)
    {
        return Result(false,
                      QString("Dut move failed, dut and projector exist collision, %1 dut target coordinate is %2.")
                          .arg(QString::fromStdString(ret.errorMsg))
                          .arg(coordToString(coordinate))
                          .toStdString());
    }

    cv::Point3f mcCoord;
    ret = getCoordinate(withDUT, mcCoord);
    if (!ret.success)
    {
        return ret;
    }

    ret = dutMcCollision(coordinate, mcCoord);
    if (!ret.success)
    {
        return Result(false, QString("Dut move failed, dut and measure exist collision, %1 dut target coordinate is %2.")
            .arg(QString::fromStdString(ret.errorMsg))
            .arg(coordToString(coordinate))
            .toStdString());
    }
    return Result();
}

Result MoveLimit::judgeMeasureCamera(const cv::Point3f &coordinate)
{
    Result ret = mcPreJudge(coordinate);
    if (!ret.success)
    {
        return ret;
    }

    ret = judgeEyeboxLimit(coordinate);
    if (!ret.success)
    {
        return ret;
    }

    cv::Point3f dutCoord;
    ret = getCoordinate(withDUT, dutCoord);
    if (!ret.success)
    {
        return ret;
    }

    //ret = dutMcCollision(dutCoord, coordinate);
    //if (!ret.success)
    //{
    //    return Result(false, QString("Measure camera move failed, %1 Measure camera target coordinate is %2.")
    //                             .arg(QString::fromStdString(ret.errorMsg))
    //                             .arg(coordToString(coordinate))
    //                             .toStdString());
    //}

    return Result();
}

Result MoveLimit::moveInit()
{
    Result ret = moveAbsToSafe(withCamera);
    if (!ret.success)
    {
        return Result(false, "Move Measure camera to init position error, " + ret.errorMsg);
    }

    ret = moveAbsToSafe(withDUT);
    if(!ret.success){
        return Result(false, "Move DUT to init position error, " + ret.errorMsg);
    }

    return Result();
}

Result MoveLimit::dutMcCollision(const cv::Point3f &dutCoord, const cv::Point3f &mcCoord)
{
    DutMcCollision cosInfo = MoveLimitConfig::instance()->getDutMcCollision();
    float minX = cosInfo.centerRelative.x - cosInfo.rectangle.x / 2.0;
    float maxX = cosInfo.centerRelative.x + cosInfo.rectangle.x / 2.0;
    float minY = cosInfo.centerRelative.y - cosInfo.rectangle.y / 2.0;
    float maxY = cosInfo.centerRelative.y + cosInfo.rectangle.y / 2.0;
    float minZ = cosInfo.centerRelative.z + cosInfo.minDistanceZ;

    float disX = dutCoord.x - mcCoord.x;
    float disY = dutCoord.y - mcCoord.y;
    float disZ = dutCoord.z - mcCoord.z;

    if (disX > minX && disX < maxX && disY > minY && disY < maxY){
        if (disZ < minZ){
            return Result(false, "DUT and Measure camera exist DUT MC Collision danger.");
        }
    }
    return Result();
}

Result MoveLimit::dutProjectorCollision(const cv::Point3f &dutCoord)
{
    DUT_EYE_TYPE eyeType = (DUT_EYE_TYPE)CalibrationConfig::instance()->getDutConfig().eyeType;
    cv::Point3f dutPrNearCoordinate;
    Result ret = DutProjectorCalibrate::instance()->getDutPrNearCoordinate(eyeType, dutPrNearCoordinate);
    if (!ret.success)
    {
        return ret;
    }

    DutProjectorCollision cosInfo = MoveLimitConfig::instance()->getDutProjectorCollision();
    float minX = dutPrNearCoordinate.x - cosInfo.rectangle.x / 2.0;
    float maxX = dutPrNearCoordinate.x + cosInfo.rectangle.x / 2.0;
    float minY = dutPrNearCoordinate.y - cosInfo.rectangle.y / 2.0;
    float maxY = dutPrNearCoordinate.y + cosInfo.rectangle.y / 2.0;
    float minZ = dutPrNearCoordinate.z + cosInfo.minDistanceZ;

    float disX = dutCoord.x;
    float disY = dutCoord.y;
    float disZ = dutCoord.z;

    //if (disX > minX && disX < maxX && disY > minY && disY < maxY)
    if (disX < maxX && disY < maxY)
    {
        if (disZ < minZ)
        {
            return Result(false, "DUT and projector exist Collision danger.");
        }
    }
    return Result();
}

Result MoveLimit::dutProjectorCollisionMoveUp(const cv::Point3f &destCoord)
{
    cv::Point3f dutCurrentCoord;
    Result ret = getCoordinate(withDUT, dutCurrentCoord);
    if (!ret.success)
    {
        return ret;
    }

    DUT_EYE_TYPE eyeType = (DUT_EYE_TYPE)CalibrationConfig::instance()->getDutConfig().eyeType;
    cv::Point3f dutPrNearCoordinate;
    ret = DutProjectorCalibrate::instance()->getDutPrNearCoordinate(eyeType, dutPrNearCoordinate);
    if (!ret.success)
    {
        return ret;
    }

    float offsetZ = 0;
    ret = dutProjectorCollisionMoveUpCal(offsetZ, dutPrNearCoordinate, dutCurrentCoord, destCoord);
    if (!ret.success)
    {
        return ret;
    }

    if (offsetZ > 0.5)
    {
        ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveRel(0, 0, offsetZ * UNIT_CONVERSION);
        if (!ret.success)
        {
            return ret;
        }
    }
    return Result();
}

Result MoveLimit::getCoordinate(motion3DType motionType, cv::Point3f &coord)
{
    std::string typeStr = motionType == withDUT ? "DUT "  :"Measure camera ";
    if (!Motion3DModel::getInstance(motionType)->Motion3DisConnected())
    {
        return Result(false, typeStr + "3D motion is not connected, get coordinate failed.");
    }

    CORE::ML_Point3D pos = Motion3DModel::getInstance(motionType)->getPosition();
    coord.x = pos.x / UNIT_CONVERSION;
    coord.y = pos.y / UNIT_CONVERSION;
    coord.z = pos.z / UNIT_CONVERSION;
    return Result();
}

Result MoveLimit::moveAbs2DTilt(const cv::Point2f &coordinate, motion2DType type)
{
    cv::Point2f currentPos;
    Result ret = getCoordinate2DTilt(type, currentPos);
    if(!ret.success){
        return ret;
    }

    if (abs(currentPos.x - coordinate.x) > 0.1 || abs(currentPos.y - coordinate.y) > 0.1)
    {
        // Judge Measure camera is or not safe position
        cv::Point3f prInitCoord = MoveLimitConfig::instance()->getLoadMeasureCameraPosition();
        cv::Point3f prCoord;
        ret = getCoordinate(withCamera, prCoord);
        if (!ret.success)
        {
            return ret;
        }

        if (abs(prInitCoord.x - prCoord.x) > 1 || abs(prInitCoord.y - prCoord.y) > 1)
        {
            return Result(false, "Motion 2D set tilt failed, Measure camera motion 3D is not safe position.");
        }

        // Judge DUT is or not safe position
        cv::Point3f dutInitCoord = MoveLimitConfig::instance()->getLoadDutPosition();
        cv::Point3f dutCoord;
        ret = getCoordinate(withDUT, dutCoord);
        if (!ret.success)
        {
            return ret;
        }

        if (abs(dutInitCoord.x - dutCoord.x) > 1 || abs(dutInitCoord.y - dutCoord.y) > 1)
        {
            return Result(false, "Motion 2D set tilt failed, DUT motion 3D is not safe position.");
        }
    }

    ret = Motion2DModel::getInstance(type)->Motion2DMoveAbs(coordinate.x, coordinate.y);
    return ret;
}

Result MoveLimit::moveBeforeInputAA()
{
    cv::Point3f mcCoord = MoveLimitConfig::instance()->getLoadMeasureCameraPosition();

    cv::Point3f mcCoordCurrent;
    Result ret = getCoordinate(withCamera, mcCoordCurrent);
    if (!ret.success)
    {
        return Result(false, "Move Measure camera to init position Z error, " + ret.errorMsg);
    }

    if (abs(mcCoord.x - mcCoordCurrent.x) < 1 && 
        abs(mcCoord.y - mcCoordCurrent.y) < 1 &&
        abs(mcCoord.z - mcCoordCurrent.z) < 1)
    {
        return Result();
    }
    else
    {
        cv::Point3f target = mcCoordCurrent;
        if (target.z > mcCoord.z)
        {
            target.z = mcCoord.z;
            return mcMoveAbs(target);
        }
        return Result();
    }
}

Result MoveLimit::judgeEyeboxLimit(const std::vector<cv::Point3f> &eyeboxs)
{
    Result ret;
    for (int i = 0; i < eyeboxs.size(); ++i){
        ret = judgeEyeboxLimit(eyeboxs[i], i + 1);
        if(!ret.success){
            return ret;
        }
    }
    return Result();
}

Result MoveLimit::dutPreJudge(const cv::Point3f &coord)
{
    CoordinateLimit limit = MoveLimitConfig::instance()->getDutLimit();
    return preJudge(coord, limit);
}

Result MoveLimit::mcPreJudge(const cv::Point3f &coord)
{
    CoordinateLimit limit = MoveLimitConfig::instance()->getMeasureCameraLimit();
    return preJudge(coord, limit);
}

Result MoveLimit::preJudge(const cv::Point3f &coord, const CoordinateLimit &limit)
{
    if (coord.x < limit.min.x || coord.y < limit.min.y || coord.z < limit.min.z)
    {
        return Result(false, QString("Motion3D move coordinate is over min limit, target is %1, min is %2")
                                 .arg(coordToString(coord))
                                 .arg(coordToString(limit.min))
                                 .toStdString());
    }

    if (coord.x > limit.max.x || coord.y > limit.max.y || coord.z > limit.max.z)
    {
        return Result(false, QString("Motion3D move coordinate is over max limit, target is %1, max is %2")
                                 .arg(coordToString(coord))
                                 .arg(coordToString(limit.max))
                                 .toStdString());
    }
    return Result();
}

QString MoveLimit::coordToString(const cv::Point3f &coord)
{
    return QString("(%1,%2,%3)").arg(coord.x).arg(coord.y).arg(coord.z);
}

Result MoveLimit::getCoordinate2DTilt(motion2DType motionType, cv::Point2f &coord)
{
    QString str = "DUT";
    if (ACS2DPro == motionType)
    {
        str = "Projector";
    }
    else if (ACS2DReticle == motionType)
    {
        str = "Reticle";
    }

    if (!Motion2DModel::getInstance(motionType)->GetIsConnected()){
        return Result(false, QString("%1 Motion 2D tilt is not connected.").arg(str).toStdString());
    }

    CORE::ML_Point point = Motion2DModel::getInstance(motionType)->getPosition();
    coord.x = point.x / UNIT_CONVERSION;
    coord.y = point.y / UNIT_CONVERSION;
    return Result();
}

Result MoveLimit::isDutChange(const InputAlignState &inputAlignState)
{
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    if (inputAlignState.dutSN != dutConfig.sn)
    {
        return Result(false, QString("DUT is change, now sn=%1.").arg(dutConfig.sn).toStdString());
    }

    if (inputAlignState.eyeType != dutConfig.eyeType)
    {
        return Result(false, QString("DUT eye type is change, now eyeType=%1.").arg(dutConfig.eyeType).toStdString());
    }

    cv::Point2f dutAngle;
    Result ret = getCoordinate2DTilt(ACS2DDUT, dutAngle);
    if (!ret.success)
    {
        return Result(false, "Judge same DUT error," + ret.errorMsg);
    }
    if (abs(inputAlignState.dutAngle.x - dutAngle.x) > 0.01 || 
        abs(inputAlignState.dutAngle.y - dutAngle.y) > 0.01){
        return Result(false, QString("DUT angle is changed, DUT tilt before(%1, %2), DUT tilt current(%3, %4).")
                                 .arg(QString::number(dutAngle.x, 'f', 3))
                                 .arg(QString::number(dutAngle.y, 'f', 3))
                                 .arg(QString::number(dutAngle.x, 'f', 3))
                                 .arg(QString::number(dutAngle.y, 'f', 3))
                                 .toStdString());
    }

    cv::Point2f prAngle;
    ret = getCoordinate2DTilt(ACS2DPro, prAngle);
    if (!ret.success)
    {
        return Result(false, "Judge same DUT error," + ret.errorMsg);
    }
    if (abs(inputAlignState.prAngle.x - prAngle.x) > 0.01 || 
        abs(inputAlignState.prAngle.y - prAngle.y) > 0.01)
    {
        return Result(false, "Projector angle is changed.");
    }

    return Result();
}

Result MoveLimit::dutMoveToSafeBeforeMoveUp(const cv::Point3f &destCoord)
{
    Result ret;
    //if (Motion2DModel::getInstance(ACS2DPro)->GetIsConnected() &&
    //    Motion2DModel::getInstance(ACS2DDUT)->GetIsConnected())
    //{
    //    ret = dutProjectorCollisionMoveUp(destCoord);
    //    return ret;
    //}

    float offsetZ = 0;
    if (!Motion2DModel::getInstance(ACS2DPro)->GetIsConnected() &&
        !Motion2DModel::getInstance(ACS2DDUT)->GetIsConnected())
    {
        cv::Point3f dutCoordCurrent;
        ret = getCoordinate(withDUT, dutCoordCurrent);
        if (!ret.success)
        {
            return ret;
        }

        if (abs(destCoord.x - dutCoordCurrent.x) < 1 && abs(destCoord.y - dutCoordCurrent.y) < 1 &&
            abs(destCoord.z - dutCoordCurrent.z) < 1)
        {
            return Result();
        }
        offsetZ = 5;
    }
    else 
    {
        int eyeType = CalibrationConfig::instance()->getDutConfig().eyeType;
        QList<DutPrNearInfo> dutPrNearInfoList;
        ret = DutProjectorCalibrate::instance()->getDutPrNearCoordinatesByEye(DUT_EYE_TYPE(eyeType), dutPrNearInfoList);
        if (!ret.success)
        {
            return ret;
        }

        cv::Point3f dutCurrentCoord;
        Result ret = getCoordinate(withDUT, dutCurrentCoord);
        if (!ret.success)
        {
            return ret;
        }

        if (Motion2DModel::getInstance(ACS2DPro)->GetIsConnected())
        {
            cv::Point2f proCoord;
            ret = getCoordinate2DTilt(ACS2DPro, proCoord);
            if (!ret.success)
            {
                return ret;
            }

            for (DutPrNearInfo nearInfo : dutPrNearInfoList)
            {
                if (abs(nearInfo.prAngle.x - proCoord.x) < 1 && abs(nearInfo.prAngle.y - proCoord.y) < 1)
                {
                    ret = dutProjectorCollisionMoveUpCal(offsetZ, nearInfo.dutPrNearCoordinate, dutCurrentCoord, destCoord);
                    if (!ret.success)
                    {
                        return ret;
                    }
                    break;
                }
            }
        }
        else
        {
            cv::Point2f dutTiltCoord;
            ret = getCoordinate2DTilt(ACS2DDUT, dutTiltCoord);
            if (!ret.success)
            {
                return ret;
            }

            for (DutPrNearInfo nearInfo : dutPrNearInfoList)
            {
                if (abs(nearInfo.dutAngle.x - dutTiltCoord.x) < 1 && abs(nearInfo.dutAngle.y - dutTiltCoord.y) < 1)
                {
                    ret = dutProjectorCollisionMoveUpCal(offsetZ, nearInfo.dutPrNearCoordinate, dutCurrentCoord,
                                                         destCoord);
                    if (!ret.success)
                    {
                        return ret;
                    }
                    break;
                }
            }
        }
    }

    if (offsetZ > 1)
    {
        ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveRel(0, 0, offsetZ * UNIT_CONVERSION);
        if (!ret.success)
        {
            return ret;
        }
    }
    return Result();
}

Result MoveLimit::dutProjectorCollisionMoveUpCal(float &offsetZ, const cv::Point3f &dutPrNearCoordinate,
                                                 const cv::Point3f &dutCurrentCoord, const cv::Point3f &destCoord)
{
    DutProjectorCollision cosInfo = MoveLimitConfig::instance()->getDutProjectorCollision();
    float minX = dutPrNearCoordinate.x - cosInfo.rectangle.x / 5.0;
    float maxX = dutPrNearCoordinate.x + cosInfo.rectangle.x / 5.0;
    float minY = dutPrNearCoordinate.y - cosInfo.rectangle.y / 5.0;
    float maxY = dutPrNearCoordinate.y + cosInfo.rectangle.y / 5.0;
    float minZ = dutPrNearCoordinate.z + cosInfo.minDistanceZ;

    float disX = dutCurrentCoord.x;
    float disY = dutCurrentCoord.y;
    float disZ = dutCurrentCoord.z;

    // if (disX > minX && disX < maxX && disY > minY && disY < maxY)
    if (disX < maxX && disY < maxY)
    {
        if (disZ < minZ)
        {
            return Result(false, "DUT is under projector, please check the device.");
        }
        else
        {
            if (abs(destCoord.x - disX) > 1 || abs(destCoord.y - disY) > 1)
            {
                offsetZ = 5;
            }
        }
    }
    else if (destCoord.x < maxX && destCoord.y < maxY)
    {
        if (abs(destCoord.x - disX) > 1 || abs(destCoord.y - disY) > 1)
        {
            offsetZ = dutPrNearCoordinate.z - disZ + 5;
        }
    }
    return Result();
}

Result MoveLimit::judgeEyeboxLimit(const cv::Point3f &destCoordMc, int eyeboxIndex)
{
    int eyeType = CalibrationConfig::instance()->getDutConfig().eyeType;
    QMap<int, QList<EyeboxLimit>> limitMap = MoveLimitConfig::instance()->getEyeboxLimit();
    if (!limitMap.contains(eyeType)){
        return eyeboxIndex < 0
            ? Result() : Result(
            false, QString("Judge eyebox limit error, config file does not contain limit info, eye type is %1.")
                       .arg(eyeType)
                       .toStdString());
    }

    cv::Point2f dutTiltCoord;
    Result ret = getCoordinate2DTilt(ACS2DDUT, dutTiltCoord);
    if(!ret.success){
        return Result(false, "Judge eyebox limit error," + ret.errorMsg);
    }

    cv::Point2f proTiltCoord;
    ret = getCoordinate2DTilt(ACS2DPro, proTiltCoord);
    if (!ret.success)
    {
        return Result(false, "Judge eyebox limit error," + ret.errorMsg);
    }

    QList<EyeboxLimit> limitList = limitMap[eyeType];
    for (EyeboxLimit limit: limitList){
        if (abs(proTiltCoord.x - limit.projectorAngle.x) < 0.5 && 
            abs(proTiltCoord.y - limit.projectorAngle.y) < 0.5){
            if (destCoordMc.y > limit.min.y && destCoordMc.y < limit.max.y){
                if (destCoordMc.x < limit.min.x)
                {
                    return Result(false,
                                  QString("Judge eyebox limit error, eyebox index is %1, X min is over limit, MC target %2, limit min %3.")
                                      .arg(eyeboxIndex)
                                      .arg(coordToString(destCoordMc))
                                      .arg(coordToString(limit.min))
                                      .toStdString());
                }
            }
        }

        if (eyeboxIndex >= 0){
            if (abs(dutTiltCoord.x - limit.dutAngle.x) < 0.5 &&
                abs(dutTiltCoord.y - limit.dutAngle.y) < 0.5)
            {
                if (destCoordMc.x > limit.min.x && destCoordMc.x < limit.max.x)
                {
                    if (destCoordMc.y > limit.max.y)
                    {
                        return Result(false, QString("Judge eyebox limit error, eyebox index is %1, Y max is over "
                                                     "limit, MC target %2, limit max %3.")
                                                 .arg(eyeboxIndex)
                                                 .arg(coordToString(destCoordMc))
                                                 .arg(coordToString(limit.max))
                                                 .toStdString());
                    }
                    else if (destCoordMc.y > limit.min.y)
                    {
                        if (destCoordMc.z > limit.max.z){
                            return Result(false, QString("Judge eyebox limit error, eyebox index is %1, Z max is over "
                                                         "limit, MC target %2, limit max %3.")
                                                     .arg(eyeboxIndex)
                                                     .arg(coordToString(destCoordMc))
                                                     .arg(coordToString(limit.max))
                                                     .toStdString());
                        }
                    }
                }
            }
        }
    }

    return Result();
}

#include "MoveLimitJX.h"
#include <QMutex>
#include <QThread>
#include "Motion2DModel.h"
#include "DutProjectorCalibrate.h"
#include "Core/loggingwrapper.h"

MoveLimitJX *MoveLimitJX::m_instance = nullptr;

MoveLimitJX *MoveLimitJX::instance()
{
    if (!m_instance)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance)
        {
            m_instance = new MoveLimitJX();
        }
    }
    return m_instance;
}

MoveLimitJX::MoveLimitJX()
{
}

Result MoveLimitJX::dutMoveRelNoCollision(const cv::Point2f& coordinate)
{
    if (!Motion2DModel::getInstance(ACS2DDUTPlane)->GetIsConnected())
    {
        return Result(false, "DUT 2D motion is not connected, DUT abs move failed.");
    }

    cv::Point2f point;
    bool ret = Motion2DModel::getInstance(ACS2DDUTPlane)->getPosition(point.x, point.y);
    cv::Point2f current = cv::Point2f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION);
    current = current + coordinate;

    return dutMoveAbsNoCollision(current);
}

Result MoveLimitJX::dutMoveAbsNoCollision(const cv::Point2f &coordinate)
{
    if (!Motion2DModel::getInstance(ACS2DDUTPlane)->GetIsConnected())
    {
        return Result(false, "DUT 2D motion is not connected, DUT abs move failed.");
    }

    cv::Point2f acsCoord = coordinate;
    Result ret = dutPreJudge(acsCoord);
    if (!ret.success)
    {
        return ret;
    }

    //TODO:test
    //ret = judgeDut(coordinate);
    //if (!ret.success)
    //{
    //    return ret;
    //}

    return dutMoveOrder(acsCoord);  
}

Result MoveLimitJX::mcMoveAbsNoCollision(const cv::Point3f &coordinate)
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

    //TODO:test
    //result = judgeMeasureCamera(coordinate);
    //if (!result.success)
    //{
    //    return result;
    //}

    return mcMoveOrder(acsCoord);
}

Result MoveLimitJX::moveDutInitTestPos()
{
    cv::Point2f pos = MoveLimitConfigJX::instance()->getInitTestDutPosition();
    Result ret = dutMoveAbs(pos);
    return ret;
}

Result MoveLimitJX::moveMcInitTestPos()
{
    Result ret;
    cv::Point3f pos = MoveLimitConfigJX::instance()->getInitTestMcPosition();
    ret = mcMoveAbs(pos);
    return ret;
}

Result MoveLimitJX::dutMoveOrder(const cv::Point2f &coordinate)
{
    ML_Point point = Motion2DModel::getInstance(ACS2DDUTPlane)->getPosition();
    cv::Point2f currCoord = cv::Point2f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION);
    cv::Point2f newCoord = coordinate;

    Result ret;
    if (newCoord.y > currCoord.y) {
        ret = Motion2DModel::getInstance(ACS2DDUTPlane)->Motion2DMoveAbs(currCoord.x, newCoord.y);
        if (!ret.success)
        {
            return ret;
        }
    }
    else {
        ret = Motion2DModel::getInstance(ACS2DDUTPlane)->Motion2DMoveAbs(newCoord.x, currCoord.y);
        if (!ret.success)
        {
            return ret;
        }
    }

    ret = Motion2DModel::getInstance(ACS2DDUTPlane)->Motion2DMoveAbs(newCoord.x, newCoord.y);
    return ret;
}

Result MoveLimitJX::mcMoveOrder(const cv::Point3f &coordinate)
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

MoveLimitJX::~MoveLimitJX()
{
}

Result MoveLimitJX::setAlignState(bool isAlign, const cv::Point2f& inputAlignCoord)
{
    for (AlignCalibrateCallback* pCall : m_alignCalibrateCallbacks)
    {
        if (pCall != nullptr)
        {
            pCall->notifyInputAAStateChanged(isAlign);
        }
    }

    if (!isAlign) {
        m_alignState.isAlign = false;
        return Result();
    }

    m_alignState.inputAlignCoord = inputAlignCoord;

    Result ret = getCoordinate2DTilt(m_alignState.prAngle);
    if (!ret.success)
    {
        return Result(false, "Set input align state error," + ret.errorMsg);
    }

    m_alignState.isAlign = true;
    m_alignState.dutSN = CalibrationConfig::instance()->getDutConfig().sn;
    m_alignState.eyeType = CalibrationConfig::instance()->getDutConfig().eyeType;
    return Result();
}

bool MoveLimitJX::completeAA()
{
    if (!m_alignState.isAlign) {
        return false;
    }

    Result ret = isDutChange(m_alignState);
    if (!ret.success) {
        LoggingWrapper::instance()->warn(QString::fromStdString(ret.errorMsg));
        return false;
    }

    return true;
}

Result MoveLimitJX::resetLastInputAA()
{
    if (!m_alignState.isAlign)
    {
        return Result(false, "Please first auto or manual align.");
    }

    Result ret = isDutChange(m_alignState);
    if (!ret.success)
    {
        return Result(false, "Please first auto or manual align, " + ret.errorMsg);
    }

    //ret = moveBeforeInputAA(m_alignState.inputAlignCoord);
    //if (!ret.success)
    //{
    //    return Result(false, QString("Reset last inputAA failed, %1.").arg(QString::fromStdString(ret.errorMsg)).toStdString());
    //}

    return dutMoveAbs(m_alignState.inputAlignCoord);
}

Result MoveLimitJX::setMotion3DMoveAlignSpeed(bool flag)
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

        cv::Point3f alignSpeed = MoveLimitConfigJX::instance()->getMotion3DMoveAlignSpeed();
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

void MoveLimitJX::subscribe(AlignCalibrateCallback* callback)
{
    if (!m_alignCalibrateCallbacks.contains(callback))
    {
        m_alignCalibrateCallbacks.push_back(callback);
    }
}

void MoveLimitJX::unsubscribe(AlignCalibrateCallback* callback)
{
    if (m_alignCalibrateCallbacks.contains(callback))
    {
        m_alignCalibrateCallbacks.removeOne(callback);
    }
}

Result MoveLimitJX::moveAbs(const cv::Point3f &coordinate, motion3DType type)
{
    if (type == withCamera){
        return mcMoveAbs(coordinate);
    }
}

Result MoveLimitJX::moveRel(const cv::Point3f &coord, motion3DType type)
{
    ML_Point3D point = Motion3DModel::getInstance(type)->getPosition();
    cv::Point3f current = cv::Point3f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION, point.z / UNIT_CONVERSION);
    current = current + coord;

    if (type == withCamera)
    {
        return mcMoveAbs(current);
    }
}

Result MoveLimitJX::moveAbsNoCollision(const cv::Point3f &coordinate, motion3DType type)
{
    if (type == withCamera)
    {
        return mcMoveAbsNoCollision(coordinate);
    }
}

Result MoveLimitJX::moveRelNoCollision(const cv::Point3f &coord, motion3DType type)
{
    ML_Point3D point = Motion3DModel::getInstance(type)->getPosition();
    cv::Point3f current = cv::Point3f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION, point.z / UNIT_CONVERSION);
    current = current + coord;

    if (type == withCamera)
    {
        return mcMoveAbsNoCollision(current);
    }
}

Result MoveLimitJX::moveAbsToSafe(motion3DType type)
{
    Result ret;
    cv::Point3f pos;
    if (withCamera == type)
    {
        pos = MoveLimitConfigJX::instance()->getLoadMeasureCameraPosition();
    }

    ret = moveAbsNoCollision(pos, type);
    return ret;
}

Result MoveLimitJX::moveDutAbsToSafe()
{
    Result ret;
    cv::Point2f pos = MoveLimitConfigJX::instance()->getLoadDutPosition();

    ret = dutMoveAbsNoCollision(pos);
    return ret;
}

Result MoveLimitJX::dutMoveAbs(const cv::Point2f &coordinate)
{
    if(!Motion2DModel::getInstance(ACS2DDUTPlane)->GetIsConnected()){
        return Result(false, "DUT 2D motion is not connected, DUT abs move failed.");
    }

    cv::Point2f acsCoord = coordinate;
    Result ret = MoveLimitJX::judgeDut(acsCoord);
    if (!ret.success)
    {
        return ret;
    }

    return dutMoveOrder(acsCoord);
}

Result MoveLimitJX::mcMoveAbs(const cv::Point3f &coordinate)
{
    if (!Motion3DModel::getInstance(withCamera)->Motion3DisConnected())
    {
        return Result(false, "Measure camera 3D motion is not connected, Measure camera abs move failed.");
    }

    Result result = MoveLimitJX::judgeMeasureCamera(coordinate);
    if (!result.success)
    {
        return result;
    }

    return mcMoveOrder(coordinate);
}

Result MoveLimitJX::dutMoveRel(const cv::Point2f& coordinate)
{
    cv::Point2f point;
    bool ret = Motion2DModel::getInstance(ACS2DDUTPlane)->getPosition(point.x, point.y);
    cv::Point2f current = cv::Point2f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION);
    current = current + coordinate;
    return dutMoveAbs(current);
}

Result MoveLimitJX::mcMoveRel(const cv::Point3f& coordinate)
{
    return moveRel(coordinate);
}

Result MoveLimitJX::judgeDut(const cv::Point2f &coordinate)
{
    //TODO: test
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
    ret = getCoordinate3D(mcCoord);
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

Result MoveLimitJX::judgeMeasureCamera(const cv::Point3f &coordinate)
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

    cv::Point2f dutCoord;
    ret = getCoordinate2D(dutCoord);
    if (!ret.success)
    {
        return ret;
    }

    //TODO:test
    ret = dutMcCollision(dutCoord, coordinate);
    if (!ret.success)
    {
        return Result(false, QString("Measure camera move failed, %1 Measure camera target coordinate is %2.")
                                 .arg(QString::fromStdString(ret.errorMsg))
                                 .arg(coordToString(coordinate))
                                 .toStdString());
    }

    ret = mcProjectorCollision(coordinate);
    if (!ret.success)
    {
        return Result(false, QString("Measure camera move failed, %1 Measure camera target coordinate is %2.")
            .arg(QString::fromStdString(ret.errorMsg))
            .arg(coordToString(coordinate))
            .toStdString());
    }

    return Result();
}

Result MoveLimitJX::moveInit()
{
    Result ret = moveAbsToSafe(withCamera);
    if (!ret.success)
    {
        return Result(false, "Move Measure camera to init position error, " + ret.errorMsg);
    }

    ret = moveDutAbsToSafe();
    if(!ret.success){
        return Result(false, "Move DUT to init position error, " + ret.errorMsg);
    }

    return Result();
}

Result MoveLimitJX::dutMcCollision(const cv::Point2f &dutCoord, const cv::Point3f &mcCoord)
{
    DutMcCollisionJX cosInfo;
    Result ret = MoveLimitConfigJX::instance()->getDutMcCollision(cosInfo);
    if(!ret.success){
        return ret;
    }

    cv::Point2f dutCenter = cv::Point2f(cosInfo.centerRelative.x + mcCoord.x, cosInfo.centerRelative.y + mcCoord.y);
    float minX = dutCenter.x - cosInfo.rectangle.x / 2.0;
    float maxX = dutCenter.x + cosInfo.rectangle.x / 2.0;
    float minY = dutCenter.y - cosInfo.rectangle.y / 2.0;
    float maxY = dutCenter.y + cosInfo.rectangle.y / 2.0;
    float mcMaxZ = cosInfo.mcMaxZ;

    float disX = dutCoord.x;
    float disY = dutCoord.y;
    float disZ = mcCoord.z;

    if (disX > minX && disX < maxX) {
        if (disY > minY && disY < maxY) {
            if (disZ > mcMaxZ) {
                return Result(false, "DUT and Measure camera exist DUT MC Collision danger, exist collision in Z direction.");
            }
        }else if(disY < minY){
            return Result(false, "DUT and Measure camera exist DUT MC Collision danger, exist collision in Y direction.");
        }
    }
    return Result();
}

Result MoveLimitJX::dutProjectorCollision(const cv::Point2f &dutCoord)
{
    DutProjectorCollisionJX cosInfo;
    cv::Point2f proCoord;
    Result ret = getCoordinate2DTilt(proCoord);
    if (ret.success){
        ret = MoveLimitConfigJX::instance()->getDutProjectorCollision(cosInfo, proCoord);
    }else{
        ret = MoveLimitConfigJX::instance()->getDutProjectorCollision(cosInfo);
    }
    if(!ret.success){
        return ret;
    }

    float minX = cosInfo.centerRelative.x - cosInfo.rectangle.x / 2.0;
    float maxX = cosInfo.centerRelative.x + cosInfo.rectangle.x / 2.0 + 10;
    float minY = cosInfo.centerRelative.y - cosInfo.rectangle.y / 2.0;
    float maxY = cosInfo.centerRelative.y + cosInfo.rectangle.y / 2.0;

    float disX = dutCoord.x;
    float disY = dutCoord.y;

    if (disY < maxY)
    {
        if (disX < minX)
        {
            return Result(false, "DUT and projector exist Collision danger, exist collision in X direction.");
        }
    }

    if (disX < maxX) {
        if (disY < minY) {
            return Result(false, "DUT and projector exist Collision danger, exist collision in Y direction.");
        }
    }
    return Result();
}

Result MoveLimitJX::mcProjectorCollision(const cv::Point3f& mcCoord)
{
    McProjectorCollisionJX cosInfo;
    cv::Point2f proCoord;
    Result ret = getCoordinate2DTilt(proCoord);
    if(ret.success )
    {
        ret = MoveLimitConfigJX::instance()->getMcProjectorCollision(cosInfo, proCoord);
    }else{
        ret = MoveLimitConfigJX::instance()->getMcProjectorCollision(cosInfo);
    }
    if (!ret.success) {
        return ret;
    }

    float minZ = cosInfo.centerRelative.z - cosInfo.rectangle.z / 2.0;
    float maxZ = cosInfo.centerRelative.z + cosInfo.rectangle.z / 2.0;
    float minY = cosInfo.centerRelative.y - cosInfo.rectangle.y / 2.0;
    float maxY = cosInfo.centerRelative.y + cosInfo.rectangle.y / 2.0;

    float disX = mcCoord.x;
    float disY = mcCoord.y;
    float disZ = mcCoord.z;

    if(disY > minY && disY < maxY && disZ > minZ){
        if(disX < cosInfo.mcMinX){
            return Result(false, "Measure camera and projector exist Collision danger.");
        }
    }
    return Result();
}

Result MoveLimitJX::getCoordinate3D(cv::Point3f &coord, motion3DType motionType)
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

Result MoveLimitJX::moveAbs2DTilt(const cv::Point2f &coordinate, motion2DType type)
{
    cv::Point2f currentPos;
    Result ret = getCoordinate2DTilt(currentPos, type);
    if(!ret.success){
        return ret;
    }

    if (abs(currentPos.x - coordinate.x) > 0.1 || abs(currentPos.y - coordinate.y) > 0.1)
    {
        // Judge Measure camera is or not safe position
        cv::Point3f prInitCoord = MoveLimitConfigJX::instance()->getLoadMeasureCameraPosition();
        cv::Point3f prCoord;
        ret = getCoordinate3D(prCoord);
        if (!ret.success)
        {
            return ret;
        }

        if (abs(prInitCoord.x - prCoord.x) > 1 || abs(prInitCoord.y - prCoord.y) > 1)
        {
            return Result(false, "Motion 2D set tilt failed, Measure camera motion 2D is not safe position.");
        }

        // Judge DUT is or not safe position
        cv::Point2f dutInitCoord = MoveLimitConfigJX::instance()->getLoadDutPosition();
        cv::Point2f dutCoord;
        ret = getCoordinate2D(dutCoord);
        if (!ret.success)
        {
            return ret;
        }

        if (abs(dutInitCoord.x - dutCoord.x) > 1 || abs(dutInitCoord.y - dutCoord.y) > 1)
        {
            return Result(false, "Motion 2D set tilt failed, DUT motion 2D is not safe position.");
        }
    }

    ret = Motion2DModel::getInstance(type)->Motion2DMoveAbs(coordinate.x, coordinate.y);
    return ret;
}

Result MoveLimitJX::moveBeforeInputAA(const cv::Point2f& dutCoord)
{
    cv::Point3f mcCoordCurrent;
    Result ret = getCoordinate3D(mcCoordCurrent);
    if(!ret.success){
        return ret;
    }

    ret = dutMcCollision(dutCoord, mcCoordCurrent);
    if(ret.success)
    {
        return Result();
    }

    cv::Point3f mcCoord = MoveLimitConfigJX::instance()->getLoadMeasureCameraPosition();
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

Result MoveLimitJX::judgeEyeboxLimit(const std::vector<cv::Point3f> &eyeboxs)
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

Result MoveLimitJX::dutPreJudge(const cv::Point2f&coord)
{
    Limit2D limit = MoveLimitConfigJX::instance()->getDutLimit();
    return preJudge(coord, limit);
}

Result MoveLimitJX::mcPreJudge(const cv::Point3f &coord)
{
    CoordinateLimit limit = MoveLimitConfigJX::instance()->getMeasureCameraLimit();
    return preJudge(coord, limit);
}

Result MoveLimitJX::preJudge(const cv::Point3f &coord, const CoordinateLimit &limit)
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

Result MoveLimitJX::preJudge(const cv::Point2f& coord, const CoordinateLimit& limit)
{
    if (coord.x < limit.min.x || coord.y < limit.min.y)
    {
        return Result(false, QString("Motion3D move coordinate is over min limit, target is %1, min is %2")
            .arg(coordToString(coord))
            .arg(coordToString(limit.min))
            .toStdString());
    }

    if (coord.x > limit.max.x || coord.y > limit.max.y)
    {
        return Result(false, QString("Motion3D move coordinate is over max limit, target is %1, max is %2")
            .arg(coordToString(coord))
            .arg(coordToString(limit.max))
            .toStdString());
    }
    return Result();
}

Result MoveLimitJX::preJudge(const cv::Point2f& coord, const Limit2D& limit)
{
    if (coord.x < limit.min.x || coord.y < limit.min.y)
    {
        return Result(false, QString("Motion3D move coordinate is over min limit, target is %1, min is %2")
            .arg(coordToString(coord))
            .arg(coordToString(limit.min))
            .toStdString());
    }

    if (coord.x > limit.max.x || coord.y > limit.max.y)
    {
        return Result(false, QString("Motion3D move coordinate is over max limit, target is %1, max is %2")
            .arg(coordToString(coord))
            .arg(coordToString(limit.max))
            .toStdString());
    }
    return Result();
}

QString MoveLimitJX::coordToString(const cv::Point3f &coord)
{
    return QString("(%1,%2,%3)").arg(coord.x).arg(coord.y).arg(coord.z);
}

QString MoveLimitJX::coordToString(const cv::Point2f& coord)
{
    return QString("(%1,%2)").arg(coord.x).arg(coord.y);
}

Result MoveLimitJX::getCoordinate2DTilt(cv::Point2f &coord, motion2DType motionType)
{
    QString str = "DUT";
    if (ACS2DPro == motionType)
    {
        str = "Projector tilt";
    }
    else if (ACS2DReticle == motionType)
    {
        str = "Reticle";
    }
    else if (ACS2DDUTPlane == motionType)
    {
        str = "DUT";
    }

    if (!Motion2DModel::getInstance(motionType)->GetIsConnected()){
        return Result(false, QString("%1 Motion 2D is not connected.").arg(str).toStdString());
    }

    CORE::ML_Point point = Motion2DModel::getInstance(motionType)->getPosition();
    coord.x = point.x / UNIT_CONVERSION;
    coord.y = point.y / UNIT_CONVERSION;
    return Result();
}

Result MoveLimitJX::getCoordinate2DTiltRaw(cv::Point2f& coord, motion2DType motionType)
{
    QString str = "DUT";
    if (ACS2DPro == motionType)
    {
        str = "Projector tilt";
    }
    else if (ACS2DReticle == motionType)
    {
        str = "Reticle";
    }
    else if (ACS2DDUTPlane == motionType)
    {
        str = "DUT";
    }

    if (!Motion2DModel::getInstance(motionType)->GetIsConnected()) {
        return Result(false, QString("%1 Motion 2D is not connected.").arg(str).toStdString());
    }

    bool ret = Motion2DModel::getInstance(motionType)->getPositionRaw(coord);
    if (!ret) {
        return Result(false, "Get motion 2D coordinate error.");
    }
    return Result();
}

Result MoveLimitJX::getCoordinate2D(cv::Point2f& coord, motion2DType motionType)
{
    return getCoordinate2DTilt(coord, motionType);
}

Result MoveLimitJX::isDutChange(const AlignStateJX &inputAlignState)
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

    cv::Point2f prAngle;
    Result ret = getCoordinate2DTilt(prAngle);
    if (!ret.success)
    {
        return Result(false, "Get projector tilt error," + ret.errorMsg);
    }

    if (abs(inputAlignState.prAngle.x - prAngle.x) > 0.01 || 
        abs(inputAlignState.prAngle.y - prAngle.y) > 0.01)
    {
        return Result(false, "Projector angle is changed.");
    }

    return Result();
}

Result MoveLimitJX::judgeEyeboxLimit(const cv::Point3f &destCoordMc, int eyeboxIndex)
{
    //TODO: to be test
    return Result();

    int eyeType = CalibrationConfig::instance()->getDutConfig().eyeType;
    QMap<int, QList<EyeboxLimitJX>> limitMap = MoveLimitConfigJX::instance()->getEyeboxLimit();
    if (!limitMap.contains(eyeType)){
        return Result(
            false, QString("Judge eyebox limit error, config file does not contain limit info, eye type is %1.")
            .arg(eyeType)
            .toStdString());
    }

    //TODO: to be done in config file
    cv::Point2f measureTiltCoord;

    cv::Point2f proTiltCoord;
    Result ret = getCoordinate2DTilt(proTiltCoord, ACS2DPro);
    if (!ret.success)
    {
        return Result(false, "Judge eyebox limit error," + ret.errorMsg);
    }

    QList<EyeboxLimitJX> limitList = limitMap[eyeType];
    for (EyeboxLimitJX limit: limitList){
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
                    if (destCoordMc.z > limit.max.z) {
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

    return Result();
}

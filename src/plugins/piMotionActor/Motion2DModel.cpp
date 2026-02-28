
#include"Motion2DModel.h"
#include "MLACSController2D.h"
#include "QMutex"
#include "configItem.h"
#include "Core/icore.h"
#include "CalibrationConfig.h"
#include "PLCControl/PLCController.h"
#include "PrjCommon/PrjCommon.h"

Motion2DModel *Motion2DModel::selfACS2DPro = nullptr;
Motion2DModel *Motion2DModel::selfACS2DDUT = nullptr;
Motion2DModel *Motion2DModel::selfACS2DReticle = nullptr;
Motion2DModel* Motion2DModel::selfACS2DDUTPlane = nullptr;
Motion2DModel* Motion2DModel::selfACS2DCameraModule = nullptr;

Motion2DModel::Motion2DModel(int type)
{
    typeMotion = type;
    m_pMotion2D = new MLACSCController2D(type);
    if (ACS2DPro == typeMotion)
    {
        isHome = true;
    }

    updateDutConfigSlot();
    connect(Core::PrjCommon::instance(), SIGNAL(updateDutConfigSignal()), this, SLOT(updateDutConfigSlot()));
}

void Motion2DModel::updatePositon(double &posX, double &posY)
{
    posX = m_zeroPosition.x + posX;
    posY = m_zeroPosition.y + posY;
}

Result Motion2DModel::collisionControl()
{
    if (typeMotion == ACS2DReticle)
    {
        return Result();
    }

    Result ret;
    ret = PLCController::instance()->collisionControlPreMove();
    return ret;
}

Result Motion2DModel::judgeLimit(cv::Point2f target)
{
    if (typeMotion == ACS2DReticle)
    {
        return Result();
    }
     
    MotionLimit m_limitss = ConfigItem::instance()->getMotion2DLimits(typeMotion);
    if (target.x > m_limitss.x_pos || 
        target.x < m_limitss.x_neg || 
        target.y > m_limitss.y_pos ||
        target.y < m_limitss.y_neg)
    {
        return Result(
            false,
            QString(
                "Move Motion2D failed, coordinate is over limit, target coordinate (%1,%2), Min (%3,%4), Max (%5,%6).")
                .arg(target.x)
                .arg(target.y)
                .arg(m_limitss.x_neg)
                .arg(m_limitss.y_neg)
                .arg(m_limitss.x_pos)
                .arg(m_limitss.y_pos)
                .toStdString());
    }
    return Result();
}

Result Motion2DModel::judgeLimitRel(cv::Point2f offset)
{
    cv::Point2f currentCoord;
    //getPositionRaw(currentCoord);
    getPosition(currentCoord.x, currentCoord.y);
    currentCoord = currentCoord / m_scale;
    return judgeLimit(currentCoord + offset);
}

Result Motion2DModel::getPosition(cv::Point2f &coord, cv::Point2f &coordOriginal)
{
    if (!m_pMotion2D->IsConnected())
    {
        return Result(false, "Motion2D get position error, it is not connnected.");
    }

    CORE::ML_Point point = m_pMotion2D->GetPosition();
    cv::Point zeroPosition = m_zeroPosition * m_scale;
    coord.x = point.x - zeroPosition.x;
    coord.y = point.y - zeroPosition.y;

    coordOriginal.x = point.x;
    coordOriginal.y = point.y;
    return Result();
}

Motion2DModel::~Motion2DModel()
{
    if (m_pMotion2D != nullptr)
    {
        delete (MLACSCController2D*)m_pMotion2D;
        m_pMotion2D = nullptr;
    }
}

Result Motion2DModel::Motion2DMoveRelAsync(double posX, double posY)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    isEStop = false;
    isHome = isHoming().success;
    if (is_Connected && !isEStop&&isHome)
    {
        Result ret = judgeLimitRel(cv::Point2f(posX, posY));
        if (!ret.success)
        {
            return ret;
        }

        ret = m_pMotion2D->MoveRelAsync(posX, posY);
        if (isEStop)
        {
            return Result(false, "ACS 2D is requested to stop", 1);
        }
        return ret;
    }

    return Result(false, std::string("Acs 2D connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop) + std::string(", and Homing = ") +
                             std::to_string(isHome));
}

Result Motion2DModel::Motion2DMoveAbsAsync(double posX, double posY)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    updatePositon(posX, posY);

    isEStop = false;
    isHome = isHoming().success;
    if (is_Connected && !isEStop && isHome)
    {
        Result ret = judgeLimit(cv::Point2f(posX, posY));
        if (!ret.success)
        {
            return ret;
        }

        ret = m_pMotion2D->MoveAbsAsync(posX, posY);
        if (isEStop)
        {
            return Result(false, "ACS 2D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs 2D connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop) + std::string(", and Homing = ") +
                             std::to_string(isHome));
}

Result Motion2DModel::Motion2DMoveRel(double posX, double posY)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    isEStop = false;
    isHome = isHoming().success;
    if (is_Connected && !isEStop && isHome)
    {     
        Result ret = judgeLimitRel(cv::Point2f(posX, posY));
        if (!ret.success)
        {
            return ret;
        }

        ret = m_pMotion2D->MoveRel(posX, posY);
        if (isEStop)
        {
            return Result(false, "ACS 2D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs 2D connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop) + std::string(", and Homing = ") +
                             std::to_string(isHome));
}

Result Motion2DModel::Motion2DMoveAbs(double posX, double posY)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    updatePositon(posX, posY);

    isEStop = false;
    isHome = isHoming().success;
    if (is_Connected && !isEStop && isHome)
    {
        Result ret = judgeLimit(cv::Point2f(posX, posY));
        if (!ret.success)
        {
            return ret;
        }

        ret = m_pMotion2D->MoveAbs(posX, posY);
        if (isEStop)
        {
            return Result(false, "ACS 2D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs 2D connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop) + std::string(", and Homing = ") +
                             std::to_string(isHome));
}

Result Motion2DModel::Motion2DMoveStop()
{
    isEStop = true;
    isHome = isHoming().success;
    if (is_Connected && isHome)
    {
       return m_pMotion2D->StopMove();
    }
    return Result(false, std::string("Motion2D connect status = ") + std::to_string(is_Connected) +
                             std::string(", and Homing = ") + std::to_string(isHome));
}

bool Motion2DModel::is2DMoving()
{
    return m_pMotion2D->IsMoving();
}

Result Motion2DModel::connectAndInit(QString str)
{
    Result res = m_pMotion2D->Connect(str.toStdString().c_str());
    if (res.success)
    {
        is_Connected = true;
        emit updateMotion2DStatusSignal(MLMotionState::kMLStatusConnected);
    }
    return res;
}

bool Motion2DModel::disconnect()
{
    bool res = m_pMotion2D->Disconnect();
    if (res)
    {
        is_Connected = false;
    }
    emit updateMotion2DStatusSignal(MLMotionState::kMLStatusDisConnected);
    return res;
}

bool Motion2DModel::Motion2DisConnected()
{
    bool ok = m_pMotion2D->IsConnected();
    return ok;
}

CORE::ML_Point Motion2DModel::getPosition()
{
    CORE::ML_Point point = m_pMotion2D->GetPosition();
    point.x = point.x - m_zeroPosition.x * m_scale;
    point.y = point.y - m_zeroPosition.y * m_scale;
    return point;
}

bool Motion2DModel::getPosition(float &x, float &y)
{
    if (!is_Connected)
    {
        return false;
    }

    ML_Point point = m_pMotion2D->GetPosition();
    x = point.x - m_zeroPosition.x * m_scale;
    y = point.y - m_zeroPosition.x * m_scale;
    return true;
}

void Motion2DModel::SetSpeed(int speed)
{
    if (is_Connected)
    {
        m_pMotion2D->SetSpeed(speed);
    }
}

void Motion2DModel::SetSpeedX(int speed)
{
    if (is_Connected)
    {
        m_pMotion2D->SetSpeedX(speed * 1.0);
    }
}

void Motion2DModel::SetSpeedY(int speed)
{
    if (is_Connected)
    {
        m_pMotion2D->SetSpeedY(speed * 1.0);
    }
}

//void Motion2DModel::SetDecSpeed(int speed)
//{
//    if (is_Connected)
//    {
//        ((MTmotion2DControl*)m_pMotion2D)->SetDecSpeed(speed);
//    }
//}
//void Motion2DModel::SetAccSpeed(int speed)
//{
//    if (is_Connected)
//    {
//        ((MTmotion2DControl *)m_pMotion2D)->SetAccSpeed(speed);
//    }
//}

Result Motion2DModel::Home()
{
    if (is_Connected)
    {
        return m_pMotion2D->Homing();
    }
    return Result(false, std::string("Motion2D connect status = ") + std::to_string(is_Connected));
}

Result Motion2DModel::isHoming()
{
    if (ACS2DPro == typeMotion || ACS2DDUTPlane == typeMotion|| ACS2DCameraTilt == typeMotion)
    {
        return Result();
    }
    bool flag;
    Result res=m_pMotion2D->IsHome(flag,typeMotion);
    return res;
}

Result Motion2DModel::homingForced()
{
    if (is_Connected)
    {
        return m_pMotion2D->homingForced();
    }
    return Result(false, std::string("Motion2D is not connected, homing is failed."));
}

void Motion2DModel::Motion2DSubscribeMotionEvent(MLMotionEvent event)
{
    if (m_pMotion2D != nullptr)
    {
        m_pMotion2D->Subscribe(event, this);
    }
}

void Motion2DModel::Motion2DUnsubscribeMotionEvent(MLMotionEvent event)
{
    if (m_pMotion2D != nullptr)
    {
        m_pMotion2D->Unsubscribe(event, this);
    }
}

void Motion2DModel::NotifyMotion2DStateChanged(MLMotionState old_state, MLMotionState new_state)
{
    emit updateMotion2DStatusSignal(new_state);
}

void Motion2DModel::NotifyMotion2DPosition()
{
    _sleep(200);
    //CORE::ML_Point pos = getPosition();
    cv::Point2f coord;
    cv::Point2f coordOriginal;

    Result ret = getPosition(coord, coordOriginal);
    if (ret.success)
    {
        emit updateMotion2DPositionSignal(coord.x / m_scale, coord.y / m_scale);
        emit updateMotion2DPositionOriginalSignal(coordOriginal.x / m_scale, coordOriginal.y / m_scale);
    }
}

Motion2DModel *Motion2DModel::getInstance(int type)
{
    switch (type)
    {
    case 0: {
        if (selfACS2DDUT == nullptr)
        {
            static QMutex mutex;
            QMutexLocker locker(&mutex);
            if (selfACS2DDUT == nullptr)
            {
                selfACS2DDUT = new Motion2DModel(type);
            }
        }
        return selfACS2DDUT;
    }
    case 1: {
        if (selfACS2DPro == nullptr)
        {
            static QMutex mutex;
            QMutexLocker locker(&mutex);
            if (selfACS2DPro == nullptr)
            {
                selfACS2DPro = new Motion2DModel(type);
            }
        }
        return selfACS2DPro;
    }
    case 2: {
        if (selfACS2DReticle == nullptr)
        {
            static QMutex mutex;
            QMutexLocker locker(&mutex);
            if (selfACS2DReticle == nullptr)
            {
                selfACS2DReticle = new Motion2DModel(type);
            }
        }
        return selfACS2DReticle;
    }    
    case 3: {
        if (selfACS2DDUTPlane == nullptr)
        {
            static QMutex mutex;
            QMutexLocker locker(&mutex);
            if (selfACS2DDUTPlane == nullptr)
            {
                selfACS2DDUTPlane = new Motion2DModel(type);
            }
        }
        return selfACS2DDUTPlane;
    }
    case 4: {
        if (selfACS2DCameraModule == nullptr)
        {
            static QMutex mutex;
            QMutexLocker locker(&mutex);
            if (selfACS2DCameraModule == nullptr)
            {
                selfACS2DCameraModule = new Motion2DModel(type);
            }
        }
        return selfACS2DCameraModule;
    }
    }
}

int Motion2DModel::GetSpeed()
{
    return m_pMotion2D->GetSpeed();
}

//void Motion2DModel::SetLimits(int limit_p_x, int limit_p_y, int limit_n_x, int limit_n_y)
//{
//    ((MTmotion2DControl *)m_pMotion2D)->setPosLimits(limit_p_x, limit_p_y, limit_n_x, limit_n_y);
//}

bool Motion2DModel::GetIsConnected()
{
    return is_Connected;
}

bool Motion2DModel::getEStop(){
    return isEStop;
}

void Motion2DModel::setEStop(bool val){
    isEStop = val;
}

bool Motion2DModel::getPositionRaw(cv::Point2f &pos)
{
    isHome = isHoming().success;
    if (!is_Connected || !isHome)
    {
        return false;
    }

    ML_Point point = m_pMotion2D->GetPosition();
    pos.x = point.x / m_scale;
    pos.y = point.y / m_scale;
    return true;
}

Result Motion2DModel::Motion2DMoveAbsRaw(cv::Point2f &pos)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    isEStop = false;
    isHome = isHoming().success;
    if (is_Connected && !isEStop && isHome)
    {
        Result ret = m_pMotion2D->MoveAbs(pos.x, pos.y);
        if (isEStop)
        {
            return Result(false, "ACS 2D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs 2D connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop) + std::string(", and Homing = ") +
                             std::to_string(isHome));
}

void Motion2DModel::updateMotion2DZero(int eyeType, const cv::Point2f &zeroPos)
{
    m_zeroPosition = zeroPos;
}

void Motion2DModel::updateDutConfigSlot()
{
    if (ACS2DPro == typeMotion)
    {
        DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
        m_zeroPosition = ConfigItem::instance()->getMotion2DZeroPosition(typeMotion, (DUT_EYE_TYPE)dutConfig.eyeType);
    }
}

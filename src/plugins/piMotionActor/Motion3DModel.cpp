#include "Motion3DModel.h"
#include "MLACSCController3D.h"
#include "PLCControl/PLCController.h"

Motion3DModel *Motion3DModel::selfDUT = nullptr;
Motion3DModel *Motion3DModel::selfCamera = nullptr;
Motion3DModel::Motion3DModel(int type)
{
    m_type = (motion3DType)type;
    m_pMotion3D = new MLACSCController3D;
}

Motion3DModel::~Motion3DModel()
{
    if (m_pMotion3D != nullptr)
    {
        delete m_pMotion3D;
        m_pMotion3D = nullptr;
    }
}
bool Motion3DModel::Motion3DisConnected()
{
    bool ok = m_pMotion3D->IsConnected();
    return ok;
}
void Motion3DModel::SetSpeedX(double speed)
{
    m_pMotion3D->SetSpeedX(speed);
}
void Motion3DModel::SetSpeedY(double speed)
{
    m_pMotion3D->SetSpeedY(speed);
}
void Motion3DModel::SetSpeedZ(double speed)
{
    if (speed > 10.0)
    {
        speed = 10;
    }
    m_pMotion3D->SetSpeedZ(speed);
}
int Motion3DModel::GetSpeed()
{
    return m_pMotion3D->GetSpeed() / 1000;
}
Result Motion3DModel::setAcsSLimit(cv::Point3f max, cv::Point3f min)
{
    return m_pMotion3D->setAcsSLimit(max, min);
}

Result Motion3DModel::collisionControl()
{
    Result ret;
    ret = PLCController::instance()->collisionControlPreMove();
    return ret;
}
bool Motion3DModel::Motion3DisEnabled()
{
    bool ok = false;
    if (is_Connected)
    {
        ok = m_pMotion3D->IsEnabled();
    }
    return ok;
}
void Motion3DModel::Motion3DSetEnabled(bool b)
{
    if (is_Connected && !isEStop)
    {
        m_pMotion3D->SetEnable(b);
    }
}

Result Motion3DModel::Motion3DMoveRelAsync(int posX, int posY, int posZ)
{
    Result result = collisionControl();
    if (!result.success){
        return result;
    }

    isEStop = false;
    if (is_Connected && !isEStop)
    {
        Result ret = m_pMotion3D->MoveRelAsync(posX, posY, posZ);
        if (isEStop)
        {
            return Result(false, "ACS 3D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop));
}

Result Motion3DModel::Motion3DMoveAbsAsync(int pos, int posY, int posZ)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    isEStop = false;
    if (is_Connected && !isEStop)
    {
        Result ret = m_pMotion3D->MoveAbsAsync(pos, posY, posZ);
        if (isEStop)
        {
            return Result(false, "ACS 3D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop));
}

Result Motion3DModel::Motion3DMoveRelAsync(const char *axis, int pos)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    isEStop = false;
    if (is_Connected && !isEStop)
    {
        Result ret = m_pMotion3D->MoveRelAsync(axis, pos);
        if (isEStop)
        {
            return Result(false, "ACS 3D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop));
}

Result Motion3DModel::Motion3DMoveAbsAsync(const char *axis, int pos)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    isEStop = false;
    if (is_Connected && !isEStop)
    {
        Result ret = m_pMotion3D->MoveAbsAsync(axis, pos);
        if (isEStop)
        {
            return Result(false, "ACS 3D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop));
}

Result Motion3DModel::Motion3DMoveRel(const char *axis, int pos)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    isEStop = false;
    if (is_Connected && !isEStop)
    {
        Result ret = m_pMotion3D->MoveRel(axis, pos);
        if (isEStop)
        {
            return Result(false, "ACS 3D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop));
}

Result Motion3DModel::Motion3DMoveAbs(const char *axis, int pos)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    isEStop = false;
    if (is_Connected && !isEStop)
    {
        Result ret = m_pMotion3D->MoveAbs(axis, pos);
        if (isEStop)
        {
            return Result(false, "ACS 3D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop));
}

Result Motion3DModel::Motion3DMoveRel(int posX, int posY, int posZ)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    isEStop = false;
    if (is_Connected && !isEStop)
    {
        Result ret = m_pMotion3D->MoveRel(posX, posY, posZ);
        if (isEStop)
        {
            return Result(false, "ACS 3D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop));
}

Result Motion3DModel::Motion3DMoveAbs(int posX, int posY, int posZ)
{
    Result result = collisionControl();
    if (!result.success)
    {
        return result;
    }

    Result ret;
    isEStop = false;
    if (is_Connected && !isEStop)
    {
        ML_Point3D curPos = getPosition();
        if (posX != curPos.x)
        {
            m_pMotion3D->MoveAbs(posX, curPos.y, curPos.z);
        }
        if (isEStop)
        {
            return Result(false, "ACS 3D is requested to stop", 1);
        }

        curPos = getPosition();
        if (posY != curPos.y)
        {
            m_pMotion3D->MoveAbs(curPos.x, posY, curPos.z);
        }
        if (isEStop)
        {
            return Result(false, "ACS 3D is requested to stop", 1);
        }

        Result ret = m_pMotion3D->MoveAbs(posX, posY, posZ);
        if (isEStop)
        {
            return Result(false, "ACS 3D is requested to stop", 1);
        }
        return ret;
    }
    return Result(false, std::string("Acs connect status = ") + std::to_string(is_Connected) +
                             std::string(", and EStop = ") + std::to_string(isEStop));
}

Result Motion3DModel::Motion3DMoveStop()
{
    isEStop = true;
    return m_pMotion3D->StopMove();
}
bool Motion3DModel::getEStop()
{
    return isEStop;
}
void Motion3DModel::setEStop(bool val)
{
    isEStop = val;
}
void Motion3DModel::SetSpeed(double speed)
{
    m_pMotion3D->SetSpeed(speed * 1000);
    if (speed > 10.0)
    {
        SetSpeedZ(10.0);
    }
}
bool Motion3DModel::is3DMoving()
{
    return m_pMotion3D->IsMoving();
}

void Motion3DModel::Motion3DSubscribeMotionEvent(MLMotionEvent event)
{
    m_pMotion3D->Subscribe(event, this);
}

void Motion3DModel::Motion3DUnsubscribeMotionEvent(MLMotionEvent event)
{
    m_pMotion3D->Unsubscribe(event, this);
}
Result Motion3DModel::connectAndInit(QString ipstr)
{
    std::string stdstr = ipstr.toStdString();
    Result res = m_pMotion3D->Connect(stdstr.c_str());
    if (res.success)
    {
        is_Connected = true;
        emit updateMotion3DStatusSignal( MLMotionState::kMLStatusConnected);
    }
    return res;
}
bool Motion3DModel::connect(QString ipstr)
{
    std::string stdstr = ipstr.toStdString();
    Result res = m_pMotion3D->Connect(stdstr.c_str());
    if (res.success)
    {
        is_Connected = true;
        emit updateMotion3DStatusSignal(MLMotionState::kMLStatusConnected);
    }
    return res.success;
}
void Motion3DModel::disconnect()
{
    if (is_Connected)
    {
        m_pMotion3D->Disconnect();
        is_Connected = false;
    }

    emit updateMotion3DStatusSignal(MLMotionState::kMLStatusDisConnected);
}
void Motion3DModel::NotifyMotion3DPosition()
{
    CORE::ML_Point3D pos = getPosition();
    emit updateMotion3DPositionSignal(pos.x / 1000.0, pos.y / 1000.0, pos.z / 1000.0);
}

Motion3DModel *Motion3DModel::getInstance(int type)
{
    switch (type)
    {
    case 0: {
        if (selfCamera == nullptr)
        {
            selfCamera = new Motion3DModel(type);
        }
        return selfCamera;
    }
        break;
    case 1: {
            if (selfDUT == nullptr)
            {
                selfDUT = new Motion3DModel(type);
            }

            return selfDUT;
    }
        break;
    }
}
CORE::ML_Point3D Motion3DModel::getPosition()
{
    return m_pMotion3D->GetPosition();
}
void Motion3DModel::NotifyMotion3DStateChanged(MLMotionState old_state, MLMotionState new_state)
{

    emit updateMotion3DStatusSignal(new_state);
}
Result Motion3DModel::acsMovezeroSqeue()
{
    ML_Point3D point = getPosition();
    //Motion3DMoveAbs(0, point.y, point.z);
    //while (is3DMoving())
    //{
    //    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    //}
    return Motion3DMoveAbs(0, 0, 0);
}
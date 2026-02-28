#include "Motion1DModel.h"
#include "MLPIController1D.h"
#include "MLACSController1D.h"
#include "ELLOLib.h"
Motion1DModel* Motion1DModel::self = nullptr;
Motion1DModel::Motion1DModel(QString type)
{
    if (type == "PI") {
        m_pMotion1D = new MLPIController1D;
    }
    else if (type == "ACS") {
        m_pMotion1D = new MLACSController1D;
    }
    else if (type == "Thorlabs") {
        m_pMotion1D = new ELLOMOTOR;
    }

    qRegisterMetaType<MLMotionState>("MLMotionState");
}

Motion1DModel::~Motion1DModel()
{
    if (m_pMotion1D != nullptr) {
        delete m_pMotion1D;
        m_pMotion1D = nullptr;
    }
}
bool Motion1DModel::Motion1DisConnected()
{
    return m_pMotion1D->IsConnected();
}
bool Motion1DModel::Motion1DisEnabled()
{
    return m_pMotion1D->IsEnabled();
}
void Motion1DModel::Motion1DSetEnabled(bool b)
{
   if(is_Connected)
    m_pMotion1D->SetEnable(b);
}

Result Motion1DModel::Motion1DMoveRelAsync(int pos)
{
    if (is_Connected) {
        return m_pMotion1D->MoveRelAsync(pos);
    }
    return Result(false, "Motion1D relative move(async) failed, motion not connected.");
}

Result Motion1DModel::Motion1DMoveAbsAsync(int pos)
{
    if (is_Connected) {
        return m_pMotion1D->MoveAbsAsync(pos);
    }
    return Result(false, "Motion1D absolute move(async) failed, motion not connected.");
}

Result Motion1DModel::Motion1DMoveRel(int pos)
{
    if (isEStop) {
        return Result(false, "Motion 1D is requested to stop", 1);
    }

    if (is_Connected) {
        return m_pMotion1D->MoveRel(pos);
    }
    return Result(false, "Motion1D relative move failed, motion not connected.");
}

Result Motion1DModel::Motion1DMoveAbs(int pos)
{
    if (isEStop) {
        return Result(false, "Motion 1D is requested to stop", 1);
    }
    if (!is_Connected)
    {
        return Result(false, "Motion 1D move abs failed, motion not connectd.");
    }

    Result ret = m_pMotion1D->MoveAbs(pos);
    if(!ret.success){
        return ret;
    }

    while (m_pMotion1D->IsMoving())
    {
        Sleep(100);
    }

    if (isEStop) {
        return Result(false, "Motion 1D is requested to stop", 1);
    }
    return Result();
}

Result Motion1DModel::Motion1DMoveStop()
{
    if (is_Connected) {
        return m_pMotion1D->StopMove();
    }
    return Result(false, "Motion1D move stop failed, motion not connected.");
}

bool Motion1DModel::is1DMoving()
{
    return m_pMotion1D->IsMoving();
}

void Motion1DModel::Motion1DSubscribeMotionEvent(MLMotionEvent event)
{
    m_pMotion1D->Subscribe(event, this);
}

void Motion1DModel::Motion1DUnsubscribeMotionEvent(MLMotionEvent event)
{
    m_pMotion1D->Unsubscribe(event, this);
}
bool Motion1DModel::connect(QString str)
{
    return connectNew(str).success;
}
Result Motion1DModel::connectNew(QString ipstr)
{
    if (!is_Connected) {
        std::string stdstr = ipstr.toStdString();
        Result res = m_pMotion1D->Connect(stdstr.c_str());
        if (res.success) {
            is_Connected = true;
        }
        return res;
    }
    return Result(true, "", -2);
}
void Motion1DModel::disconnect()
{
    if (is_Connected) {
        m_pMotion1D->Disconnect();
        is_Connected = false;
    }
}
Motion1DModel* Motion1DModel::getInstance(QString type)
{
    if (self == nullptr) {
        self = new Motion1DModel(type);
    }
    return self;
}
int Motion1DModel::getPosition() {
    return m_pMotion1D->GetPosition();
}
void Motion1DModel::NotifyMotionStateChanged(MLMotionState old_state, MLMotionState new_state) {
    updateMotion1DStatus(new_state);
    emit updateMotion1DStatusSignal(new_state);
}
void Motion1DModel::NotifyMotionPosition(int x) {
    emit updateMotion1DPositionSignal(x / 1000.0);
}

bool Motion1DModel::getEStop() {
    return isEStop;
}

void Motion1DModel::setEStop(bool val) {
    isEStop = val;
}

Result Motion1DModel::stopMove()
{
    return m_pMotion1D->StopMove();
}

void Motion1DModel::updateMotion1DStatus(MLMotionState status)
{
    if (status == MLMotionState::kMLStatusDisConnected)
    {
        is_Connected = false;
    }
}

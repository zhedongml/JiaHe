#include "MLPIController1D.h"
#include "PIController.h"
#include <iostream>
#include <map>
#include <stdexcept>

#include <windows.h>
#include "PI_GCS2_DLL.h"

MLPIController1D::MLPIController1D()
{
    this->m_State = MLMotionState::kMLStatusDisConnected;
    this->threadStart();
}

MLPIController1D::~MLPIController1D()
{
    this->threadStop();
}

void MLPIController1D::threadStart()
{
    this->m_thread_ = std::thread(&MLPIController1D::threadFunc, this);
}

void MLPIController1D::threadStop()
{
    this->m_runThread_ = false;
    this->m_thread_.join();
}

void MLPIController1D::waitForMotionFinish()
{
    BOOL isMovingX = TRUE;
    while (isMovingX)
    {
        if (!PI_IsMoving(m_ControllerID, m_Axis.c_str(), &isMovingX))
        {
            int error_code = PI_GetError(m_ControllerID);
            LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "isMoving call failed, error: " << error_code);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

Result MLPIController1D::Connect(const char *type_name)
{
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Connection initializing...");
    string str(type_name);

    std::vector<string> strlist = split(str, '/');

    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "PI Controller :" << strlist[0].c_str());

    m_ControllerID = PIController::GetInstance().OpenConnection(
        ConnectionType::USB, /*"C884"*/ strlist[0].c_str()); //-1 if not connected, or failed

    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "PI ControllerID :" << m_ControllerID);

    if (m_ControllerID < 0)
    {
        return Result(false, "error");
    }
    if (strlist.size() > 1)
    {
        m_Axis = strlist[1];
    }

    int servo = 1;
    int state = 1; // SERVO_ON
    if (!PI_SVO(m_ControllerID, m_Axis.c_str(), &state))
    {
        return Result(false, "PI SVO failed. Exiting.");
    }

    double velocityX = m_VelocityNormalX / m_Scale;
    PI_VEL(m_ControllerID, m_Axis.c_str(), &velocityX);

    if (strlist.size() > 2 && strlist[2].find("NL") != string::npos)
    {
        if (!PI_FNL(m_ControllerID, m_Axis.c_str())) // 0--PI_FNL  0-100 PI_FRF
        {
            return Result(false, "PI Unable to reference X-axis.");
        }
    }
    else if (strlist.size() > 2 && strlist[2].find("PL") != string::npos)
    {
        if (!PI_FPL(m_ControllerID, m_Axis.c_str())) // 0--PI_FNL  0-100 PI_FRF
        {
            return Result(false, "PI Unable to reference X-axis.");
        }
    }
    else
    {
        if (!PI_FRF(m_ControllerID, m_Axis.c_str())) // 0--PI_FNL  0-100 PI_FRF
        {
            return Result(false, "PI Unable to reference X-axis.");
        }
    }

    BOOL bRefDone = FALSE;

    while (!bRefDone)
    {
        if (!PI_IsControllerReady(m_ControllerID, &bRefDone))
        {
            return Result(false, "PI Unable to retrieve controller ready state.");
        }
    }

    BOOL bReferenced;

    // check if Xaxis was referenced
    if (!PI_qFRF(m_ControllerID, m_Axis.c_str(), &bReferenced))
    {
        int error_code = PI_GetError(m_ControllerID);
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger,
                        "Referencing X axis failed in Connect(), error code: " << error_code);
        return Result(false, "PI Unable to retrieve referenced state.");
    }

    if (!bReferenced)
    {
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Referencing finished, but stage still not referenced");
    }
    else
    {
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Referencing finished!");
    }

    this->m_State = MLMotionState::kMLStatusConnected;
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Connection successful!");
    return Result(true, "success");
}

bool MLPIController1D::Disconnect()
{
    if (PI_IsConnected(m_ControllerID))
    {
        // set servo off
        int state = 0; // SERVO_OFF
        if (!PI_SVO(m_ControllerID, m_Axis.c_str(), &state))
        {
            int error_code = PI_GetError(m_ControllerID);
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Disconnection failed, error code: " << error_code);
        }
        PI_CloseConnection(m_ControllerID);
        this->m_State = MLMotionState::kMLStatusDisConnected;
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Disconnected!");
        return true;
    }
    return false;
}

bool MLPIController1D::IsConnected()
{
    if (PI_IsConnected(m_ControllerID))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MLPIController1D::SetEnable(bool enable)
{
    this->isEnable = enable;
}

bool MLPIController1D::IsEnabled()
{
    return this->isEnable;
}

Result MLPIController1D::MoveRelAsync(int posX)
{
    double target = posX / m_Scale;
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Moving relative (async) to " << posX << "with target: " << target);
    if (!PI_MVR(m_ControllerID, m_Axis.c_str(), &target))
    {
        int error_code = PI_GetError(m_ControllerID);
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Moving relative (async) failed with err code: " << error_code);
        if (error_code == 7)
        {
            throw std::runtime_error("Movement out of range");
        }
        return Result(false, "Moving relative (async) failed with err code: " + error_code);
    }
    else
    {
        this->m_State = MLMotionState::kMLStatusIsMoving;
        return Result();
    }
}

Result MLPIController1D::MoveAbsAsync(int posX)
{
    double target = posX / m_Scale;
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Moving absolute (async) to " << posX << "with target: " << target);
    if (!PI_MOV(m_ControllerID, m_Axis.c_str(), &target))
    {
        int error_code = PI_GetError(m_ControllerID);
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Moving absolute (async) failed with err code: " << error_code);
        if (error_code == 7)
        {
            throw std::runtime_error("Movement out of range");
        }
        return Result(false, "Moving absolute (async) failed with err code: " + error_code);
    }
    else
    {
        this->m_State = MLMotionState::kMLStatusIsMoving;
        return Result();
    }
}

Result MLPIController1D::MoveRel(int posX) // sync'd method, implement later
{
    double target = posX / m_Scale;
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Moving relative (sync) to " << posX << "with target: " << target);
    if (!PI_MVR(m_ControllerID, m_Axis.c_str(), &target))
    {
        int error_code = PI_GetError(m_ControllerID);
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Moving relative (sync) failed with err code: " << error_code);
        if (error_code == 7)
        {
            throw std::runtime_error("Movement out of range");
        }
        return Result(false, "Moving relative (sync) failed with err code: " + error_code);
    }
    else
    {
        this->m_State = MLMotionState::kMLStatusIsMoving;
    }
    waitForMotionFinish();
    return Result();
}

Result MLPIController1D::MoveAbs(int posX) // sync'd method, implement later
{
    double target = posX / m_Scale;
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Moving absolute (sync) to " << posX << "with target: " << target);
    if (!PI_MOV(m_ControllerID, m_Axis.c_str(), &target))
    {
        int error_code = PI_GetError(m_ControllerID);
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Moving absolute (sync) failed with err code: " << error_code);
        if (error_code == 7)
        {
            throw std::runtime_error("Movement out of range");
        }
        return Result(false, "Moving absolute (sync) failed with err code: " + error_code);
    }
    else
    {
        this->m_State = MLMotionState::kMLStatusIsMoving;
    }
    waitForMotionFinish();
    return Result();
}

Result MLPIController1D::StopMove()
{
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Stopping stage movement...");
    if (!PI_HLT(m_ControllerID, m_Axis.c_str()))
    {
        int error_code = PI_GetError(m_ControllerID);
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Stopping stage movement failed with err code: " << error_code);
        throw std::runtime_error("stopping movement error");
        return Result(false, "Stopping stage movement failed with err code: " + error_code);
    }
    else
    {
        this->m_State = MLMotionState::kMLStatusStationary;
    }
    return Result();
}

Result MLPIController1D::Homing()
{
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Stage going to home position");
    if (this->IsHome())
    {
        // logging
        return Result();
    }
    if (!PI_GOH(m_ControllerID, m_Axis.c_str()))
    {
        int error_code = PI_GetError(m_ControllerID);
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Homing operation failed with err code: " << error_code);
        return Result(false, "Homing operation failed with err code: " + error_code);
    }
    else
    {
        this->m_State = MLMotionState::kMLStatusIsMoving;
    }
    return Result();
}

bool MLPIController1D::IsHome()
{
    // TODO:  checking if home has pos == 0, then this method should be straight forward
    return (this->GetPosition() == 0);
}

void MLPIController1D::SetSpeed(int speedX)
{
    double velX = speedX / m_Scale;
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Setting speed to: " << velX << "with param: " << speedX);
    if (!PI_VEL(m_ControllerID, m_Axis.c_str(), &velX))
    {
        int error_code = PI_GetError(m_ControllerID);
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Setting speed failed with err code: " << error_code);
    }
}

int MLPIController1D::GetSpeed()
{
    double v = 0.0;
    if (!PI_qVEL(m_ControllerID, m_Axis.c_str(), &v)) // get velocity city commanded with VEL
    {
        int error_code = PI_GetError(m_ControllerID);
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Getting speed failed with err code: " << error_code);
    }
    return int(v);
}

bool MLPIController1D::IsMoving()
{
    BOOL isMovingX = TRUE; // default value
    PI_IsMoving(m_ControllerID, m_Axis.c_str(), &isMovingX);

    if (isMovingX == TRUE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int MLPIController1D::GetPosition()
{
    double posX = 0.0;
    if (!PI_qPOS(m_ControllerID, m_Axis.c_str(), &posX))
    {
        int error_code = PI_GetError(m_ControllerID);
        // LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Getting position failed with err code: " << error_code);
    }
    posX = posX * 1000.0;
    posX = posX + 0.5 - (posX < 0);

    return int(posX);
}

MLMotionState MLPIController1D::GetState()
{
    if (m_State != MLMotionState::kMLStatusDisConnected)
    {
        if (!PI_IsConnected(m_ControllerID))
        {
            m_State = MLMotionState::kMLStatusDisConnected;
        }
    }
    return m_State;
}

void MLPIController1D::Subscribe(MLMotionEvent event, CoreMotionCallback *callback)
{
    if (event == MLMotionEvent::kStateChanged || event == MLMotionEvent::kPositionChanged)
    {
        if (m_callbacks.count(event) > 0)
        {
            m_callbacks[event].push_back(callback);
            LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "callback subscribed for" << event);
        }
        else
        {
            std::list<CoreMotionCallback *> l;
            l.push_back(callback);
            m_callbacks[event] = l;
        }
        return;
    }
    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Callback functions do not support this type of event");
}

void MLPIController1D::Unsubscribe(MLMotionEvent event, CoreMotionCallback *callback)
{
    if (m_callbacks.count(event) > 0) // found key
    {
        std::list<CoreMotionCallback *> ll = m_callbacks[event];
        ll.remove(callback);
    }
}

void MLPIController1D::invokeStatesChangedCB(MLMotionState old_state, MLMotionState new_state)
{
    std::list<CoreMotionCallback *> cb_list = m_callbacks[MLMotionEvent::kStateChanged];
    std::list<CoreMotionCallback *>::iterator it;
    for (it = cb_list.begin(); it != cb_list.end(); it++)
    {
        (*it)->NotifyMotionStateChanged(old_state, new_state);
    }
}

void MLPIController1D::notifyPosition()
{
    std::map<MLMotionEvent, std::list<CoreMotionCallback *>>::iterator it;
    for (it = m_callbacks.begin(); it != m_callbacks.end(); it++)
    {
        if (it->second.size() == 0)
        {
            continue;
        }

        // it->second.front()->NotifyMotionPosition(this->GetPosition());
        it->second.back()->NotifyMotionPosition(this->GetPosition());
    }
}

void MLPIController1D::threadFunc()
{
    this->m_runThread_ = true;
    MLMotionState prevState = this->GetState();
    while (this->m_runThread_)
    {
        MLMotionState newState = this->GetState();
        if ((prevState == MLMotionState::kMLStatusIsMoving) && (!this->IsMoving()))
        {
            newState = MLMotionState::kMLStatusStationary;
        }
        if (prevState != newState)
        {
            if (this->m_callbacks.count(MLMotionEvent::kStateChanged) > 0)
            {
                this->invokeStatesChangedCB(prevState, newState);
            }
            prevState = newState;
            this->m_State = newState;
        }
        std::list<CoreMotionCallback *> cb_list = m_callbacks[MLMotionEvent::kStateChanged];
        std::list<CoreMotionCallback *> posCB_list = m_callbacks[MLMotionEvent::kPositionChanged];
        std::list<CoreMotionCallback *>::iterator it;
        if (IsConnected())
        {
            int pos = GetPosition();
            for (it = posCB_list.begin(); it != posCB_list.end(); it++)
            {
                (*it)->NotifyMotionPosition(pos);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

CoreMotion *MLPIController1D::createInstance()
{
    return new MLPIController1D();
}

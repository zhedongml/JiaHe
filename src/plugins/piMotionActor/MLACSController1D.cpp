#include "MLACSController1D.h"
#include <stdexcept>
#include <map>
#include <iostream>


MLACSController1D::MLACSController1D()
{
    this->m_State = MLMotionState::kMLStatusDisConnected;
    threadStart();
}

MLACSController1D::~MLACSController1D()
{
    this->threadStop();
}

void MLACSController1D::threadStart()
{
    this->m_thread_ = std::thread(&MLACSController1D::threadFunc, this);
}

void MLACSController1D::threadStop()
{
    if (m_runThread_) {
        this->m_runThread_ = false;
        this->m_thread_.join();
    }
}

Result MLACSController1D::InitialMotionSystem()
{
    Result initResult = judgeInitBuffer();
    if (initResult.success) {
        return initResult;
    }

    if (!acsc_RunBuffer(m_AcsHandle, 2, NULL, NULL))
    {
        string msg = "ACS 1D motion Initialize failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());;
        return Result(false, msg);
    }
    acsc_WaitProgramEnd(m_AcsHandle, 2, 80000);

    return judgeInitBuffer();
}

std::string MLACSController1D::GetErrorMessage()
{
    int recvCnt = 0;
    char errString[101];
    string err;

    // Get last error message from ACS library (Result of library command)
    // - If ACS got an error in the buffer, this function cannot get the error code
    int errCode = acsc_GetLastError();

    if (m_AcsHandle)
    {
        // Get Error string from ACS controller by error code
        acsc_GetErrorString(m_AcsHandle, errCode, errString, 100, &recvCnt);
        errString[recvCnt] = '\0';
    }
    if (recvCnt > 0)
    {
        err = "Error code: " + std::to_string(errCode) + ", error message: " + errString;
    }
    else
    {
        err = "Error code: " + std::to_string(errCode) + ", error message: Unknown error!";
    }

//TODO: to be test
    if (errCode == ACSC_CLOSED_BY_CONTROLLER || ACSC_INITFAILURE == errCode || ACSC_INVALIDHANDLE == errCode)
    {
        Disconnect();
    }
    return err;
}

std::vector<std::string> MLACSController1D::split(std::string str, char a)
{
    vector<string> strvec;

    string::size_type pos1, pos2;
    pos2 = str.find(a);
    pos1 = 0;
    while (string::npos != pos2)
    {
        strvec.push_back(str.substr(pos1, pos2 - pos1));

        pos1 = pos2 + 1;
        pos2 = str.find(a, pos1);
    }
    strvec.push_back(str.substr(pos1));
    return strvec;
}

void MLACSController1D::waitForMotionFinish()
{
    BOOL isMovingX = TRUE;
    while (IsMoving())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

Result MLACSController1D::Connect(const char* type_name)
{
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Connection initializing...");
    if (m_IsConnected)return Result();
    if (m_AcsHandle != ACSC_INVALID)return Result();
    string info(type_name);
    vector<string> list = split(info, ':');
    const char* ip = list[0].c_str();

    int port = std::stoi(list[1]);

    m_AcsHandle = acsc_OpenCommEthernetTCP(const_cast<char*>(ip), port);
    if (m_AcsHandle == ACSC_INVALID)
    {
        string msg = "ACS system connect failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    double SysInfo = 0.0;
    acsc_SysInfo(m_AcsHandle, 13, &SysInfo, NULL);
    m_TotalAxes = (int)SysInfo;

    if (m_AcsHandle == ACSC_INVALID)
    {
        string msg = "ACS 1D motion connect failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    Result err = InitialMotionSystem();
    if (!err.success)
    {
        Disconnect();
        return err;
    }
    m_IsConnected = true;
    SetEnable(true);
    isEnable = true;
    m_State = MLMotionState::kMLStatusConnected;
    return Result();
}

bool MLACSController1D::Disconnect()
{
    if (m_AcsHandle != ACSC_INVALID)
    {
        //SetEnable(false);
        //if (!isEnable)
        {
            if (acsc_CloseComm(m_AcsHandle))
            {
                m_AcsHandle = ACSC_INVALID;
            }
            else
            {
                string msg = "ACS system disconnect failed: " + GetErrorMessage();
                LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
            }
            m_IsConnected = false;
            m_State = MLMotionState::kMLStatusDisConnected;
            m_AcsHandle = ACSC_INVALID;
        }
        invokeStatesChangedCB(MLMotionState::kMLStatusConnected, MLMotionState::kMLStatusDisConnected);
        return true;
    }

    m_IsConnected = false;
    m_State = MLMotionState::kMLStatusDisConnected;
    return true;
}

bool MLACSController1D::IsConnected()
{
    return m_IsConnected;
}

void MLACSController1D::SetEnable(bool enable)
{
    if (enable)
    {
        //if (!isEnable)
        {
            if (!acsc_Enable(m_AcsHandle, m_Axis, NULL))
            {
                string msg = "ACS 1D motion enable failed: " + GetErrorMessage();
                LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
                isEnable = false;
            }else{
                acsc_WaitMotorEnabled(m_AcsHandle, m_Axis, 1, 3000);
                isEnable = true;
            }
        }
    }
    else
    {
        //if (isEnable)
        {
            if (!acsc_Disable(m_AcsHandle, m_Axis, NULL))
            {
                string msg = "ACS 1D motion disable failed: " + GetErrorMessage();
                LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
            }
            isEnable = enable;
        }
    }
}

bool MLACSController1D::IsEnabled()
{
    return this->isEnable;
}

Result MLACSController1D::MoveRelAsync(int posX)
{
    if (IsMoving())
    {
        string msg = "The relative moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    double position = posX / m_Scale;
    int a = acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, m_Axis, position, NULL);
    if (!a)
    {
        string msg = "ACS 1D motion relative moving failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    return Result();
}

Result MLACSController1D::MoveAbsAsync(int posX)
{
    if (IsMoving())
    {
        string msg = "The absolute moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    double postion =  posX / m_Scale;
    if (!acsc_ToPoint(m_AcsHandle, 0, m_Axis, postion, NULL))
    {
        string msg = "ACS 1D motion absolute moving failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    return Result();
}

Result MLACSController1D::MoveRel(int posX) // sync'd method, implement later
{
    if (IsMoving())
    {
        string msg = "The relative moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    double position = posX / m_Scale;
    int a = acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, m_Axis, position, NULL);
    if (!a)
    {
        string msg = "ACS 1D motion relative moving failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    acsc_WaitMotionEnd(m_AcsHandle, m_Axis, 3000);
    return Result();
}

Result MLACSController1D::MoveAbs(int posX) // sync'd method, implement later
{
    if (IsMoving())
    {
        string msg = "The absolute moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    double postion = posX / m_Scale;
    if (!acsc_ToPoint(m_AcsHandle, 0, m_Axis, postion, NULL))
    {
        string msg = "ACS 1D motion absolute moving failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    acsc_WaitMotionEnd(m_AcsHandle, m_Axis, 3000);
    return Result();
}

Result MLACSController1D::StopMove()
{
    if (!acsc_Halt(m_AcsHandle, m_Axis, NULL))
    {
        string msg = "ACS 1D motion stop moving failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    return Result();
}

Result MLACSController1D::Homing()
{
    if (m_IsConnected)
    {
        Result err = InitialMotionSystem();
        if (!err.success)
        {
            string msg = "ACS 1D motion homing failed: " + GetErrorMessage();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
            return Result(false, msg);
        }
        return Result();
    }
    return Result(false, "ACS 1D motion homing failed, motion not connected.");
}

bool MLACSController1D::IsHome()
{
    return (this->GetPosition() == 0);
}

void MLACSController1D::SetSpeed(int speedX)
{
    double s = speedX / m_Scale;
    if (!acsc_SetVelocity(m_AcsHandle, m_Axis, s, NULL))
    {
        string msg = "ACS 1D motion set X axis Velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    }
}

int MLACSController1D::GetSpeed()
{
    double speed;
    if (!acsc_GetVelocity(m_AcsHandle, m_Axis, &speed, NULL))
    {
        string msg = "ACS 1D motion get velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return 0;
    }
    else
    {
        return int(speed * m_Scale);
    }
}

bool MLACSController1D::IsMoving()
{
    BOOL isMovingX = false; //default value
    int state;
    if (acsc_GetAxisState(m_AcsHandle, m_Axis, &state, NULL))
    {
        if (state & ACSC_MST_MOVE)
        {
            isMovingX = true;
        }
    }
    return isMovingX;
}

int MLACSController1D::GetPosition()
{
    if (!m_IsConnected) {
        return 0;
    }

    double pos;
    if (acsc_GetFPosition(m_AcsHandle, m_Axis, &pos, NULL))
    {
        return round(pos * m_Scale);
    }
    else
    {
        string msg = "ACS 1D motion get x position failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return 0;
    }
}

MLMotionState MLACSController1D::GetState()
{
    return m_State;
}

void MLACSController1D::Subscribe(MLMotionEvent event, CoreMotionCallback* callback)
{
    if (event == MLMotionEvent::kStateChanged ||event ==MLMotionEvent::kPositionChanged)
    {
        if (m_callbacks.count(event) > 0) {
            m_callbacks[event].push_back(callback);
            LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "callback subscribed for" << event);
        }
        else {
            std::list<CoreMotionCallback*> l;
            l.push_back(callback);
            m_callbacks[event] = l;
        }
        return;
    }
    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Callback functions do not support this type of event");
}

void MLACSController1D::Unsubscribe(MLMotionEvent event, CoreMotionCallback* callback)
{
    if (m_callbacks.count(event)> 0) //found key
    {
        std::list<CoreMotionCallback*> ll = m_callbacks[event];
        ll.remove(callback);
    }
}

void MLACSController1D::invokeStatesChangedCB(MLMotionState old_state, MLMotionState new_state)
{
    std::list<CoreMotionCallback* > cb_list = m_callbacks[MLMotionEvent::kStateChanged];
    std::list<CoreMotionCallback*>::iterator it;
    for (it = cb_list.begin(); it != cb_list.end(); it++)
    {
        (*it)->NotifyMotionStateChanged(old_state, new_state);
    }
}

void MLACSController1D::threadFunc()
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
        std::list<CoreMotionCallback* > cb_list = m_callbacks[MLMotionEvent::kStateChanged];
        std::list<CoreMotionCallback* > posCB_list = m_callbacks[MLMotionEvent::kPositionChanged];
        std::list<CoreMotionCallback*>::iterator it;
        int pos = GetPosition();
        for (it = posCB_list.begin(); it != posCB_list.end(); it++)
        {
            (*it)->NotifyMotionPosition(pos);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

CoreMotion* MLACSController1D::createInstance()
{
    return new MLACSController1D();
}

Result MLACSController1D::judgeInitBuffer()
{
    int AnalogInputs[1] = { 0 };
    if (!acsc_ReadInteger(m_AcsHandle,
        ACSC_NONE,
        (char*)"MFLAGS",
        0, 0,
        ACSC_NONE, ACSC_NONE,
        AnalogInputs,
        NULL
    ))
    {
        string msg = "Get state of ACS 1D motion Initialize failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!(AnalogInputs[0] & 8)) {
        acsc_StopBuffer(m_AcsHandle, 2, NULL);
        return Result(false, "ACS 1D motion, Buffer 2 init error.");
    }
    return Result();
}


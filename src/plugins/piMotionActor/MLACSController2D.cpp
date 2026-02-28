#include "MLACSController2D.h"

MLACSCController2D::MLACSCController2D(int type)
{
    if (type == 2)
    {
        setBufFlag();
    }
    m_IsHome = false;
    isEnable = false;
    m_IsConnected = false;
    m_CallbackFlag = false;
    m_TotalAxes = 0;
    m_AcsHandle = ACSC_INVALID;
    m_State = MLMotionState::kMLStatusDisConnected;
}

MLACSCController2D::~MLACSCController2D()
{
    if (m_IsConnected)
    {
        Disconnect();
        m_State = MLMotionState::kMLStatusDisConnected;
    }
    if (m_CallbackFlag)
    {
        m_CallbackFlag = false;
        m_CallbackHandle.join();
    }
    m_Callbacks.clear();
}

Result MLACSCController2D::Connect(const char *type_name)
{
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Connection initializing...");

    if (m_IsConnected || m_AcsHandle != ACSC_INVALID)
    {
        SetEnable(true);
        return Result();
    }
    string info(type_name);
    vector<string> listMotion = Split(type_name, '\\');
    vector<string> list = Split(listMotion[0], '/');
    int index = 1;
    numBuffer = {};

    if (listMotion.size() > 1)
    {
        for (int j = 1; j < listMotion.size(); j++)
        {
            numBuffer.append(atoi(listMotion[index].c_str()));
            index++;
        }
    }

    const char *ip = list[0].c_str();
    int port = std::stoi(list[1]);

    X = std::stoi(list[2]);
    Y = std::stoi(list[3]);
    Result ret = setDirection(list);
    if (!ret.success)
    {
        return Result(false, "ACS 2D motion connect failed," + ret.errorMsg);
    }

    m_AcsHandle = acsc_OpenCommEthernetTCP(const_cast<char *>(ip), port);
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
        string msg = "ACS 2D motion connect failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    SetEnable(true);
    if (m_bufFlag)
    {
        Result err = InitialMotionSystem();
        if (!err.success)
        {
            Disconnect();
            return Result(false, err.errorMsg);
         }
    }
    m_IsConnected = true;
    isEnable = true;
    m_State = MLMotionState::kMLStatusConnected;
    return Result();
}

void MLACSCController2D::setBufFlag()
{
    m_bufFlag = true;
}

bool MLACSCController2D::Disconnect()
{
    if (m_AcsHandle != ACSC_INVALID)
    {
        // SetEnable(false);
        // if (!m_Enable)
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

        m_IsConnected = false;
        return true;
    }
    m_IsConnected = false;
    return false;
}

bool MLACSCController2D::IsConnected()
{
    return m_IsConnected;
}

void MLACSCController2D::SetEnable(bool enable)
{
    if (enable)
    {
        int AxisList[] = {Y, X, -1};
        //if (!isEnable)
        {
            if (!acsc_EnableM(m_AcsHandle, AxisList, NULL))
            {
                string msg = "ACS 2D motion enable failed: " + GetErrorMessage();
                LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
            }
            acsc_WaitMotorEnabled(m_AcsHandle, X, 1, 3000);
            isEnable = enable;
        }
    }
    else
    {
        int AxisList[] = {Y, X, -1};
        //if (isEnable)
        {
            if (!acsc_DisableM(m_AcsHandle, AxisList, NULL))
            {
                string msg = "ACS 2D motion disable failed: " + GetErrorMessage();
                LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
            }
            isEnable = enable;
        }
    }
}

bool MLACSCController2D::IsEnabled()
{
    return isEnable;
}

Result MLACSCController2D::MoveRelAsync(double posX, double posY)
{
    if (IsMoving())
    {
        string msg = "The relative moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    int AxisList[] = {Y, X, -1};
    double PosList[] = { updateDirection(Y, posY), updateDirection(X, posX), -1};
    int a = acsc_ToPointM(m_AcsHandle, ACSC_AMF_RELATIVE, AxisList, PosList, NULL);
    if (!a)
    {
        string msg = "ACS 2D motion relative moving failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    return Result();
}

Result MLACSCController2D::MoveAbsAsync(double posX, double posY)
{
    if (IsMoving())
    {
        string msg = "The absolute moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    int AxisList[] = {Y, X, -1};
    double PosList[] = { updateDirection(Y, posY), updateDirection(X, posX), -1};
    if (!acsc_ToPointM(m_AcsHandle, 0, AxisList, PosList, NULL))
    {
        string msg = "ACS 2D motion absolute moving failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    return Result();
}

Result MLACSCController2D::MoveRel(double posX, double posY)
{
    if (IsMoving())
    {
        string msg = "The relative moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    ML_Point posCurrent = GetPosition();

    //int AxisList[] = {Y, X, -1};
    //double PosList[] = { updateDirection(Y, posY), updateDirection(X, posX), -1};
    //if (!acsc_ToPointM(m_AcsHandle, ACSC_AMF_RELATIVE, AxisList, PosList, NULL))
    //{
    //    string msg = "ACS 2D motion relative moving failed: " + GetErrorMessage();
    //    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    //    return Result(false, msg);
    //}

    if (!acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, X, updateDirection(X, posX), NULL))
    {
        string msg = "ACS 2D motion relative moving X failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, Y, updateDirection(Y, posY), NULL))
    {
        string msg = "ACS 2D motion relative moving Y failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    acsc_WaitMotionEnd(m_AcsHandle, X, 5000);
    acsc_WaitMotionEnd(m_AcsHandle, Y, 5000);
    while (IsMoving())
    {
        Sleep(50);
    }

    Result ret = judgePos(updateDirection(X, posCurrent.x + posX * m_Scale), updateDirection(Y, posCurrent.y + posY * m_Scale));
    return ret;
}

Result MLACSCController2D::MoveAbs(double posX, double posY)
{
    if (IsMoving())
    {
        string msg = "The absolute moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    //int AxisList[] = {Y, X,-1};
    //double PosList[] = { updateDirection(Y, posY) , updateDirection(X, posX) ,-1};
    //if (!acsc_ToPointM(m_AcsHandle, 0, AxisList, PosList, NULL))
    //{
    //    string msg = "ACS 2D motion absolute moving failed: " + GetErrorMessage();
    //    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    //    return Result(false, msg);
    //}

    //int AxisList[] = { X, Y,-1 };
    //double PosList[] = { updateDirection(X, posX) ,updateDirection(Y, posY) , -1 };
    //if (!acsc_ToPointM(m_AcsHandle, 0, AxisList, PosList, NULL))
    //{
    //    string msg = "ACS 2D motion absolute moving failed: " + GetErrorMessage();
    //    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    //    return Result(false, msg);
    //}

    if (!acsc_ToPoint(m_AcsHandle, 0, X, updateDirection(X, posX), NULL))
	{
		string msg = "ACS 2D motion absolute moving X failed: " + GetErrorMessage();
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
		return Result(false, msg);
    }

    if (!acsc_ToPoint(m_AcsHandle, 0, Y, updateDirection(Y, posY), NULL))
    {
        string msg = "ACS 2D motion absolute moving Y failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    acsc_WaitMotionEnd(m_AcsHandle, X, 5000);
    acsc_WaitMotionEnd(m_AcsHandle, Y, 5000);
    while (IsMoving())
    {
        Sleep(50);
    }

    Result ret = judgePos(updateDirection(X, posX * m_Scale), updateDirection(Y, posY * m_Scale));
    return ret;
}

Result MLACSCController2D::StopMove()
{
    int AxisList[] = {Y, X, -1};
    if (!acsc_HaltM(m_AcsHandle, AxisList, NULL))
    {
        string msg = "ACS 2D motion stop moving failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    return Result();
}

Result MLACSCController2D::Homing()
{
    if (m_IsConnected)
    {
        // for (int i = 0; i < m_TotalAxes;i++)
        //{
        //    std::string buf = "HOME "+std::to_string(i)+",1,10";
        //    char* command = const_cast<char*>(buf.c_str());
        //    if (!acsc_Command(m_AcsHandle, command,buf.size() , NULL)) return ACSCErrorExecFailed;
        //}
        Result err = InitialMotionSystem();
        if (!err.success)
        {
            string msg = "ACS 2D motion homing failed: " + GetErrorMessage();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
            m_IsHome = false;
            return Result(false, msg);
        }
        m_IsHome = true;
    }
    return Result();
}

Result MLACSCController2D::homingForced()
{
    for (int j = 0; j < numBuffer.size(); j++)
    {
        if (!acsc_RunBuffer(m_AcsHandle, numBuffer[j], NULL, NULL))
        {
            string msg = "ACS 2D motion homing failed: " + GetErrorMessage();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
            return Result(false, msg);
        }
        acsc_WaitProgramEnd(m_AcsHandle, numBuffer[j], 30000);
        while (IsMoving())
        {
            Sleep(50);
        }

        for (int i = 0; i < 12; ++i)
        {
            Sleep(500);
            while (IsMoving())
            {
                Sleep(50);
            }

            bool flag;
            int type;
            Result ret = IsHome(flag, type);
            if (ret.success)
            {
                break;
            }
        }
    }

    return Result();
}

Result MLACSCController2D::IsHome(bool &flag,int type)
{
    if (!m_IsConnected)
    {
        return Result(false, "Motion 2D is not connected.");
    }

    // ACS2DPro
    if (type == 1)
    {
        return Result(true, "Motion 2D is not need homing.");
    }

    int AnalogInputs[3] = {0, 0, 0};
    if (acsc_ReadInteger(m_AcsHandle, ACSC_NONE, (char *)"MFLAGS", 0, 1, ACSC_NONE, ACSC_NONE, AnalogInputs, NULL))
    {
        /*int i = AnalogInputs[0];
        vector<int> vn;
        while (i) {
            vn.push_back(i % 2);
            i /= 2;
        }
        if (vn.size() >= 4) {
            int nValue = vn[3];
            if (1 == nValue)
                return true;
        }*/
    }
    else
    {
        string msg = "Get state of ACS 2D motion Initialize failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    string msg;
    if (!(AnalogInputs[1] & 8) || !(AnalogInputs[0] & 8))
    {
        flag = false;
        msg = "Havn't homing";
    }
    else
    {
        flag = true;
        msg = "Already homing";
    }

    return Result(flag,msg);
}

void MLACSCController2D::SetSpeed(int speed)
{
    double s = speed*1.0;
    if (!acsc_SetVelocity(m_AcsHandle, X, s, NULL))
    {
        string msg = "ACS 2D motion set X axis Velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    }
    if (!acsc_SetVelocity(m_AcsHandle, Y, s, NULL))
    {
        string msg = "ACS 2D motion set Y axis Velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    }
}

int MLACSCController2D::GetSpeed()
{
    double speed;
    if (!acsc_GetVelocity(m_AcsHandle, X, &speed, NULL))
    {
        string msg = "ACS 2D motion get velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return 0;
    }
    else
    {
        return int(speed * m_Scale);
    }
}

bool MLACSCController2D::IsMoving()
{
    int state;
    bool moving = false;
    if (acsc_GetAxisState(m_AcsHandle, X, &state, NULL))
    {
        if (state & ACSC_MST_MOVE)
        {
            moving = true;
        }
    }
    if (acsc_GetAxisState(m_AcsHandle, Y, &state, NULL))
    {
        if (state & ACSC_MST_MOVE)
        {
            moving = true;
        }
    }
    return moving;
}

ML_Point MLACSCController2D::GetPosition()
{
    if (!m_IsConnected)
    {
        ML_Point tmp;
        tmp.x = 0;
        tmp.y = 0;
        return tmp;
    }

    ML_Point positions;
    double pos;
    if (acsc_GetFPosition(m_AcsHandle, X, &pos, NULL))
    {
        positions.x = round(pos * m_Scale);
    }
    else
    {
        string msg = "ACS 2D motion get X position failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        positions.x = 0;
    }

    if (acsc_GetFPosition(m_AcsHandle, Y, &pos, NULL))
    {
        positions.y = round(pos * m_Scale);
    }
    else
    {
        string msg = "ACS 2D motion get Y position failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        positions.y = 0;
    }

    positions.x = updateDirection(X, positions.x);
    positions.y = updateDirection(Y, positions.y);
    return positions;
}

MLMotionState MLACSCController2D::GetState()
{
    if (IsMoving())
    {
        m_State = MLMotionState::kMLStatusIsMoving;
    }
    else
    {
        m_State = MLMotionState::kMLStatusStationary;
    }
    return m_State;
}

void MLACSCController2D::Subscribe(MLMotionEvent event, MLMotion2DCallback *callback)
{
    if (!m_Callbacks.count(event))
    {
        m_Callbacks[event] = callback;
        if (!m_CallbackFlag)
        {
            m_CallbackHandle = std::thread(&MLACSCController2D::threadFunc, this);
            m_CallbackFlag = true;
        }
    }
}

void MLACSCController2D::Unsubscribe(MLMotionEvent event, MLMotion2DCallback *callback)
{
    if (m_Callbacks.count(event))
    {
        if (m_CallbackFlag)
        {
            m_CallbackFlag = false;
            m_CallbackHandle.join();
            m_Callbacks.erase(event);
        }
    }
}

void MLACSCController2D::clearAlarm()
{
}

void MLACSCController2D::SetSpeedX(double speed)
{
    if (!acsc_SetVelocity(m_AcsHandle, X, speed, NULL))
    {
        string msg = "ACS 2D motion set X axis Velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    }
}

void MLACSCController2D::SetSpeedY(double speed)
{
    if (!acsc_SetVelocity(m_AcsHandle, Y, speed, NULL))
    {
        string msg = "ACS 2D motion set Y axis Velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    }
}

void MLACSCController2D::RegisterEmergencyStop()
{
    acsc_RegisterEmergencyStop();
}

void MLACSCController2D::UnregisterEmergencyStop()
{
    acsc_UnregisterEmergencyStop();
}

std::vector<std::string> MLACSCController2D::Split(std::string str, char a)
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

std::string MLACSCController2D::GetErrorMessage()
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

    // TODO: to be test
    if (errCode == ACSC_CLOSED_BY_CONTROLLER || ACSC_INITFAILURE == errCode || ACSC_INVALIDHANDLE == errCode)
    {
        Disconnect();
    }
    return err;
}
Result MLACSCController2D::InitialMotionSystem()
{
    //if (!acsc_StopBuffer(m_AcsHandle, 1, NULL) && !acsc_StopBuffer(m_AcsHandle, 2, NULL))
    //{

    //    string msg = "Stop buffer failed: " + GetErrorMessage();
    //    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    //    return Result(false, msg);
    //}
    int AnalogInputs[3] = {0, 0, 0};
    if (acsc_ReadInteger(m_AcsHandle, ACSC_NONE, (char *)"MFLAGS", 0, 1, ACSC_NONE, ACSC_NONE, AnalogInputs, NULL))
    {
        /*int i = AnalogInputs[0];
        vector<int> vn;
        while (i) {
            vn.push_back(i % 2);
            i /= 2;
        }
        if (vn.size() >= 4) {
            int nValue = vn[3];
            if (1 == nValue)
                return true;
        }*/
    }
    else
    {
        string msg = "Get state of ACS 2D motion Initialize failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if ( !(AnalogInputs[1] & 8) || !(AnalogInputs[0] & 8))
    {
        return homingForced();
    }

    return Result();

    if (Y == 1)
    {
        if (!(AnalogInputs[2] & 8))
        {
            if (!acsc_RunBuffer(m_AcsHandle, 3, NULL, NULL))
            {
                string msg = "ACS 3D motion Initialize failed: " + GetErrorMessage();
                LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
                return Result(false, msg);
            }
            acsc_WaitProgramEnd(m_AcsHandle, 3, 30000);
        }

        // TODO: check buffer 1(Z)
        if (!(AnalogInputs[0] & 8))
        {
            // TODO: reload buffer 1
            // if (!acsc_RunBuffer(m_AcsHandle, 1, NULL, NULL))
            //{
            //    string msg = "ACS 3D motion Initialize failed: " + GetErrorMessage();
            //    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());;
            //    return Result(false, msg);
            //}
            // acsc_WaitProgramEnd(m_AcsHandle, 1, 30000);
        }
        if (!(AnalogInputs[1] & 8))
        {
            if (!acsc_RunBuffer(m_AcsHandle, 2, NULL, NULL))
            {
                string msg = "ACS 3D motion Initialize failed: " + GetErrorMessage();
                LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
                ;
                return Result(false, msg);
            }
            acsc_WaitProgramEnd(m_AcsHandle, 2, 30000);
            acsc_RunBuffer(m_AcsHandle, 8, NULL, NULL);
            acsc_WaitProgramEnd(m_AcsHandle, 8, 30000);
        }

        return judgeInitBuffer();
    }
    else if (Y == 3)
    {
        if (!acsc_RunBuffer(m_AcsHandle, 1, NULL, NULL))
        {
            string msg = "ACS 3D motion Initialize failed: " + GetErrorMessage();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
            ;
            return Result(false, msg);
        }
        acsc_WaitProgramEnd(m_AcsHandle, 1, 30000);
    }
    return Result();
}

Result MLACSCController2D::judgeInitBuffer()
{
    int AnalogInputs[1] = {0};
    if (!acsc_ReadInteger(m_AcsHandle, ACSC_NONE, (char *)"MFLAGS", 0, 0, ACSC_NONE, ACSC_NONE, AnalogInputs, NULL))
    {
        string msg = "Get state of ACS 2D motion Initialize failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!(AnalogInputs[0] & 8))
    {
        acsc_StopBuffer(m_AcsHandle, 2, NULL);
        return Result(false, "ACS 2D motion, Buffer 2 init error.");
    }
    return Result();
}
void MLACSCController2D::threadFunc()
{
    while (m_CallbackFlag)
    {
        if (IsConnected())
        {
            if (m_Callbacks.count(kStateChanged))
            {
                m_Callbacks[kStateChanged]->NotifyMotion2DStateChanged(m_State, GetState());
            }

            if (m_Callbacks.count(kPositionChanged))
            {
                int postion;
                m_Callbacks[kPositionChanged]->NotifyMotion2DPosition();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
void MLACSCController2D::threadStart()
{

}
void MLACSCController2D::threadStop()
{

}
Result MLACSCController2D::setDirection(const std::vector<std::string>& list)
{
    if (list.size() < 6)
    {
        return Result(false, "Motion 2D config file is missing coordinate direction.");
    }
    m_directionMap[X] = list.at(4) == "+" ? true : false;
    m_directionMap[Y] = list.at(5) == "+" ? true : false;
    return Result();
}
float MLACSCController2D::updateDirection(int axis, float value)
{
    float newValue = m_directionMap[axis] ? value : -value;
    return newValue;
}
Result MLACSCController2D::judgePos(int posX, int posY)
{
    Result ret = judgePos("X", posX);
    if (!ret.success) {
        return ret;
    }

    ret = judgePos("Y", posY);
    if (!ret.success)
    {
        return ret;
    }
    return Result();
}
Result MLACSCController2D::judgePos(const char* axis, int posDest)
{
    int pos;
    Result ret = getPosition(axis, pos);
    if (!ret.success) {
        return ret;
    }

    // < 1mm
    if (abs(posDest - pos) < 1000) {
        return Result();
    }

    int ax;
    if (strcmp(axis, "X") == 0)
    {
        ax = X;
    }
    else
    {
        ax = Y;
    }
    posDest = updateDirection(ax, posDest);
    pos = updateDirection(ax, pos);

    return Result(false, QString("Motion2D move axis %1 error, dest position is %2, real position is %3")
        .arg(QString::fromUtf8(axis))
        .arg(posDest)
        .arg(pos)
        .toStdString());
}
Result MLACSCController2D::getPosition(const char* axis, int& pos)
{
    int ax;
    if (strcmp(axis, "X") == 0)
    {
        ax = X;
    }
    else
    {
        ax = Y;
    }

    double posD;
    if (acsc_GetFPosition(m_AcsHandle, ax, &posD, NULL))
    {
        pos = round(posD * m_Scale);
        return Result();
    }
    else
    {
        string msg = QString("ACS 2D motion get axis %1 position failed: ").arg(QString::fromStdString(axis)).toStdString() + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
}
//void MLACSCController2D::invokeStatesChangedCB(MLMotionState old_state, MLMotionState new_state)
//{
//    std::list<CoreMotionCallback *> cb_list = m_callbacks[MLMotionEvent::kStateChanged];
//    std::list<CoreMotionCallback *>::iterator it;
//    for (it = cb_list.begin(); it != cb_list.end(); it++)
//    {
//        (*it)->NotifyMotionStateChanged(old_state, new_state);
//    }
//}

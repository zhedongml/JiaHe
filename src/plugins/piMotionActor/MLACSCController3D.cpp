#include "MLACSCController3D.h"
#include "LogPlus.h"

MLACSCController3D::MLACSCController3D()
{
    m_IsHome = false;
    m_Enable = false;
    m_IsConnected = false;
    m_CallbackFlag = false;
    m_TotalAxes = 0;
    m_AcsHandle = ACSC_INVALID;
    m_State = MLMotionState::kMLStatusDisConnected;
}

MLACSCController3D::~MLACSCController3D()
{
    if (m_IsConnected) 
    {
        Disconnect();
        m_State = CORE::MLMotionState::kMLStatusDisConnected;
    }
    if (m_CallbackFlag)
    {
        m_CallbackFlag = false;
        m_CallbackHandle.join();
    }
    m_Callbacks.clear();
}
HANDLE MLACSCController3D::Connect(const char* address, int port)
{
    if (m_AcsHandle != ACSC_INVALID)return m_AcsHandle;
    m_AcsHandle = acsc_OpenCommEthernetTCP(const_cast<char*>(address), port);
    if (m_AcsHandle == ACSC_INVALID)
    {
        string msg = "ACS system connect failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return ACSC_INVALID;
    }

    double SysInfo = 0.0;
    acsc_SysInfo(m_AcsHandle, 13, &SysInfo, NULL);
    m_TotalAxes = (int)SysInfo;

    return m_AcsHandle;
}
Result MLACSCController3D::setAcsSLimit(cv::Point3f max, cv::Point3f min)
{
    string varR = "SRLIMIT";
    char *varCR = const_cast<char *>(varR.c_str());

    double dataR[4];
    if (!acsc_ReadReal(m_AcsHandle,          // communication handle
                       ACSC_NONE,            // standard variable
                       varCR,                // variable name
                       0, 3,                 // first dimension indexes
                       ACSC_NONE, ACSC_NONE, // no second dimension
                       dataR,                // pointer to the buffer that
                                             // receives requested values
                       NULL                  // waiting call
                       ))
    {
        return Result(false, QString("ACS read max soft right limit(SRLIMIT) failed, ").arg(acsc_GetLastError()).toStdString());
    }

    dataR[X] = max.x;
    dataR[Y] = max.y;
    dataR[Z] = max.z;
    if (!acsc_WriteReal(m_AcsHandle,          // communication handle
                        ACSC_NONE,            // standard variable
                        varCR,                // variable name
                        0, 3,                 // first dimension indexes
                        ACSC_NONE, ACSC_NONE, // no second dimension
                        dataR,                // pointer  to  the buffer contained values
                                              // to be written
                        NULL                  // waiting call
                        ))
    {
        return Result(false, QString("ACS write max soft right limit(SRLIMIT) failed, ").arg(acsc_GetLastError()).toStdString());
    }

    string varL = "SLLIMIT";
    char *varCL = const_cast<char *>(varL.c_str());
    double dataL[4];
    if (!acsc_ReadReal(m_AcsHandle,          // communication handle
                       ACSC_NONE,            // standard variable
                       varCL,                // variable name
                       0, 3,                 // first dimension indexes
                       ACSC_NONE, ACSC_NONE, // no second dimension
                       dataL,                // pointer to the buffer that
                                             // receives requested values
                       NULL                  // waiting call
                       ))
    {
        return Result(
            false, QString("ACS read max soft right limit(SRLIMIT) failed, ").arg(acsc_GetLastError()).toStdString());
    }

    dataL[X] = min.x;
    dataL[Y] = min.y;
    dataL[Z] = min.z;
    if (!acsc_WriteReal(m_AcsHandle,          // communication handle
                        ACSC_NONE,            // standard variable
                        varCL,                // variable name
                        0, 3,                 // first dimension indexes
                        ACSC_NONE, ACSC_NONE, // no second dimension
                        dataL,                // pointer  to  the buffer contained values
                                              // to be written
                        NULL                  // waiting call
                        ))
    {
        return Result(false, QString("ACS write max soft left limit(SLLIMIT) failed, ").arg(acsc_GetLastError()).toStdString());
    }
    return Result();
}
Result MLACSCController3D::Connect(const char* address)
{
    if (IsConnected())
    {
        SetEnable(true);
        return Result();
    }
    string info(address);
    vector<string> listMotion = Split(address, '\\');
    vector<string> list = Split(listMotion[0], '/');

    if (listMotion.size() >= 2)
    {
        numBuffer = atoi(listMotion[1].c_str());
    }

    const char* ip = list[0].c_str();
    int port = std::stoi(list[1]);

    X = std::stoi(list[2]);
    Y = std::stoi(list[3]);
    Z = std::stoi(list[4]);
    Result ret = setDirection(list);
    if (!ret.success)
    {
        return Result(false, "ACS 3D motion connect failed," + ret.errorMsg);
    }

    m_AcsHandle = Connect(ip,port);
    if (m_AcsHandle == ACSC_INVALID)
    {
        string msg = "ACS 3D motion connect failed, connect tcp error: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    //Result err = InitialMotionSystem();
    //if (!err.success)
    //{
    //    Disconnect();
    //    return err;
    //}
    m_IsConnected = true;
    SetEnable(true);
    m_Enable = true;
    m_State = MLMotionState::kMLStatusConnected;
    return Result();
}

bool MLACSCController3D::Disconnect()
{
    if (m_AcsHandle != ACSC_INVALID)
    {
        //SetEnable(false);
        //if (!m_Enable)
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
string MLACSCController3D::GetErrorMessage()
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
    return err;
}
Result MLACSCController3D::judgeInitBuffer()
{
    int AnalogInputs[4] = { 0,0,0,0 };
    if (!acsc_ReadInteger(m_AcsHandle,
        ACSC_NONE,
        (char*)"MFLAGS",
        0, 3,
        ACSC_NONE, ACSC_NONE,
        AnalogInputs,
        NULL
    )){
        string msg = "Get state of ACS 3D motion Initialize failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!(AnalogInputs[2] & 8)) {
        acsc_StopBuffer(m_AcsHandle, 3, NULL);
        return Result(false, "Buffer 3(X) init error.");
    }

    if (!(AnalogInputs[0] & 8)) {
        acsc_StopBuffer(m_AcsHandle, 1, NULL);
        return Result(false, "Buffer 1(Z) init error.");
    }

    if (!(AnalogInputs[1] & 8)) {
        acsc_StopBuffer(m_AcsHandle, 2, NULL);
        return Result(false, "Buffer 2(Y) init error.");
    }
    return Result();
}
Result MLACSCController3D::errorStartOrEnd(bool start)
{
    if (start) {
        int AxisList[] = { Y, X, Z, -1 };
        if (!acsc_FaultClearM(m_AcsHandle, AxisList, NULL)) {
            string msg = "ACS 3D motion disable failed: " + GetErrorMessage();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        }
        return Result();
    }
    else {
        string error = GetErrorMessage();
        if (error.length() == 0) {
            return Result();
        }
        return Result(false, error);
    }
    
}
Result MLACSCController3D::setDirection(const std::vector<std::string> &list)
{
    if (list.size() < 8)
    {
        return Result(false, "Motion 3D config file is missing coordinate direction.");
    }
    m_directionMap[X] = list.at(5) == "+" ? true: false;
    m_directionMap[Y] = list.at(6) == "+" ? true : false;
    m_directionMap[Z] = list.at(7) == "+" ? true : false;
    return Result();
}
int MLACSCController3D::updateDirection(int axis, int value)
{
    int newValue = m_directionMap[axis] ? value : -value;
    return newValue;
}

Result MLACSCController3D::judgePos(int posX, int posY, int posZ)
{
    Result ret = judgePos("X", posX);
    if(!ret.success){
        return ret;
    }

    ret = judgePos("Y", posY);
    if (!ret.success)
    {
        return ret;
    }

    ret = judgePos("Z", posZ);
    if (!ret.success)
    {
        return ret;
    }
    return Result();
}

Result MLACSCController3D::judgePos(const char *axis, int posDest)
{
    //return Result();

    int pos;
    Result ret = getPosition(axis, pos);
    if(!ret.success){
        return ret;
    }

    // < 1mm
    if (abs(posDest - pos) < 1000){
        return Result();
    }
    return Result(false, QString("Motion3D move axis %1 error, dest position is %2, real position is %3")
        .arg(QString::fromUtf8(axis))
        .arg(posDest)
        .arg(pos)
        .toStdString());
}

Result MLACSCController3D::getPosition(const char *axis, int &pos)
{
    int ax;
    if (strcmp(axis, "X") == 0)
    {
        ax = X;
    }
    else if (strcmp(axis, "Y") == 0)
    {
        ax = Y;
    }
    else
    {
        ax = Z;
    }

    double posD;
    if (acsc_GetFPosition(m_AcsHandle, ax, &posD, NULL))
    {
        pos = round(posD * m_Scale);
        return Result();
    }
    else
    {
        string msg = QString("ACS 3D motion get axis %1 position failed: ").arg(QString::fromStdString(axis)).toStdString() + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
}

bool MLACSCController3D::IsConnected()
{
    //return m_IsConnected;

    ACSC_CONNECTION_INFO* ConnectionInfo;
    
    if (!acsc_GetConnectionInfo(m_AcsHandle, ConnectionInfo))
    {
        string msg = "ACS 3D motion get connection info failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return false;
    }

    if (ConnectionInfo->Type == ACSC_CONNECTION_TYPE::ACSC_NOT_CONNECTED)
    {
        m_IsConnected = false;
        return false;
    }
    else {
        m_IsConnected = true;
        return true;
    }
}

void MLACSCController3D::SetEnable(bool enable)
{
    if (enable)
    {
        int AxisList[] = { Y, X, Z, -1 };
        if (!acsc_EnableM(m_AcsHandle, AxisList, NULL))
        {
            string msg = "ACS 3D motion enable failed: " + GetErrorMessage();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        }
        acsc_WaitMotorEnabled(m_AcsHandle, Z, 1, 5000);

        if (!acsc_EnableM(m_AcsHandle, AxisList, NULL))
        {
            string msg = "ACS 3D motion enable failed: " + GetErrorMessage();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        }
        acsc_WaitMotorEnabled(m_AcsHandle, Z, 1, 5000);
        m_Enable = enable;
    }
    else
    {
        int AxisList[] = { Y, X, Z, -1 };
        if (!acsc_DisableM(m_AcsHandle, AxisList, NULL))
        {
            string msg = "ACS 3D motion disable failed: " + GetErrorMessage();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        }
        m_Enable = enable;
    }

    
}

bool MLACSCController3D::IsEnabled()
{
    return m_Enable;
}

//Result MLACSCController3D::MoveRelAsync(int posX, int posY, int posZ)
//{
//    
//    if (IsMoving())
//    {
//        string msg = "The relative moving failed: the motion is moving!";
//        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
//        return Result(false, msg);
//    }
//    int AxisList[] = { Y, X, Z, -1 };
//    double PosList[] = {updateDirection(Y, posY) / m_Scale, updateDirection(X, posX) / m_Scale, updateDirection(Z, posZ) / m_Scale, -1};
//    int a = acsc_ToPointM(m_AcsHandle, ACSC_AMF_RELATIVE, AxisList, PosList, NULL);
//    if (!a) 
//    {
//        string msg = "ACS 3D motion relative moving failed: " +GetErrorMessage();
//        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
//        return Result(false, msg);
//
//    }
//    return Result();
//}
//
//Result MLACSCController3D::MoveAbsAsync(int posX, int posY, int posZ)
//{
//    if (IsMoving())
//    {
//        string msg = "The absolute moving failed: the motion is moving!";
//        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
//        return Result(false, msg);
//    }
//    int AxisList[] = { Y, X, Z, -1 };
//    double PosList[] = {updateDirection(Y, posY) / m_Scale, updateDirection(X, posX) / m_Scale, updateDirection(Z, posZ )/ m_Scale, -1};
//    if (!acsc_ToPointM(m_AcsHandle, 0, AxisList, PosList, NULL))
//    {
//        string msg = "ACS 3D motion absolute moving failed: " + GetErrorMessage();
//        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
//        return Result(false, msg);
//    }
//    return Result();
//}

Result MLACSCController3D::MoveRelAsync(int posX, int posY, int posZ)
{

    if (IsMoving())
    {
        string msg = "The relative moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, X, updateDirection(X, posX) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion relative moving X failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, Y, updateDirection(Y, posY) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion relative moving Y failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, Z, updateDirection(Z, posZ) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion relative moving Z failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    return Result();
}

Result MLACSCController3D::MoveAbsAsync(int posX, int posY, int posZ)
{
    if (IsMoving())
    {
        string msg = "The absolute moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    //int AxisList[] = { Y, X, Z, -1 };
    //double PosList[] = { updateDirection(Y, posY) / m_Scale, updateDirection(X, posX) / m_Scale, updateDirection(Z, posZ) / m_Scale, -1 };
    //if (!acsc_ToPointM(m_AcsHandle, 0, AxisList, PosList, NULL))
    //{
    //    string msg = "ACS 3D motion absolute moving failed: " + GetErrorMessage();
    //    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    //    return Result(false, msg);
    //}

    if (!acsc_ToPoint(m_AcsHandle, 0, X, updateDirection(X, posX) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion absolute moving X failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!acsc_ToPoint(m_AcsHandle, 0, Y, updateDirection(Y, posY) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion absolute moving Y failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!acsc_ToPoint(m_AcsHandle, 0, Z, updateDirection(Z, posZ) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion absolute moving Z failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    return Result();
}

Result MLACSCController3D::MoveRelAsync(const char* axis, int pos)
{
    int ax;
    if (strcmp (axis,"X") == 0)
    {
        ax = X;
    }
    else if (strcmp(axis, "Y") == 0)
    {
        ax = Y;
    }
    else
    {
        ax = Z;
    }
    pos = updateDirection(ax, pos);

    int State;
    acsc_GetMotorState(m_AcsHandle, ax, &State, NULL);
    // If motor is moving state, 
    if (State & ACSC_MST_MOVE)
    {
        string msg = "[" + string(axis) + "] axis is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    double position = pos / m_Scale;
    if (!acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, ax, position, NULL))
    {
        string msg = "ACS 3D motion " + string(axis) + " axis relative moving failed : " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    return Result();
}

Result MLACSCController3D::MoveAbsAsync(const char* axis, int pos)
{
    int ax;
    if (strcmp(axis, "X") == 0)
    {
        ax = X;
    }
    else if (strcmp(axis, "Y") == 0)
    {
        ax = Y;
    }
    else
    {
        ax = Z;
    }
    pos = updateDirection(ax, pos);

    int State;
    acsc_GetMotorState(m_AcsHandle, ax, &State, NULL);
    // If motor is moving state, 
    if (State & ACSC_MST_MOVE)
    {
        string msg = "[" + string(axis) + "] axis is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    double position = pos / m_Scale;
    // Seconds arguement '0', it means absolute position (no option)
    if (!acsc_ToPoint(m_AcsHandle, 0, ax, position, NULL))
    {
        string msg = "ACS 3D motion " + string(axis) + " axis absolute moving failed : " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    return Result();
}

Result MLACSCController3D::MoveRel(const char *axis, int pos)
{
    if (IsMoving())
    {
        string msg = "The relative moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    int posCurrent;
    Result ret = getPosition(axis, posCurrent);
    if(!ret.success){
        return ret;
    }

    int ax;
    if (strcmp(axis, "X") == 0)
    {
        ax = X;
    }
    else if (strcmp(axis, "Y") == 0)
    {
        ax = Y;
    }
    else
    {
        ax = Z;
    }
    pos = updateDirection(ax, pos);

    double position = pos / m_Scale;
    if (!acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, ax, position, NULL))
    {
        string msg = "ACS 3D motion " + string(axis) + " axis relative moving failed : " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    acsc_WaitMotionEnd(m_AcsHandle, ax, 3000);
    while (IsMoving())
    {
        Sleep(50);
    }

    ret = judgePos(axis, pos + posCurrent);
    return ret;
}

Result MLACSCController3D::MoveAbs(const char *axis, int pos)
{
    if (IsMoving())
    {
        string msg = "The relative moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    int ax;
    if (strcmp(axis, "X") == 0)
    {
        ax = X;
    }
    else if (strcmp(axis, "Y") == 0)
    {
        ax = Y;
    }
    else
    {
        ax = Z;
    }
    pos = updateDirection(ax, pos);

    double position = pos / m_Scale;
    // Seconds arguement '0', it means absolute position (no option)
    if (!acsc_ToPoint(m_AcsHandle, 0, ax, position, NULL))
    {
        string msg = "ACS 3D motion " + string(axis) + " axis absolute moving failed : " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    acsc_WaitMotionEnd(m_AcsHandle, ax, 3000);
    while (IsMoving())
    {
        Sleep(50);
    }

    Result ret = judgePos(axis, pos);
    return ret;
}

//Result MLACSCController3D::MoveRel(int posX, int posY, int posZ)
//{
//    if (IsMoving())
//    {
//        string msg = "The relative moving failed: the motion is moving!";
//        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
//        return Result(false, msg);
//    }
//
//    ML_Point3D posCurrent = GetPosition();
//
//    int AxisList[] = { Y, X, Z, -1 };
//    double PosList[] = {updateDirection(Y, posY) / m_Scale, updateDirection(X, posX) / m_Scale, updateDirection(Z, posZ) / m_Scale, -1};
//    if (!acsc_ToPointM(m_AcsHandle, ACSC_AMF_RELATIVE, AxisList, PosList, NULL))
//    {
//        string msg = "ACS 3D motion relative moving failed: " + GetErrorMessage();
//        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
//        return Result(false, msg);
//    }
//    acsc_WaitMotionEnd(m_AcsHandle, X, 3000);
//    acsc_WaitMotionEnd(m_AcsHandle, Y, 3000);
//    acsc_WaitMotionEnd(m_AcsHandle, Z, 3000);
//    while (IsMoving()) {
//        Sleep(50);
//    }
//
//    Result ret = judgePos(updateDirection(X, posCurrent.x + posX), updateDirection(Y, posCurrent.y + posY),
//                          updateDirection(Z, posCurrent.z + posZ));
//    return ret;
//}

//Result MLACSCController3D::MoveAbs(int posX, int posY, int posZ)
//{
//    if (IsMoving())
//    {
//        string msg = "The absolute moving failed: the motion is moving!";
//        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
//        return Result(false, msg);
//    }
//    int AxisList[] = { Y, X, Z, -1 };
//
//    //int mRet = acsc_FaultClearM(m_AcsHandle, AxisList, NULL);
//    //if (!mRet)
//    //{
//    //    return Result(false, "ACS 3D motion absolute moving failed: Fault Clear Error.");
//    //}
//    //int errCodeTmp = acsc_GetLastError();
//
//    double PosList[] = {updateDirection(Y, posY) / m_Scale, updateDirection(X, posX) / m_Scale, updateDirection(Z, posZ) / m_Scale, -1};
//    if (!acsc_ToPointM(m_AcsHandle, 0, AxisList, PosList, NULL))
//    {
//        string msg = "ACS 3D motion absolute moving failed: " + GetErrorMessage();
//        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
//        return Result(false, msg);
//    }
//    acsc_WaitMotionEnd(m_AcsHandle, X, 3000);
//    acsc_WaitMotionEnd(m_AcsHandle, Y, 3000);
//    acsc_WaitMotionEnd(m_AcsHandle, Z, 5000);
//    while (IsMoving()) {
//        Sleep(50);
//    }
//
//    //int timeout = acsc_GetTimeout(m_AcsHandle);
//    //int errCode = acsc_GetLastError();
//    //if (errCode > 0 && errCode != ACSC_TIMEOUT)
//    //{
//    //    string str =  GetErrorMessage();
//    //}
//
//    Result ret = judgePos(updateDirection(X, posX), updateDirection(Y, posY), updateDirection(Z, posZ));
//    return ret;
//}

Result MLACSCController3D::MoveRel(int posX, int posY, int posZ)
{
    if (IsMoving())
    {
        string msg = "The relative moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    ML_Point3D posCurrent = GetPosition();

    if (!acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, X, updateDirection(X, posX) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion relative moving X failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, Y, updateDirection(Y, posY) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion relative moving Y failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!acsc_ToPoint(m_AcsHandle, ACSC_AMF_RELATIVE, Z, updateDirection(Z, posZ) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion relative moving Z failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    acsc_WaitMotionEnd(m_AcsHandle, X, 3000);
    acsc_WaitMotionEnd(m_AcsHandle, Y, 3000);
    acsc_WaitMotionEnd(m_AcsHandle, Z, 3000);
    while (IsMoving()) {
        Sleep(50);
    }

    Result ret = judgePos(updateDirection(X, posCurrent.x + posX), updateDirection(Y, posCurrent.y + posY),
        updateDirection(Z, posCurrent.z + posZ));
    return ret;
}

Result MLACSCController3D::MoveAbs(int posX, int posY, int posZ)
{
    if (IsMoving())
    {
        string msg = "The absolute moving failed: the motion is moving!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    //int mRet = acsc_FaultClearM(m_AcsHandle, AxisList, NULL);
    //if (!mRet)
    //{
    //    return Result(false, "ACS 3D motion absolute moving failed: Fault Clear Error.");
    //}
    //int errCodeTmp = acsc_GetLastError();

    if (!acsc_ToPoint(m_AcsHandle, 0, X, updateDirection(X, posX) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion absolute moving X failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!acsc_ToPoint(m_AcsHandle, 0, Y, updateDirection(Y, posY) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion absolute moving Y failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

    if (!acsc_ToPoint(m_AcsHandle, 0, Z, updateDirection(Z, posZ) / m_Scale, NULL))
    {
        string msg = "ACS 3D motion absolute moving Z failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    acsc_WaitMotionEnd(m_AcsHandle, X, 3000);
    acsc_WaitMotionEnd(m_AcsHandle, Y, 3000);
    acsc_WaitMotionEnd(m_AcsHandle, Z, 5000);
    while (IsMoving()) {
        Sleep(50);
    }

    //int timeout = acsc_GetTimeout(m_AcsHandle);
    //int errCode = acsc_GetLastError();
    //if (errCode > 0 && errCode != ACSC_TIMEOUT)
    //{
    //    string str =  GetErrorMessage();
    //}

    Result ret = judgePos(updateDirection(X, posX), updateDirection(Y, posY), updateDirection(Z, posZ));
    return ret;
}

Result MLACSCController3D::StopMove()
{
    int AxisList[] = { Y, X, Z, -1 };

    if (!acsc_HaltM(m_AcsHandle, AxisList, NULL))
    {
        string msg = "ACS 3D motion stop moving failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }
    return Result();
}

Result MLACSCController3D::Homing()
{
    if (m_IsConnected) 
    {
        //for (int i = 0; i < m_TotalAxes;i++) 
        //{
        //    std::string buf = "HOME "+std::to_string(i)+",1,10";
        //    char* command = const_cast<char*>(buf.c_str());
        //    if (!acsc_Command(m_AcsHandle, command,buf.size() , NULL)) return ACSCErrorExecFailed;
        //} 
        Result err = InitialMotionSystem();
        if (!err.success) 
        {
            string msg = "ACS 3D motion homing failed: " + GetErrorMessage();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
            m_IsHome = false;
            return Result(false, msg);
        }
        m_IsHome = true;
        return Result();
    }
    return Result(false, "ACS 3D motion is not connected.");
}

bool MLACSCController3D::IsHome()
{
    return m_IsHome;
}

void MLACSCController3D::SetSpeed(int speed)
{
    int s = speed / m_Scale;
    if (!acsc_SetVelocity(m_AcsHandle, X, s, NULL))
    {
        string msg = "ACS 3D motion set X axis Velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    }
    if (!acsc_SetVelocity(m_AcsHandle, Y, s, NULL))
    {
        string msg = "ACS 3D motion set Y axis Velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    }
    if (!acsc_SetVelocity(m_AcsHandle, Z, s, NULL))
    {
        string msg = "ACS 3D motion set Z axis Velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    }
}

int MLACSCController3D::GetSpeed()
{
    double speed;
    if(!acsc_GetVelocity(m_AcsHandle, X, &speed, NULL))
    {
        string msg = "ACS 3D motion get velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return 0;
    }
    else
    {
        return int(speed * m_Scale);
    }
}

bool MLACSCController3D::IsMoving()
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
    if (acsc_GetAxisState(m_AcsHandle, Z, &state, NULL))
    {
        if (state & ACSC_MST_MOVE)
        {
            moving = true;
        }
    }
    return moving;
}

ML_Point3D MLACSCController3D::GetPosition()
{
    if (!m_IsConnected) {
        ML_Point3D tmp;
        tmp.x = 0;
        tmp.y = 0;
        tmp.z = 0;
        return tmp;
    }

    ML_Point3D positions;
    double pos;
    if (acsc_GetFPosition(m_AcsHandle, X, &pos, NULL))
    {
        positions.x = round(pos * m_Scale);
    }
    else
    {
        string msg = "ACS 3D motion get X position failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        positions.x = 0;
    }

    if (acsc_GetFPosition(m_AcsHandle, Y, &pos, NULL))
    {
        positions.y = round(pos * m_Scale);
    }
    else
    {
        string msg = "ACS 3D motion get Y position failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        positions.y = 0;
    }

    if (acsc_GetFPosition(m_AcsHandle, Z, &pos, NULL))
    {
        positions.z = round(pos * m_Scale);
    }
    else
    {
        string msg = "ACS 3D motion get Z position failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        positions.z = 0;
    }

    positions.x = updateDirection(X, positions.x);
    positions.y = updateDirection(Y, positions.y);
    positions.z = updateDirection(Z, positions.z);
    return positions;

    //if (!acsc_ReadReal(m_AcsHandle, -1, const_cast <char*>("FPOS"), 0, m_TotalAxes - 1, -1, -1, m_FPosition, NULL))
    //{
    //    string msg = "ACS 3D motion get positions failed: " + MLACSSystemController::GetInstance().GetErrorMessage();
    //    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    //    return ML_Point3D();
    //}
    //else
    //{
    //    ML_Point3D pos{ m_FPosition[X] * m_Scale, m_FPosition[Y] * m_Scale, m_FPosition[Z] * m_Scale };
    //    return pos;
    //}
}

MLMotionState MLACSCController3D::GetState()
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

//ACSCErrorType MLACSCController3D::GetState(int axis,MLMotionState& state)
//{
//    int State;
//    if (!acsc_GetMotorState(m_AcsHandle, axis, &State, NULL))
//    {
//        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, GetErrorMessage().c_str());
//        return ACSCErrorExecFailed;
//    }
//    switch(State)
//    {
//    case ACSC_MST_ENABLE:
//        state = Enabble;
//        break;
//    case ACSC_MST_INPOS:
//        state = InPosition;
//        break;
//    case ACSC_MST_MOVE:
//        state = Moving;
//        break;
//    case ACSC_MST_ACC:
//        state = Accelerating;
//        break;
//    default:
//        state = Disconnected;
//        break;
//    }
//    return ACSCErrorSuccess;
//}

void MLACSCController3D::Subscribe(MLMotionEvent event, MLMotion3DCallback* callback)
{
    if (!m_Callbacks.count(event)) 
    {
        m_Callbacks[event] = callback;
        if (!m_CallbackFlag) 
        {
            m_CallbackHandle = std::thread(&MLACSCController3D::threadFunc, this);
            m_CallbackFlag = true;
        }
    }
}

void MLACSCController3D::Unsubscribe(MLMotionEvent event, MLMotion3DCallback* callback)
{
    if(m_Callbacks.count(event))
    {  
       if (m_CallbackFlag)
        {
            m_CallbackFlag = false;
            m_CallbackHandle.join();

            m_Callbacks.clear();
        }
    }
}

void MLACSCController3D::SetSpeedX(double speed)
{
    if (!acsc_SetVelocity(m_AcsHandle, X, speed, NULL))
    {
        string msg = "ACS 3D motion set X axis Velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    }
    
    
}

void MLACSCController3D::SetSpeedY(double speed)
{
    if (!acsc_SetVelocity(m_AcsHandle, Y, speed, NULL))
    {
        string msg = "ACS 3D motion set Y axis Velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    }
}

void MLACSCController3D::SetSpeedZ(double speed)
{
    if (!acsc_SetVelocity(m_AcsHandle, Z, speed, NULL))
    {
        string msg = "ACS 3D motion set Z axis Velocity failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
    }
}

void MLACSCController3D::threadFunc()
{
    while (m_CallbackFlag) 
    {
        if (m_IsConnected)
        {
            if (m_Callbacks.count(kStateChanged))
            {
                //if(acsc_ReadInteger(m_AcsHandle, -1, const_cast<char*>("MST"), 0, m_TotalAxes - 1, -1, -1, m_MotorStatus, NULL))
                m_Callbacks[kStateChanged]->NotifyMotion3DStateChanged(m_State, GetState());
            }

            if (m_Callbacks.count(kPositionChanged))
            {
                /*ML_Point3D pos = GetPosition();
                m_Callbacks[kPositionChanged]->NotifyMotion3DPosition(pos.x,pos.y,pos.z);*/
                m_Callbacks[kPositionChanged]->NotifyMotion3DPosition();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

Result MLACSCController3D::InitialMotionSystem()
{
    //double SysInfo = 0.0;
    //acsc_SysInfo(m_AcsHandle, 10, &SysInfo, NULL);
    //int bufferSize = (int)SysInfo;
    // TODO: Measuresetting.json, "Motion3D":{"IP":"10.0.1.100/701/2/1/0"},y==1 is IQT7,y==3 is system

    int AnalogInputs[4] = { 0,0,0,0 };
    if (acsc_ReadInteger(m_AcsHandle,
        ACSC_NONE,
        (char*)"MFLAGS",
        0, 3,
        ACSC_NONE, ACSC_NONE,
        AnalogInputs,
        NULL
    ))
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
        string msg = "Get state of ACS 3D motion Initialize failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return Result(false, msg);
    }

     if (!(AnalogInputs[0] & 8) || !(AnalogInputs[2] & 8) || !(AnalogInputs[3] & 8))
    {
         if (!acsc_RunBuffer(m_AcsHandle, numBuffer, NULL, NULL))
        {
            string msg = "Get state of ACS 3D motion Initialize failed: " + GetErrorMessage();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
            return Result(false, msg);
        }
        acsc_WaitProgramEnd(m_AcsHandle, numBuffer, 30000);
    }

    return Result();


    if (Y == 1) {
        if (!(AnalogInputs[2] & 8)) {
            if (!acsc_RunBuffer(m_AcsHandle, 3, NULL, NULL))
            {
                string msg = "ACS 3D motion Initialize failed: " + GetErrorMessage();
                LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
                return Result(false, msg);
            }
            acsc_WaitProgramEnd(m_AcsHandle, 3, 30000);
        }

        //TODO: check buffer 1(Z)
        if (!(AnalogInputs[0] & 8)) {
            //TODO: reload buffer 1
            //if (!acsc_RunBuffer(m_AcsHandle, 1, NULL, NULL))
            //{
            //    string msg = "ACS 3D motion Initialize failed: " + GetErrorMessage();
            //    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());;
            //    return Result(false, msg);
            //}
            //acsc_WaitProgramEnd(m_AcsHandle, 1, 30000);
        }
        if (!(AnalogInputs[1] & 8)) {
            if (!acsc_RunBuffer(m_AcsHandle, 2, NULL, NULL))
            {
                string msg = "ACS 3D motion Initialize failed: " + GetErrorMessage();
                LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());;
                return Result(false, msg);
            }
            acsc_WaitProgramEnd(m_AcsHandle, 2, 30000);
            acsc_RunBuffer(m_AcsHandle, 8, NULL, NULL);
            acsc_WaitProgramEnd(m_AcsHandle, 8, 30000);
        }

        return judgeInitBuffer();
    }
    else if(Y == 3){
        if (!acsc_RunBuffer(m_AcsHandle, 1, NULL, NULL))
        {
            string msg = "ACS 3D motion Initialize failed: " + GetErrorMessage();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());;
            return Result(false, msg);
        }
        acsc_WaitProgramEnd(m_AcsHandle, 1, 30000);
    }
    return Result();
}
vector<string> MLACSCController3D::Split(string str, char a)
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

void MLACSCController3D::RegisterEmergencyStop()
{
    acsc_RegisterEmergencyStop();
}

void MLACSCController3D::UnregisterEmergencyStop()
{
    acsc_UnregisterEmergencyStop();
}
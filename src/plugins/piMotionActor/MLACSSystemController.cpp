#include "MLACSSystemController.h"


MLACSSystemController::MLACSSystemController()
{
    m_AcsHandle = ACSC_INVALID;
}

MLACSSystemController::~MLACSSystemController()
{
    if (m_AcsHandle != ACSC_INVALID) Disconnect();
}


HANDLE MLACSSystemController::Connect(const char* address,int port)
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

bool MLACSSystemController::Disconnect()
{
    if (!acsc_DisableAll(m_AcsHandle, NULL))
    {
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, GetErrorMessage().c_str());
    }

    if (acsc_CloseComm(m_AcsHandle))
    {
        m_AcsHandle = ACSC_INVALID;
        return true;
    }
    else
    {
        string msg = "ACS system disconnect failed: " + GetErrorMessage();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, msg.c_str());
        return false;
    }
    
}

string MLACSSystemController::GetErrorMessage()
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

vector<string> MLACSSystemController::Split(string str, char a)
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

void MLACSSystemController::RegisterEmergencyStop()
{
    acsc_RegisterEmergencyStop();
}

void MLACSSystemController::UnregisterEmergencyStop()
{
    acsc_UnregisterEmergencyStop();
}


#include "PIController.h"
#include "LogPlus.h"
#include <windows.h>
#include "PI_GCS2_DLL.h"

PIController& PIController::GetInstance()
{
    static PIController pi_controller;
    return pi_controller;
}


int PIController::OpenConnection(ConnectionType type, const char* hostName)
{
    int         iD = -1;
    std::string connType;

    switch (type)
    {
    case Dialog:
        iD = PI_InterfaceSetupDlg("");
        connType = "Dialog";
        break;
    case RS232:
        iD = PI_ConnectRS232(1, 115200);
        connType = "RS232";
        break;
    case TCPIP:
        iD = PI_ConnectTCPIP(hostName, 50000);
        connType = "TCPIP";
        break;
    case USB:
        vector<string> strlist = split(hostName, ' ');

        char connectedUsbController[1024];
        int noDevices = PI_EnumerateUSB(connectedUsbController, 1024, strlist[0].c_str());

        if (noDevices == 0)
        {
            map< string, int >::iterator iter;
            for (iter = mapPIUSBcontroller.begin(); iter != mapPIUSBcontroller.end(); iter++)
            {
                cout << iter->first << ' ' << iter->second << endl;

                if (iter->first.find(strlist[2]) != string::npos)
                {
                    iD = iter->second;
                    break;
                }
            }
        }
        else
        {
            vector<string> desclist = split(connectedUsbController, '\n');

            for (size_t i = 0; i < noDevices; i++)
            {
                if (desclist[i].find(strlist[2]) != string::npos)
                {
                    iD = PI_ConnectUSB(desclist[i].c_str());

                    if (iD > -1)
                    {
                        mapPIUSBcontroller.insert(pair<string, int>(desclist[i], iD));
                    }

                    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "PI Description :" << desclist[i].c_str());

                    break;
                }
            }
        }
        
        
        connType = "USB";
        break;
    }

    if (iD < 0)
    {
        ReportError(iD);

        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Connect ERROR" << connType.c_str());

        std::cout << "Connect " << connType << ": ERROR" << std::endl;
    }

    return iD;

    return 0;
}


void PIController::ReportError(int iD)
{
    /*int  err = PI_GetError(iD);
    char szErrMsg[300];

    if (PI_TranslateError(err, szErrMsg, 299))
    {
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Error " << err << " occurred: " << szErrMsg);

    }*/
}

vector<string> split(string str, char a)
{
    string strtem = str;
    vector<string> strvec;

    string::size_type pos1, pos2;
    pos2 = strtem.find(a);
    pos1 = 0;
    while (string::npos != pos2)
    {
        strvec.push_back(strtem.substr(pos1, pos2 - pos1));

        pos1 = pos2 + 1;
        pos2 = strtem.find(a, pos1);
    }
    strvec.push_back(strtem.substr(pos1));
    return strvec;
}
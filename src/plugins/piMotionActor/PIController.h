#ifndef PICONTROLLER_H_
#define PICONTROLLER_H_
#include "pimotionactor_global.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <Windows.h>
#include "PI_GCS2_DLL.h"

using namespace std;

enum ConnectionType
{
    Dialog,
    RS232,
    TCPIP,
    USB
};


class PIMOTIONACTOR_EXPORT PIController
{
public:
    static PIController& GetInstance();
    int OpenConnection(ConnectionType type, const char* hostName = "localhost");
private:
    void ReportError(int iD);

    PIController() 
    {
    }
    PIController(const PIController&) = delete;
    PIController& operator=(const PIController&) = delete;

    map<string, int> mapPIUSBcontroller;
};

vector<string> split(string strtem, char a);

#endif
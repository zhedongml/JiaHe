#ifndef ACS_SYSTEM_H_
#define ACS_SYSTEM_H_
#include "pimotionactor_global.h"
#include <ACSC.h>
#include <string>
#include <vector>
#include "LogPlus.h"
using std::string;

using std::vector;

class MLACSSystemController
{
public:
    static MLACSSystemController& GetInstance()
    {
        static MLACSSystemController system;
        return system;
    }

    HANDLE Connect(const char* address,int port);

    bool Disconnect();

    string GetErrorMessage();

    void RegisterEmergencyStop();

    void UnregisterEmergencyStop();

    vector<string> Split(string str, char a);

private:
    MLACSSystemController();
    ~MLACSSystemController();
    
    MLACSSystemController(MLACSSystemController const&) = delete;

    MLACSSystemController& operator=(MLACSSystemController const&) = delete;

private:

    HANDLE m_AcsHandle;

    int m_TotalAxes;
};

#endif // !ACS_SYSTEM_H_
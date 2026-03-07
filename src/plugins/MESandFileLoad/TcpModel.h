#pragma once

#define WIN32_LEAN_AND_MEAN  // 减少无关头文件

#define _WINSOCKAPI_ // 禁止winsock.h被间接包含
#include <WinSock2.h>

#include <Windows.h>  // WindowsAPI基础头文件
#include <WS2tcpip.h>
#include <stdio.h>
#include "mesandfileload_global.h"
#include "json.hpp"
#include "MesCommon.h"

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include "PrjCommon/service/ml_motion.h"

#define DEFAULT_BUFLEN 512

using namespace std;
using namespace CORE;

using Json = nlohmann::json;

class MESANDFILELOAD_EXPORT TcpModel
{
public:
    TcpModel(void);
    ~TcpModel(void);

    void setIpPort(const string& ip, int port);
    void setCallback(CoreMotionCallback* callback);

    Result start(const string& ip, int port, bool forced = false);
    Result close();
    bool isOpened();
    Result sendData(const string& data);
    Result sendData(const ExternalTOSPathbody& data);
    Result sendData_old(const ExternalTOSPathbody& data);
    Result sendData(const ExternalTOSPathToRDbody& data);
    Result queryRecv(string& recvInfo, const string& sendInfo);
    Result queryRecv(string& recvInfo, const ExternalTOSPathbody& sendInfo);
    Result queryRecv(string& recvInfo, const ExternalTOSPathToRDbody& sendInfo);

private:
    Result start();
    Result setWaitTime(SOCKET socket);
    std::string createJson(const std::string& use, const std::string& csvPath, const std::vector<std::string>& zipPaths);

private:
    SOCKET m_socket;
    string m_ip;
    int m_port;
    CoreMotionCallback* m_callback;

    bool m_isConnected;
};
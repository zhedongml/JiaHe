#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Ws2tcpip.h>
#include <stdio.h>
#include <winsock2.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include "PrjCommon/service/ml_motion.h"

#define DEFAULT_BUFLEN 512

using namespace std;
using namespace CORE;

class IsTcp
{
  public:
    IsTcp(void);
    ~IsTcp(void);

    void setIpPort(const string &ip, int port);
    void setCallback(CoreMotionCallback *callback);

    Result start(const string &ip, int port, bool forced = false);
    Result close();
    bool isOpened();
    Result sendData(const string &data);
    Result queryRecv(string &recvInfo, const string &sendInfo);

  private:
    Result start();
    Result setWaitTime(SOCKET socket);

  private:
    SOCKET m_socket;
    string m_ip;
    int m_port;
    CoreMotionCallback *m_callback;

    bool m_isConnected;
};
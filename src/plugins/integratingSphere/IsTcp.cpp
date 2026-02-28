#include "IsTcp.h"
#include <QtEndian>
#include <QMutex>
#include <QDebug>

IsTcp::IsTcp(void)
{
    m_socket = INVALID_SOCKET;
    m_isConnected = false;

    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        qCritical() << QString("Labsphere WSAStartup failed with error: %1").arg(iResult);
    }
}

IsTcp::~IsTcp(void)
{
    close();
    WSACleanup();
}

void IsTcp::setIpPort(const string &ip, int port)
{
    m_ip = ip;
    m_port = port;
}

void IsTcp::setCallback(CoreMotionCallback *callback)
{
    m_callback = callback;
}

Result IsTcp::start(const string &ip, int port, bool forced)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    m_ip = ip;
    m_port = port;

    if (isOpened() && !forced)
    {
        return Result(); 
    }

    if (isOpened()){
        close();
    }

    int iResult;

    // Create a SOCKET for connecting to server
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        m_callback->NotifyMotionStateChanged(MLMotionState::kMLStatusConnected, MLMotionState::kMLStatusDisConnected);
        return Result(false, QString("Socket failed with error: %1").arg(WSAGetLastError()).toStdString());
    }

    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    struct sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(ip.c_str());
    clientService.sin_port = htons(port);

    // Connect to server.
    iResult = connect(m_socket, (SOCKADDR *)&clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR)
    {
        close();
        return Result(false, QString("Connect failed with error: %1").arg(WSAGetLastError()).toStdString());
    }

    m_callback->NotifyMotionStateChanged(MLMotionState::kMLStatusDisConnected, MLMotionState::kMLStatusConnected);
    m_isConnected = true;
    setWaitTime(m_socket);
    return Result();
}

Result IsTcp::close()
{
    bool pre = m_isConnected;
    m_isConnected = false;
    int iResult = closesocket(m_socket);
    if (iResult == SOCKET_ERROR)
    {
        m_socket = INVALID_SOCKET;
        if (pre){
            m_callback->NotifyMotionStateChanged(MLMotionState::kMLStatusConnected, MLMotionState::kMLStatusDisConnected);
        }
        return Result(false, QString("Close failed with error: %1").arg(WSAGetLastError()).toStdString());
    }

    m_socket = INVALID_SOCKET;
    if (pre)
    {
        m_callback->NotifyMotionStateChanged(MLMotionState::kMLStatusConnected, MLMotionState::kMLStatusDisConnected);
    }
    return Result();
}

bool IsTcp::isOpened()
{
    if (m_socket != INVALID_SOCKET && m_isConnected)
    {
        return true;
    }
    return false;

    //char buffer[1];
    //buffer[0] = '\0';
    //int iResult = send(m_socket, buffer, 1, 0);
    //if (iResult == SOCKET_ERROR)
    //{
    //    close();
    //    return false;
    //}
}

Result IsTcp::sendData(const string &data)
{
    char sendBuf[256];
    int len;
    memset(sendBuf, 0, 256);

    Result ret = start();
    if (!ret.success){
        return ret;
    }

    {
        QString dataStr = QString::fromStdString(data);
        QByteArray dataArray = dataStr.toUtf8();
        uchar datalen[4];
        qToBigEndian(dataArray.length(), datalen);

        QByteArray bytes((char *)datalen, 4);
        bytes.append(dataArray);

        len = bytes.length();
        memcpy(sendBuf, bytes, len);
    }

    int iResult = send(m_socket, sendBuf, len, 0);
    if (iResult == SOCKET_ERROR)
    {
        close();
        return Result(false, QString("Send failed with error: %1").arg(WSAGetLastError()).toStdString());
    }
    return Result();
}

Result IsTcp::queryRecv(string &recvInfo, const string &sendInfo)
{
    Result ret = sendData(sendInfo);
    if(!ret.success){
        return ret;
    }

    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN] = "";

    int number = 10;
    while (number-- > 0)
    {
        Sleep(100);

        int iResult = recv(m_socket, recvbuf, recvbuflen, 0);
        if (iResult == 0)
        {
            close();
            return Result(false, "Connect closed.");
        }
        else if (iResult < 0)
        {
            close();
            return Result(false, QString("Recv failed with error: %1").arg(WSAGetLastError()).toStdString());
        }

        recvInfo = string(recvbuf, recvbuf + iResult);
        if (recvInfo.find(sendInfo) != string::npos)
        {
            break;
        }
    }

    return Result();
}

Result IsTcp::start()
{
    return start(m_ip, m_port);
}

Result IsTcp::setWaitTime(SOCKET socket)
{
    int nTimeout = 3000;
    if (SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&nTimeout, sizeof(int)))
    {
        return Result(false, QString("Set SO_SNDTIMEO error: %1").arg(WSAGetLastError()).toStdString());
    }

    if (SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nTimeout, sizeof(int)))
    {
        return Result(false, QString("Set SO_RCVTIMEO error: %1").arg(WSAGetLastError()).toStdString());
    }
    return Result();
}

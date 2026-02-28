#include "DecoderTcp.h"
#include <QtEndian>
#include <QMutex>

DecoderTcp::DecoderTcp(void)
{
    m_socket = INVALID_SOCKET;
    m_isConnected = false;
}

DecoderTcp::~DecoderTcp(void)
{
    close();
}

void DecoderTcp::setIpPort(const string &ip, int port)
{
    m_ip = ip;
    m_port = port;
}

void DecoderTcp::setCallback(CoreMotionCallback *callback)
{
    m_callback = callback;
}

Result DecoderTcp::start(const string &ip, int port, bool forced)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    m_ip = ip;
    m_port = port;

    if (isOpened())
    {
        return Result();
    }

    Result ret = connectWithTimeout(ip, port);
    if(!ret.success){
        m_callback->NotifyMotionStateChanged(MLMotionState::kMLStatusDisConnected, MLMotionState::kMLStatusFailed);
    }else{
        m_callback->NotifyMotionStateChanged(MLMotionState::kMLStatusDisConnected, MLMotionState::kMLStatusConnected);
    }
    return ret;
}

Result DecoderTcp::close()
{
    bool pre = m_isConnected;
    m_isConnected = false;
    int iResult = closesocket(m_socket);
    if (iResult == SOCKET_ERROR)
    {
        WSACleanup();
        m_socket = INVALID_SOCKET;
        if (pre){
            m_callback->NotifyMotionStateChanged(MLMotionState::kMLStatusConnected, MLMotionState::kMLStatusDisConnected);
        }
        return Result(false, QString("Close failed with error: %1").arg(WSAGetLastError()).toStdString());
    }

    WSACleanup();
    m_socket = INVALID_SOCKET;
    if (pre)
    {
        m_callback->NotifyMotionStateChanged(MLMotionState::kMLStatusConnected, MLMotionState::kMLStatusDisConnected);
    }
    return Result();
}

bool DecoderTcp::isOpened()
{
    if (m_socket != INVALID_SOCKET && m_isConnected)
    {
        return true;
    }
    return false;
}

Result DecoderTcp::sendData(const string &data)
{
    Result ret = start();
    if (!ret.success){
        return ret;
    }

    string sendBuf = data + "\r";
    const char* mm = sendBuf.c_str();

    int iResult = send(m_socket, sendBuf.c_str(), (int)sendBuf.size(), 0);
    if (iResult == SOCKET_ERROR)
    {
        close();
        return Result(false, QString("Decoder send failed, send data: %1, error: %2")
            .arg(QString::fromStdString(sendBuf))
            .arg(WSAGetLastError()).toStdString());
    }
    return Result();
}

Result DecoderTcp::queryRecv(string &recvInfo, const string &sendInfo)
{
    recvInfo.clear();

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
        if(recvbuf[0] == '\0'){
            continue;
        }

        recvInfo = string(recvbuf, recvbuf + iResult);
        if (!recvInfo.empty()) {
            break;
        }
    }

    if(recvInfo.empty()){
        return Result(false, "Query failed, send info: " + sendInfo + ", recive info is empty.");
    }

    recvInfo.erase(std::remove(recvInfo.begin(), recvInfo.end(), '\r'), recvInfo.end());
    return Result();
}

Result DecoderTcp::connectBlocking(const string& ip, int port)
{
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        return Result(false, QString("WSAStartup failed with error: %1").arg(iResult).toStdString());
    }

    // Create a SOCKET for connecting to server
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        WSACleanup();
        return Result(false, QString("Socket failed with error: %1").arg(WSAGetLastError()).toStdString());
    }

    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    struct sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(ip.c_str());
    clientService.sin_port = htons(port);

    // Connect to server.
    iResult = connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR)
    {
        close();
        return Result(false, QString("Connect failed with error: %1").arg(WSAGetLastError()).toStdString());
    }

    m_isConnected = true;
    setWaitTime(m_socket);
    return Result();
}

Result DecoderTcp::connectWithTimeout(const string& ip, int port)
{
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        return Result(false, QString("WSAStartup failed with error: %1").arg(iResult).toStdString());
    }

    // Create a SOCKET for connecting to server
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        WSACleanup();
        return Result(false, QString("Socket failed with error: %1").arg(WSAGetLastError()).toStdString());
    }

    // Set to non-blocking mode
    unsigned long mode = 1;
    ioctlsocket(m_socket, FIONBIO, &mode);

    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    struct sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(ip.c_str());
    clientService.sin_port = htons(port);

    // Connect to server.
    int timeout_sec = 5;
    iResult = connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            close();
            return Result(false, QString("Connect failed with error: %1.").arg("invalid socket").toStdString());
        }

        // set select timeout
        fd_set set;
        FD_ZERO(&set);
        FD_SET(m_socket, &set);

        timeval timeout;
        timeout.tv_sec = timeout_sec;
        timeout.tv_usec = 0;

        // Wait for the connection to complete or time out
        iResult = select(0, NULL, &set, NULL, &timeout);
        if (iResult <= 0) {
            close();
            return Result(false, QString("Connect failed with error: %1").arg(WSAGetLastError()).toStdString());
        }
    }

    // Reset to blocking mode
    mode = 0;
    ioctlsocket(m_socket, FIONBIO, &mode);

    m_isConnected = true;
    setWaitTime(m_socket);
    return Result();
}

Result DecoderTcp::start()
{
    Result ret = start(m_ip, m_port);
    if(!ret.success){
        return Result(false, "Barcode reader connect failed.");
    }
    return Result();
}

Result DecoderTcp::setWaitTime(SOCKET socket)
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

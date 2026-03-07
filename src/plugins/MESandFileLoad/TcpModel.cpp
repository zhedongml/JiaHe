#include "TcpModel.h"
#include <QtEndian>
#include <QMutex>
#include "MesConfig.h"

TcpModel::TcpModel(void)
{
    m_socket = INVALID_SOCKET;
    m_isConnected = false;

    QString ip;
    int port;
    MesConfig::instance().getTcpInfo(ip, port);
    setIpPort(ip.toStdString(), port);

    // Initialize Winsock;
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        qCritical() << QString("QDP WSAStartup failed with error: %1").arg(iResult);
    }
}

TcpModel::~TcpModel(void)
{
    close();
    WSACleanup();
}

void TcpModel::setIpPort(const string& ip, int port)
{
    m_ip = ip;
    m_port = port;
}

void TcpModel::setCallback(CoreMotionCallback* callback)
{
    m_callback = callback;
}

Result TcpModel::start(const string& ip, int port, bool forced)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    m_ip = ip;
    m_port = port;

    if (isOpened() && !forced)
    {
        return Result();
    }

    if (isOpened()) {
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
    iResult = connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR)
    {
        close();
        return Result(false, QString("Connect failed with error: %1").arg(WSAGetLastError()).toStdString());
    }

    //m_callback->NotifyMotionStateChanged(MLMotionState::kMLStatusDisConnected, MLMotionState::kMLStatusConnected);
    m_isConnected = true;
    setWaitTime(m_socket);
    return Result();
}

Result TcpModel::close()
{
    bool pre = m_isConnected;
    m_isConnected = false;
    int iResult = closesocket(m_socket);
    if (iResult == SOCKET_ERROR)
    {
        m_socket = INVALID_SOCKET;
        if (pre) {
            //m_callback->NotifyMotionStateChanged(MLMotionState::kMLStatusConnected, MLMotionState::kMLStatusDisConnected);
        }
        return Result(false, QString("Close failed with error: %1").arg(WSAGetLastError()).toStdString());
    }

    m_socket = INVALID_SOCKET;
    if (pre)
    {
        //m_callback->NotifyMotionStateChanged(MLMotionState::kMLStatusConnected, MLMotionState::kMLStatusDisConnected);
    }
    return Result();
}

bool TcpModel::isOpened()
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

Result TcpModel::sendData(const string& data)
{
    char sendBuf[256];
    int len;
    memset(sendBuf, 0, 256);

    Result ret = start();
    if (!ret.success) {
        return ret;
    }

    {
        QString dataStr = QString::fromStdString(data);
        QByteArray dataArray = dataStr.toUtf8();
        uchar datalen[4];
        qToBigEndian(dataArray.length(), datalen);

        QByteArray bytes((char*)datalen, 4);
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

Result TcpModel::sendData(const ExternalTOSPathbody& data)
{
    Result ret = start(m_ip, m_port, true);
    if (!ret.success) {
        return ret;
    }

    std::string jsonData = createJson(data.use, data.csvPath, data.zipPath);

    {
        // Send data to the server
        int iResult = send(m_socket, jsonData.c_str(), jsonData.length(), 0);
        if (iResult == SOCKET_ERROR) {
            std::string errMsg = "ExternalTOSPathbody Received:" + WSAGetLastError();
            qWarning() << QString::fromStdString(errMsg);
            closesocket(m_socket);
            return Result(false, errMsg);
        }

        // Shutdown the send side of the socket
        iResult = shutdown(m_socket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            std::string errMsg = "ExternalTOSPathbody failed with error, " + WSAGetLastError();
            qWarning() << QString::fromStdString(errMsg);
            closesocket(m_socket);
            return Result(false, errMsg);
        }

    }

    // Receive data from the server
    char buffer[1024] = { 0 };
    int iResult = recv(m_socket, buffer, sizeof(buffer), 0);
    if (iResult > 0) {
        std::string response(buffer, iResult);
        qWarning() << "ExternalTOSPathbody Received:" << QString::fromStdString(response);
    }
    else if (iResult == 0)
        qWarning() << "ExternalTOSPathbody Connection closed gracefully";
    else {
        return Result(false, "ExternalTOSPathbody recv failed with error: " + WSAGetLastError());
    }

    // Clean up and close the socket
    closesocket(m_socket);
    return Result();
}

Result TcpModel::sendData_old(const ExternalTOSPathbody& data)
{
    Result ret = start();
    if (!ret.success) {
        return ret;
    }
    std::string jsonData = data.toJson().dump();
    // 尝试从 UTF-8 转换
    QString qJsonData = QString::fromUtf8(jsonData.c_str(), jsonData.size());

    // 将数据长度进行大端序列化
    QByteArray dataArray = qJsonData.toUtf8();
    quint32 dataLength = dataArray.length();

    uchar datalen[4];
    qToBigEndian(dataLength, datalen);

    // 准备发送缓冲区
    QByteArray bytes((char*)datalen, 4);
    bytes.append(dataArray);

    int len = bytes.length();

    // 發送數據
    int iResult = send(m_socket, bytes, len, 0);
    if (iResult == SOCKET_ERROR) {
        close();
        return Result(false, QString("Send failed with error: %1").arg(WSAGetLastError()).toStdString());
    }

    return Result();
}

Result TcpModel::sendData(const ExternalTOSPathToRDbody& data)
{
    Result ret = start();
    if (!ret.success) {
        return ret;
    }
    std::string jsonData = data.toJson().dump();
    // 尝试从 UTF-8 转换
    QString qJsonData = QString::fromUtf8(jsonData.c_str(), jsonData.size());

    // 将数据长度进行大端序列化
    QByteArray dataArray = qJsonData.toUtf8();
    quint32 dataLength = dataArray.length();

    uchar datalen[4];
    qToBigEndian(dataLength, datalen);

    // 准备发送缓冲区
    QByteArray bytes((char*)datalen, 4);
    bytes.append(dataArray);

    int len = bytes.length();

    // 發送數據
    int iResult = send(m_socket, bytes, len, 0);
    if (iResult == SOCKET_ERROR) {
        close();
        return Result(false, QString("Send failed with error: %1").arg(WSAGetLastError()).toStdString());
    }

    return Result();
}

Result TcpModel::queryRecv(string& recvInfo, const string& sendInfo)
{
    Result ret = sendData(sendInfo);
    if (!ret.success) {
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

Result TcpModel::queryRecv(string& recvInfo, const ExternalTOSPathbody& sendInfo)
{
    Result ret = sendData(sendInfo);
    if (!ret.success) {
        return ret;
    }

    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN] = "";

    int number = 10;
    //while (number-- > 0)
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
            return Result(false, QString("Recv failed with error: %1").arg(WSAGetLastError()).toUtf8().constData());
        }

        recvInfo = string(recvbuf, recvbuf + iResult);
    }
    
    return Result(true,recvInfo);
}

Result TcpModel::queryRecv(string& recvInfo, const ExternalTOSPathToRDbody& sendInfo)
{
    Result ret = sendData(sendInfo);
    if (!ret.success) {
        return ret;
    }

    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN] = "";

    int number = 10;
    //while (number-- > 0)
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
            QString errorMessage = QString("Recv failed with error: %1").arg(WSAGetLastError());
            return Result(false, errorMessage.toUtf8().constData());
        }

        recvInfo = string(recvbuf, recvbuf + iResult);
    }

    return Result(true, recvInfo);
}

Result TcpModel::start()
{
    return start(m_ip, m_port);
}

Result TcpModel::setWaitTime(SOCKET socket)
{
    int nTimeout = 3000;
    if (SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&nTimeout, sizeof(int)))
    {
        return Result(false, QString("Set SO_SNDTIMEO error: %1").arg(WSAGetLastError()).toStdString());
    }

    if (SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeout, sizeof(int)))
    {
        return Result(false, QString("Set SO_RCVTIMEO error: %1").arg(WSAGetLastError()).toStdString());
    }
    return Result();
}

std::string TcpModel::createJson(const std::string& use, const std::string& csvPath, const std::vector<std::string>& zipPaths) {
    std::ostringstream jsonStream;
    jsonStream << "{"
        << "\"_use\":\"" << use << "\","
        << "\"_csvPath\":\"" << csvPath << "\","
        << "\"_zipPath\":[";

    for (size_t i = 0; i < zipPaths.size(); ++i) {
        jsonStream << "\"" << zipPaths[i] << "\"";
        if (i < zipPaths.size() - 1) {
            jsonStream << ",";
        }
    }

    jsonStream << "]}";
    return jsonStream.str();
}

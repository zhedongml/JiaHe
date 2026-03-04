#include "integratingspheretcpmodel.h"
#include <QMutex>
#include <QProcess>
#include "PrjCommon/metricsdata.h"
#include <QtEndian>
#include <QFileInfo>
#include <QDataStream>
#include "Core/icore.h"
#include "PrjCommon/PrjCommon.h"
#include <QtConcurrent>
#include "loggingwrapper.h"
#include "PrjCommon/PrjCommon.h"

IntegratingSphereTCPModel* IntegratingSphereTCPModel::self = nullptr;
IntegratingSphereTCPModel* IntegratingSphereTCPModel::getInstance(QObject* parent) {
    if (!self) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new IntegratingSphereTCPModel(parent);
        }
    }
    return self;
}
IntegratingSphereTCPModel::IntegratingSphereTCPModel(QObject *parent)
	: QObject(parent)
{
    loadconfig("./config/device/Integratingsphere.json");
    _socket = new QTcpSocket();
    m_timer.setSingleShot(true);

}

IntegratingSphereTCPModel::~IntegratingSphereTCPModel()
{
    Disconnect();
}

QVector<ISSolution> IntegratingSphereTCPModel::getSolutions()
{
    return solutionsVec;
}

ISSolution IntegratingSphereTCPModel::getSolution(QString color)
{
    for (const auto& solution : solutionsVec)
    {
        if (solution.colorFocus == color.toUpper())
        {
            return solution;
        }
    };
}

bool IntegratingSphereTCPModel::changeSolution(QString name, double scale)
{
    QVector<ISSolution>::iterator it;
    if (scale < 0)
        scale = 0;
    for (it = solutionsVec.begin(); it != solutionsVec.end(); ++it) {
        ISSolution item = *it;
        if (item.name == name.trimmed()) {
            int idx = item.index;
            QString msgcontent = "solution " + QString::number(idx) + "," + QString::number(scale,'f',6);
            QString ok = writeData(msgcontent,"solution");
            emit reponseMsg(ok);
           if (ok.contains("solution")) {
                    return true;
            }
           else 
               return false;
                
        }
    }
    return false;
}

int IntegratingSphereTCPModel::queryCurSolution(double& scale)
{
    QString msgcontent = "solution?";
    QString ok = writeData(msgcontent, "solution");
    emit reponseMsg(ok);
    if (ok.contains("solution?")) {
        QStringList resLst = ok.split(";");
        if (resLst.size() >= 3) {
            QString errorCode = resLst[1];
            QStringList slu = resLst[2].split(",");
            double index = slu[0].toDouble();
            scale = slu[1].toDouble();
            return (int)index;
        }
        return -1;
    }
    else
        return -1;
}

bool IntegratingSphereTCPModel::isConnect()
{
    if (_socket->state() != QAbstractSocket::ConnectedState) {
        _socket->connectToHost(serverIp, port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
        if (!_socket->waitForConnected(2000))
        {
            _socket->disconnectFromHost();
            return false;
        }
        return true;
    }
    return true;
}

bool IntegratingSphereTCPModel::closeSoftware()
{
    QString msgcontent = "*stop";
    QString ok = writeData(msgcontent, "stop");
    emit reponseMsg(ok);
    return true;
}

bool IntegratingSphereTCPModel::turnColor(QString color)
{
    if (color == "R") {

    }
    
    return true;
}

QString IntegratingSphereTCPModel::writeData(QString data, QString cmd)
{
    if (!isConnect()) {
        return "";
    }
    if (isSend)
        return "";
    if (_socket->state() == QAbstractSocket::ConnectedState) {
        int datalen = data.length();
        int mLen = 0;
        while (datalen > 0)
        {
            QByteArray byteData = data.toLatin1();
            for (int i = 0; i <= 3; i++) {
                char c = datalen >> i * 8;
                byteData.insert(0, c);
            }
            mLen = (int)_socket->write(byteData);
            if (mLen > 0)
            {
                _socket->waitForBytesWritten(1000);
            }
            if (mLen == -1)
            {
                return "";
            }
            datalen -= mLen;
        }
        int count = 0;
        while (1)
        {
            isSend = true;
            if (_socket->waitForReadyRead(1500))
            {
                QByteArray data = _socket->readAll();
                QString msg = QString::fromLatin1(data);
                isSend = false;
                return msg;
                
            }
            else {
                isSend = false;
                return "";
            }
        }
    }
    return "";
}
void IntegratingSphereTCPModel::loadconfig(std::string filename)
{
    std::ifstream jsonFile(filename);
    if (!jsonFile.is_open()) {
        qCritical() << QString::fromStdString(filename) << "open failed.";
        return;
    }

    std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
    jsonFile.close();
    Json settingJsonObj = Json::parse(contents);
    float scale = settingJsonObj["Scale"].get<float>();

    Json solus = settingJsonObj["Solutions"];
    if (!solus.is_null())
    {
        int size = solus.size();
        int curId = 0;
        for (auto it = solus.begin(); it != solus.end(); ++it)
        {
            Json &val = it.value();
            ISSolution item;
            item.index = val["index"].get<int>();
            item.name = QString::fromStdString(val["name"].get<std::string>());
            item.scale = scale;
            item.colorFocus = QString::fromStdString(val["colorFocus"].get<std::string>());

            QStringList currents = QString::fromStdString(val["currentOutput"].get<std::string>()).split(",");
            for (QString current : currents){
                item.currentOutputs.push_back(current.toFloat());
            }

            QStringList steady_currents = QString::fromStdString(val["steadyCurrent"].get<std::string>()).split(",");
            for (QString steadycurrent : steady_currents) {
                item.steadyCurrent.push_back(steadycurrent.toFloat());
            }
            solutionsVec.append(item);
        }
    }
    Json hostip = settingJsonObj["HostIP"];
    if (!hostip.is_null())
    {
        std::string ipstr = hostip.get<std::string>();
        serverIp = QString::fromStdString(ipstr);
    }
    Json portVal = settingJsonObj["Port"];
    if (!portVal.is_null())
    {
        port = portVal.get<int>();
    }
    Json jRCalibrationF = settingJsonObj["RedCalibrationFactor"];
    if (!jRCalibrationF.is_null())
    {
        RCalibrationFactor = jRCalibrationF.get<double>();
    }
    Json jGCalibrationF = settingJsonObj["GreenCalibrationFactor"];
    if (!jGCalibrationF.is_null())
    {
        GCalibrationFactor = jGCalibrationF.get<double>();
    }
    Json jBCalibrationF = settingJsonObj["BlueCalibrationFactor"];
    if (!jBCalibrationF.is_null())
    {
        BCalibrationFactor = jBCalibrationF.get<double>();
    }

    m_useShutter = settingJsonObj["UseShutter"].get<bool>();

    m_diviceIndexR = settingJsonObj["DeviceIndex"]["R"].get<int>();
    m_diviceIndexG = settingJsonObj["DeviceIndex"]["G"].get<int>();
    m_diviceIndexB = settingJsonObj["DeviceIndex"]["B"].get<int>();

    m_shutterIndexR = settingJsonObj["ShutterIndex"]["R"].get<int>();
    m_shutterIndexG = settingJsonObj["ShutterIndex"]["G"].get<int>();
    m_shutterIndexB = settingJsonObj["ShutterIndex"]["B"].get<int>();

    m_whiteRatioR = settingJsonObj["WhiteRatio"]["R"].get<float>();
    m_whiteRatioG = settingJsonObj["WhiteRatio"]["G"].get<float>();
    m_whiteRatioB = settingJsonObj["WhiteRatio"]["B"].get<float>();

    if (settingJsonObj.contains("SteadyCurrentCoefficient")) {
        useSteadyCurrentCoefficient = settingJsonObj["SteadyCurrentCoefficient"]["useCoefficient"].get<bool>();
        m_coefficientR = settingJsonObj["SteadyCurrentCoefficient"]["R"].get<float>();
        m_coefficientG = settingJsonObj["SteadyCurrentCoefficient"]["G"].get<float>();
        m_coefficientB = settingJsonObj["SteadyCurrentCoefficient"]["B"].get<float>();
    }

    if (settingJsonObj.contains("Voltage")) {
        VR = settingJsonObj["Voltage"]["R"].get<float>();
        VG = settingJsonObj["Voltage"]["G"].get<float>();
        VB = settingJsonObj["Voltage"]["B"].get<float>();
    }

    if (settingJsonObj.contains("VoltageCompliance")) {
        VCR = settingJsonObj["VoltageCompliance"]["R"].get<float>();
        VCG = settingJsonObj["VoltageCompliance"]["G"].get<float>();
        VCB = settingJsonObj["VoltageCompliance"]["B"].get<float>();
    }

    if (settingJsonObj.contains("CurrentSetting")) {
        m_currentSettingJudgment = settingJsonObj["CurrentSetting"]["ResultJudge"].get<bool>();
        m_waitMillisecondsAfterSet = settingJsonObj["CurrentSetting"]["WaitMilliseconds"].get<int>();
    }

    //QString rInfo = QString::fromStdString(settingJsonObj["TestInfoR"].get<std::string>());
    //QString gInfo = QString::fromStdString(settingJsonObj["TestInfoG"].get<std::string>());
    //QString bInfo = QString::fromStdString(settingJsonObj["TestInfoB"].get<std::string>());
    //QString info = rInfo + "\n" + gInfo + "\n" + bInfo + "\n";
    //MetricsData::instance()->setIGParamInfo(info);
}

Color IntegratingSphereTCPModel::switchColorEnum(const QString &enColor)
{
    Color color;
    if (enColor.toUpper() == "R")
    {
        color = R;
    }
    else if (enColor.toUpper() == "G")
    {
        color = G;
    }
    else if (enColor.toUpper() == "B")
    {
        color = B;
    }
    else
    {
        color = W;
    }
    return color;
}

void IntegratingSphereTCPModel::updateUpper(QMap<QString, float> &map)
{
    QMap<QString, float> newMap;
    QMap<QString, float>::iterator iter;
    for (iter = map.begin(); iter != map.end(); ++iter)
    {
        newMap[iter.key().toUpper()] = iter.value();
    }
    map = newMap;
}

bool IntegratingSphereTCPModel::isEqual(QMap<QString, float> valueMap)
{
    return true;
    float cR = GetColorCurrent(R);
    float cG = GetColorCurrent(G);
    float cB = GetColorCurrent(B);

    bool retR = true;
    bool retG = true;
    bool retB = true;

    if (qAbs(cR - valueMap["R"]) > 0.0001f)
    {
        retR = false;
    }

    if (qAbs(cG - valueMap["G"]) > 0.0001f)
    {
        retG = false;
    }

    if (qAbs(cB - valueMap["B"]) > 0.0001f)
    {
        retG = false;
    }

    return retR && retG && retB;
}

Color IntegratingSphereTCPModel::getColor()
{
    QMap<QString, float> valueMap;
    valueMap["R"] = m_currentR;
    valueMap["G"] = m_currentG;
    valueMap["B"] = m_currentB;
    return getColor(valueMap);
}

Color IntegratingSphereTCPModel::getColor(QMap<QString, float> valueMap)
{
    if (!m_useShutter) {
        return getColorCurrent(valueMap);
    }

    Color shutter = getColorShutter(m_shutterOpenMap);
    QString colorStr = "W";
    switch (shutter)
    {
    case CORE::R:
        colorStr = "R";
        break;
    case CORE::G:
        colorStr = "G";
        break;
    case CORE::B:
        colorStr = "B";
        break;
    case CORE::W:
        colorStr = "W";
        break;
    default:
        break;
    }

    if (abs(valueMap[colorStr]) > 1e-6) {
        return shutter;
    }
    return W;
}

Color IntegratingSphereTCPModel::getColor(const QMap<QString, bool>& valueMap)
{
    bool red = valueMap["R"];
    bool green = valueMap["G"];
    bool blue = valueMap["B"];

    if (red && !green && !blue) {
        return R;
    }
    else if (green && !red && !blue) {
        return G;
    }
    else if (blue && !green && !red) {
        return B;
    }

    return W;
}

QString IntegratingSphereTCPModel::getColorStr(const QMap<QString, bool>& valueMap)
{
    bool red = valueMap["R"];
    bool green = valueMap["G"];
    bool blue = valueMap["B"];

    if (red && !green && !blue) {
        return "R";
    }
    else if (green && !red && !blue) {
        return "G";
    }
    else if (blue && !green && !red) {
        return "B";
    }

    return "W";
}

Color IntegratingSphereTCPModel::getColorCurrent()
{
    QMap<QString, float> valueMap;
    valueMap["R"] = m_currentR;
    valueMap["G"] = m_currentG;
    valueMap["B"] = m_currentB;
    return getColorCurrent(valueMap);
}

Color IntegratingSphereTCPModel::getColorCurrent(QMap<QString, float> valueMap)
{
    bool red = false;
    bool green = false;
    bool blue = false;

    if(valueMap.contains("R") && abs(valueMap["R"]) > 1e-6){
        red = true;
    }
    if (valueMap.contains("G") && abs(valueMap["G"]) > 1e-6) {
        green = true;
    }
    if (valueMap.contains("B") && abs(valueMap["B"]) > 1e-6) {
        blue = true;
    }

    if(red && !green && !blue){
        return R;
    }
    else if (green && !red && !blue) {
        return G;
    }
    else if (blue && !green && !red) {
        return B;
    }

    return W;
}

Color IntegratingSphereTCPModel::getColorShutter(QMap<QString, bool> shutterOpenMap)
{
    bool red = false;
    bool green = false;
    bool blue = false;

    if (shutterOpenMap.contains("R") && abs(shutterOpenMap["R"])) {
        red = true;
    }
    if (shutterOpenMap.contains("G") && abs(shutterOpenMap["G"])) {
        green = true;
    }
    if (shutterOpenMap.contains("B") && abs(shutterOpenMap["B"])) {
        blue = true;
    }

    if(red && !green && !blue){
        return R;
    }
    else if (green && !red && !blue) {
        return G;
    }
    else if (blue && !green && !red) {
        return B;
    }

    return W;
}

bool IntegratingSphereTCPModel::LoadDll()
{
    if (nullptr != g_pfnApiCall)
        return true;
    // Here we demonstrate loading the DLL dynamically, via WinAPI calls LoadLibrary() and GetProcAddress(). (The alternative is to statically link to the DLL's "importlib".)
    hDll = LoadLibraryA("lsapi_x64.dll");

    if (hDll)
    {
        char sz[MAX_PATH];
        GetModuleFileNameA(hDll, sz, sizeof(sz));

        // We make a single call to GetProcAddress to get the one and only exported function:
        g_pfnApiCall = (PFN_lsapi_ApiCall)GetProcAddress(hDll, "lsapi_ApiCall");
    }
    else
    {
        return false;
    }
    return (nullptr != g_pfnApiCall);    
}
bool IntegratingSphereTCPModel::GetDeviceInfo(lsapi_DeviceHandleT hDevice)
{
    lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params infoParams;
    memset(&infoParams, 0, sizeof(infoParams));
    infoParams.CtlHdr.ApiCallHdr.hdr.StructSize = sizeof(infoParams);
    infoParams.CtlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice;
    infoParams.CtlHdr.DeviceClass = lsapi_device_DeviceClass_Generic;
    infoParams.CtlHdr.SubCmd = lsapi_device_Control_Generic_GetDeviceInfo;
    infoParams.CtlHdr.hDevice = hDevice;
    lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&infoParams);
    return LSAPISUCCESS(rslt);
}
bool IntegratingSphereTCPModel::OpenAPI()
{
    lsapi_ApiCall_Api_Open_Params openParams;
    memset(&openParams, 0, sizeof(openParams));
    openParams.ApiCallHdr.hdr.StructSize = sizeof(lsapi_ApiCall_Api_Open_Params);
    openParams.ApiCallHdr.ApiCall = lsapi_ApiCall_Api_Open;
    lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&openParams);

    bool openedOk = (lsapi_Result_Success == rslt);
    // This alternate syntax, using LSAPISUCCESS macro from <lsapi.h>, is a little more succinct:
    openedOk = LSAPISUCCESS(rslt);
    return openedOk;
}

Result IntegratingSphereTCPModel::SetColorIlluminate(const QString &enColor, double fValue){

    Color color = switchColorEnum(enColor);
    return SetColorIlluminate(color, fValue);
}

Result IntegratingSphereTCPModel::SetColorIlluminate(Color enColor, double fValue)
{
    if (!bIsconnect)
    {
        return Result(false, "Integrating sphere is not connected.");
    }

    double fR = fValue * m_whiteRatioR;
    double fG = fValue * m_whiteRatioG;
    double fB = fValue * m_whiteRatioB;          

    QString colorStr;
    bool bs1, bs2, bs3 = true;
    switch (enColor)
    {
    case W:
        colorStr = "W";
        for (int i = 0; i < 3; i++)
        {
            ControlOutputEnable(g_hDevice[i], false);
        }

        ControlOutputEnable(g_hDevice[m_diviceIndexR], true);
        SetVoltageOutput(g_hDevice[m_diviceIndexR], VR);
        bs1 = SetCurrentOutput(g_hDevice[m_diviceIndexR], 1.23);

        ControlOutputEnable(g_hDevice[m_diviceIndexG], true);
        SetVoltageOutput(g_hDevice[m_diviceIndexG], VG);
        bs2 = SetCurrentOutput(g_hDevice[m_diviceIndexG], 1.37);

        ControlOutputEnable(g_hDevice[m_diviceIndexB], true);
        SetVoltageOutput(g_hDevice[m_diviceIndexB], VB);
        // TODO: to be done
        bs3 = SetCurrentOutput(g_hDevice[m_diviceIndexB], 1.47);

        break;
    case R:
        colorStr = "R";

        ControlOutputEnable(g_hDevice[m_diviceIndexR], true);
        ControlOutputEnable(g_hDevice[m_diviceIndexB], false);
        ControlOutputEnable(g_hDevice[m_diviceIndexG], false);
        SetVoltageOutput(g_hDevice[m_diviceIndexR], VR);
        bs1 = SetCurrentOutput(g_hDevice[m_diviceIndexR], fValue / RCalibrationFactor);
        break;
    case G:
        colorStr = "G";

        ControlOutputEnable(g_hDevice[m_diviceIndexR], false);
        ControlOutputEnable(g_hDevice[m_diviceIndexG], true);
        ControlOutputEnable(g_hDevice[m_diviceIndexB], false);
        SetVoltageOutput(g_hDevice[m_diviceIndexG], VG);
        bs2 = SetCurrentOutput(g_hDevice[m_diviceIndexG], fValue / GCalibrationFactor);
        break;
    case B:
        colorStr = "B";

        ControlOutputEnable(g_hDevice[m_diviceIndexR], false);
        ControlOutputEnable(g_hDevice[m_diviceIndexG], false);
        ControlOutputEnable(g_hDevice[m_diviceIndexB], true);
        SetVoltageOutput(g_hDevice[m_diviceIndexB], VB);
        bs3 = SetCurrentOutput(g_hDevice[m_diviceIndexB], fValue / BCalibrationFactor);
        break;
    default:
        break;
    }
    if (bs1 && bs2 && bs3)
    {
        //MetricsData::instance()->updateColor(colorStr);
        return Result();
    }
    else
    {
        return Result(false, QString("Integrating sphere set failed, color=%1, value=%2.")
            .arg(enColor).arg(fValue).toStdString());
    }
}
double IntegratingSphereTCPModel::GetColorIlluminate(Color enColor)
{
    double flluminate = 0.0;
    double fCurrent = 0.0;
    //fCurrent = GetColorCurrent(enColor);
    double fCurrentR = GetColorCurrent(Color::R);
    double fCurrentG = GetColorCurrent(Color::G);
    double fCurrentB = GetColorCurrent(Color::B);
    switch (enColor)
    {
    case W:
        fCurrent = fCurrentR * m_whiteRatioR + fCurrentG * m_whiteRatioG + fCurrentB * m_whiteRatioB;
        //flluminate = fCurrent * WCalibrationFactor;
        break;
    case R:
        flluminate = fCurrentR * RCalibrationFactor;
        break;
    case G:
        flluminate = fCurrentG * GCalibrationFactor;
        break;
    case B:
        flluminate = fCurrentB * BCalibrationFactor;
        break;
    default:
        break;
    }
    return flluminate;

}
double IntegratingSphereTCPModel::GetColorCurrent(Color enColor)
{
    double fCurrent = 0.0;
    if (bIsconnect)
    {
        switch (enColor)
        {
        case W:
            //fCurrent = GetCurrentOutput(g_hDevice);
            break;
        case R:
            fCurrent = GetCurrentOutput(g_hDevice[m_diviceIndexR]);
            break;
        case G:
            fCurrent = GetCurrentOutput(g_hDevice[m_diviceIndexG]);
            break;
        case B:
            fCurrent = GetCurrentOutput(g_hDevice[m_diviceIndexB]);
            break;
        default:
            break;
        }
    }
    return fCurrent;
}
double IntegratingSphereTCPModel::GetColorVoltage(Color enColor)
{
    double fVoltage = 0.0;
    if (bIsconnect)
    {
        switch (enColor)
        {
        case W:
            //fVoltage = GetVoltageOutput(g_hDevice);
            break;
        case R:
            fVoltage = GetVoltageOutput(g_hDevice[m_diviceIndexR]);
            break;
        case G:
            fVoltage = GetVoltageOutput(g_hDevice[2]);
            break;
        case B:
            fVoltage = GetVoltageOutput(g_hDevice[1]);
            break;
        default:
            break;
        }
    }
    return fVoltage;
}
Result IntegratingSphereTCPModel::setCurrentOutput(const QString &enColor, float fValue)
{
    int startTime = QDateTime::currentMSecsSinceEpoch();

    QMap<QString, float> valueMap;
    if (enColor.toUpper() == "R")
    {
        valueMap["R"] = fValue;
    }
    else if (enColor.toUpper() == "G")
    {
        valueMap["G"] = fValue;
    }
    else if (enColor.toUpper() == "B")
    {
        valueMap["B"] = fValue;
    }
    else
    {
        return Result(false, "Set current output color error, color is " + enColor.toUpper().toStdString());
    }

    Result ret;
    if (m_useShutter)
    {
        ret = setCurrentOutput(enColor, valueMap);
    }
    else
    {
        ret = setCurrentOutput_noShutter(enColor, valueMap);
    }

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "Integrating sphere set "  << enColor<< ", times:" << takeTime << " ms...";
    return ret;
}
Result IntegratingSphereTCPModel::setCurrentOutput_noShutter(const QString &enColor, QMap<QString, float> valueMap,bool isJudgeSet)
{
    int startTime = QDateTime::currentMSecsSinceEpoch();

    if (!(enColor.toUpper() == "R" || enColor.toUpper() == "G" || enColor.toUpper() == "B" || enColor.toUpper() == "W"))
    {
        return Result(false, "Set current output error, color is " + enColor.toStdString());
    }

    if (enColor.toUpper() == "R")
    {
        valueMap["G"] = 0.0;
        valueMap["B"] = 0.0;
    }
    else if (enColor.toUpper() == "G")
    {
        valueMap["R"] = 0.0;
        valueMap["B"] = 0.0;
    }
    else if (enColor.toUpper() == "B")
    {
        valueMap["R"] = 0.0;
        valueMap["G"] = 0.0;
    }

    Result ret = setCurrentOutput_noShutter(valueMap, isJudgeSet);

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "Integrating sphere set " << enColor << ", times:" << takeTime << " ms...";
    return ret;
}

Result IntegratingSphereTCPModel::setCurrentOutput(const QString& enColor, QMap<QString, float> valueMap, bool isJudgeSet)
{
    int startTime = QDateTime::currentMSecsSinceEpoch();

    if (!(enColor.toUpper() == "R" || enColor.toUpper() == "G" || enColor.toUpper() == "B" || enColor.toUpper() == "W"))
    {
        return Result(false, "Set current output error, color is " + enColor.toStdString());
    }
    QMap<QString, QString> otherColors;
    otherColors["R"] = "GB";
    otherColors["G"] = "RB";
    otherColors["B"] = "RG";

    if (!useSteadyCurrentCoefficient)
    {
        if (enColor.toUpper() == "R")
        {
            valueMap["G"] = getSolution("G").steadyCurrent[0];
            valueMap["B"] = getSolution("B").steadyCurrent[0];
        }
        else if (enColor.toUpper() == "G")
        {
            valueMap["R"] = getSolution("R").steadyCurrent[0];
            valueMap["B"] = getSolution("B").steadyCurrent[0];
        }
        else if (enColor.toUpper() == "B")
        {
            valueMap["R"] = getSolution("R").steadyCurrent[0];
            valueMap["G"] = getSolution("G").steadyCurrent[0];
        }
    }
    else
    {
        if (enColor.toUpper() == "R")
        {
            valueMap["G"] = getSolution("G").currentOutputs[0] * m_coefficientG;
            valueMap["B"] = getSolution("B").currentOutputs[0] * m_coefficientB;
        }
        else if (enColor.toUpper() == "G")
        {
            valueMap["R"] = getSolution("R").currentOutputs[0] * m_coefficientR;
            valueMap["B"] = getSolution("B").currentOutputs[0] * m_coefficientB;
        }
        else if (enColor.toUpper() == "B")
        {
            valueMap["R"] = getSolution("R").currentOutputs[0] * m_coefficientR;
            valueMap["G"] = getSolution("G").currentOutputs[0] * m_coefficientG;
        }
    }
    Result ret = setCurrentOutput(valueMap, isJudgeSet);
    if (!ret.success)
    {
        return ret;
    }
    
    ret = setShutterTurn(enColor.toUpper(), true, true);

    if (enColor.toUpper() != "W")
    {
        for (QChar c : otherColors[enColor.toUpper()].toUtf8()) {
            ret = setShutterTurn(c, false, true);
        }
    }

    
    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "Integrating sphere set " << enColor << ", times:" << takeTime << " ms...";

    if (m_currentSettingJudgment) {
        Sleep(m_waitMillisecondsAfterSet);
    }
    return ret;
}

Result IntegratingSphereTCPModel::setCurrentOutput(QMap<QString, float> valueMap, bool isJudgeSet)
{
    if (!bIsconnect) {
        return Result(false, "Integrating sphere is not connected.");
    }

    updateUpper(valueMap);

    if (!valueMap.contains("R") || !valueMap.contains("G") || !valueMap.contains("B"))
    {
        return Result(false, "Set current output error, info is incomplete.");
    }

    QString enColor = "";
    bool bs1 = true;
    bool bs2 = true;
    bool bs3 = true;

    float newR = valueMap.value("R");
    bool needSetR = !(isJudgeSet && qFuzzyCompare(m_currentR, newR));
    if (needSetR)
    {
        //ControlOutputEnable(g_hDevice[m_diviceIndexR], true);
        SetVoltageOutput(g_hDevice[m_diviceIndexR], VR);
        bs1 = SetCurrentOutput(g_hDevice[m_diviceIndexR], newR);
        enColor = "R";
    }
    float newG = valueMap.value("G");
    bool needSetG = !(isJudgeSet && qFuzzyCompare(m_currentG, newG));
    if (needSetG)
    {
        //ControlOutputEnable(g_hDevice[m_diviceIndexG], true);
        SetVoltageOutput(g_hDevice[m_diviceIndexG], VG);
        bs2 = SetCurrentOutput(g_hDevice[m_diviceIndexG], newG);
        enColor = enColor.isEmpty() ? "G" : "W";
    }
    float newB = valueMap.value("B");
    bool needSetB = !(isJudgeSet && qFuzzyCompare(m_currentB, newB));
    if (needSetB)
    {
        //ControlOutputEnable(g_hDevice[m_diviceIndexB], true);
        SetVoltageOutput(g_hDevice[m_diviceIndexB], VB);
        bs3 = SetCurrentOutput(g_hDevice[m_diviceIndexB], newB);
        enColor = enColor.isEmpty() ? "B" : "W";
    }

    enColor = enColor.isEmpty() ? "W" : enColor;

    if (bs1 && bs2 && bs3)
    {
        m_currentR = valueMap["R"];
        m_currentG = valueMap["G"];
        m_currentB = valueMap["B"];
        emit refreshData(m_currentR, m_currentG, m_currentB);

        return Result();
    }
    else
    {
        return Result(false, QString("Integrating sphere set failed, color=%1.").arg(enColor).toStdString());
    }

    return Result();
}

Result IntegratingSphereTCPModel::setCurrentOutput_noShutter(QMap<QString, float> valueMap, bool isJudgeSet)
{
    if (!bIsconnect){
        return Result(false, "Integrating sphere is not connected.");
    }

    updateUpper(valueMap);

    if (!valueMap.contains("R") || !valueMap.contains("G") || !valueMap.contains("B"))
    {
        return Result(false, "Set current output error, info is incomplete.");
    }
    bool hasAnyZero = false;

    for (auto it = valueMap.cbegin(); it != valueMap.cend(); ++it)
    {
        if (qFuzzyIsNull(it.value()))
        {
            hasAnyZero = true;
            break;
        }
    }

    if (hasAnyZero)
    {
        for (int i = 0; i < 3; i++)
        {
            ControlOutputEnable(g_hDevice[i], false);
        }
    }

    QString enColor = "";
    bool bs1 = true;
    bool bs2 = true;
    bool bs3 = true;

    float newR = valueMap.value("R");
    bool needSetR = !(isJudgeSet && qFuzzyCompare(m_currentR, newR));
    if (!qFuzzyIsNull(newR) && needSetR)
    {
        ControlOutputEnable(g_hDevice[m_diviceIndexR], true);
        SetVoltageOutput(g_hDevice[m_diviceIndexR], VR);
        bs1 = SetCurrentOutput(g_hDevice[m_diviceIndexR], newR);
        enColor = "R";
    }
    float newG = valueMap.value("G");
    bool needSetG = !(isJudgeSet && qFuzzyCompare(m_currentG, newG));
    if (!qFuzzyIsNull(newG) && needSetG)
    {
        ControlOutputEnable(g_hDevice[m_diviceIndexG], true);
        SetVoltageOutput(g_hDevice[m_diviceIndexG], VG);
        bs2 = SetCurrentOutput(g_hDevice[m_diviceIndexG], newG);
        enColor = enColor.isEmpty() ? "G":"W";
    }
    float newB = valueMap.value("B");
    bool needSetB = !(isJudgeSet && qFuzzyCompare(m_currentB, newB));
    if (!qFuzzyIsNull(newB) && needSetB)
    {
        ControlOutputEnable(g_hDevice[m_diviceIndexB], true);
        SetVoltageOutput(g_hDevice[m_diviceIndexB], VB);
        bs3 = SetCurrentOutput(g_hDevice[m_diviceIndexB], newB);
        enColor = enColor.isEmpty() ? "B" : "W";
    }

    enColor = enColor.isEmpty() ? "W" : enColor;

    if (bs1 && bs2 && bs3)
    {
        if(m_currentSettingJudgment){
            int number = 100;
            while (!isEqual(valueMap) && number-- > 0)
            {
                Sleep(100);
            }
        }else{
            Sleep(m_waitMillisecondsAfterSet);
        }

        //MetricsData::instance()->updateColor(enColor.toUpper());
        m_lastColor = getColor(valueMap);

        emit refreshData(0, 0, 0);
        emit Core::PrjCommon::instance()->updateLabsphereColor(enColor.toUpper());

        //QtConcurrent::run([this]() {
        //    double r = GetColorCurrent(Color::R);
        //    double g = GetColorCurrent(Color::G);
        //    double b = GetColorCurrent(Color::B);
        //    int count = 0;
        //    while (count < 10) {
        //        QThread::msleep(100);
        //    }
        //    });

        m_currentR = valueMap["R"];
        m_currentG = valueMap["G"];
        m_currentB = valueMap["B"];
        return Result();
    }
    else
    {
        return Result(false, QString("Integrating sphere set failed, color=%1.").arg(enColor).toStdString());
    }

    // TODO: not used
    //if (abs(valueMap["R"]) < 0.001 || abs(valueMap["G"]) < 0.001 || abs(valueMap["B"]) < 0.001)
    //{
    //    MetricsData::instance()->updateColor("");
    //}

    return Result();
}
bool IntegratingSphereTCPModel::controlOneShutter(int shutter_index,const QString& color,bool isOn,bool isJudge)
{
    if (isJudge && m_shutterOpenMap.value(color) == isOn) {
        return true;
    }

    Sleep(300);

    bool ok = true;

    if (isOn) {
        ok &= TurnOnLamp(l_hDevice[shutter_index]);
    }
    else {
        ok &= TurnOffLamp(l_hDevice[shutter_index]);
    }

    if (ok) {
        m_shutterOpenMap[color.toUpper()] = isOn;

        QString color = getColorStr(m_shutterOpenMap);
        //MetricsData::instance()->updateColor(color.toUpper());
        m_lastColor = getColor(m_shutterOpenMap);

        //emit refreshData(0, 0, 0);
        emit Core::PrjCommon::instance()->updateLabsphereColor(color.toUpper());
    }

    return ok;
}

Result IntegratingSphereTCPModel::setShutterTurn(QString color, bool isOn, bool isJudge)
{
    color = color.toUpper();
    bool res = true;
    if (color == "R")
    {
        res = controlOneShutter(m_shutterIndexR, color, isOn, isJudge);
    }
    else if (color == "G")
    {
        res = controlOneShutter(m_shutterIndexG, color, isOn, isJudge);
    }
    else if (color == "B")
    {
        res = controlOneShutter(m_shutterIndexB, color, isOn, isJudge);
    }
    else if (color == "W")
    {
        res = controlOneShutter(m_shutterIndexR, "R", isOn, isJudge)
            && controlOneShutter(m_shutterIndexG, "G", isOn, isJudge)
            && controlOneShutter(m_shutterIndexB, "B", isOn, isJudge);
    }
    else
    {
        return Result(false, "Invalid lamp color.");
    }

    if (res) {
        m_lastColor = getColor();
    }
    else
    {
        return Result(false,
            QString("Lamp control failed: %1 lamp (%2).")
            .arg(isOn ? "turn ON" : "turn OFF")
            .arg(color)
            .toStdString());
    }
    emit shutterStatus(isOn, color);
    emit refreshData(m_currentR, m_currentG, m_currentB);
    return Result();
}

Result IntegratingSphereTCPModel::setOtherLampStableCurrent(QString color)
{
    if (color != "W") return Result();

    ISSolution nowSolution = getSolution(color);
    bool ok = false;

    if (color != "R") {
        ISSolution nowSolution = getSolution("R");
        ok &= SetCurrentOutput(g_hDevice[m_diviceIndexR], nowSolution.steadyCurrent[0]);
        if (ok)
        {
            m_currentR = nowSolution.steadyCurrent[0];
        }
    }
      
    if (color != "G") {
        ISSolution nowSolution = getSolution("G");
        ok &= SetCurrentOutput(g_hDevice[m_diviceIndexR], nowSolution.steadyCurrent[0]);
        if (ok)
        {
            m_currentG = nowSolution.steadyCurrent[0];
        }
    }

    if (color != "B") {
        ISSolution nowSolution = getSolution("B");
        ok &= SetCurrentOutput(g_hDevice[m_diviceIndexR], nowSolution.steadyCurrent[0]);
        if (ok)
        {
            m_currentB = nowSolution.steadyCurrent[0];
        }
    }

    if (!ok)
        return Result(false, "Failed to set stable current for other lamps.");

    return Result();
}

Result IntegratingSphereTCPModel::setAllCurrentOutputZero()
{
   
    if(abs(m_currentR) < 1e-5 && abs(m_currentG) < 1e-5 && abs(m_currentB) < 1e-5){
        return Result();
    }
    

    if (!bIsconnect){
        return Result(false, "Integrating Sphere is not connected.");
    }

   Result res = setDeviceEnable(false);
	if(!res.success){
	return res;
	}

    emit refreshData(0, 0, 0);

    m_currentR = 0.0f;
    m_currentG = 0.0f;
    m_currentB = 0.0f;
    return Result();
}
Result IntegratingSphereTCPModel::setDeviceEnable(bool enable)
{
    for (int i = 0; i < 3; i++)
    {
        if (!ControlOutputEnable(g_hDevice[i], enable))
        {
            return Result(
                false,
                QString("Failed to %1 output on device index %2.")
                .arg(enable ? "enable" : "disable")
                .arg(i)
                .toStdString()
            );
        }
    }
    return Result();
}
Result IntegratingSphereTCPModel::setSolution(const ISSolution &solution)
{
    Result ret;
    if (solution.colorFocus.toUpper() != "W")
    {
        ret = setCurrentOutput(solution.colorFocus, solution.currentOutputs.at(0));
    }
    else
    {
        QMap<QString, float> map;
        map["R"] = solution.currentOutputs.at(0);
        map["G"] = solution.currentOutputs.at(1);
        map["B"] = solution.currentOutputs.at(2);

        if (m_useShutter)
        {
            ret = setCurrentOutput(solution.colorFocus, map);
        }
        else
        {
            ret = setCurrentOutput_noShutter(solution.colorFocus, map);
        }
    }
    return ret;
}
Result IntegratingSphereTCPModel::setSolution(const QString &name)
{
    for (ISSolution sol : solutionsVec)
    {
        if (name.toUpper() == sol.name)
        {
            return setSolution(sol);
        }
    }

    return Result(false, QString("Integrating sphere set solution error, solution name  %1 is not exist.").arg(name).toStdString());
}

Result IntegratingSphereTCPModel::setFan(bool open)
{
    if (!bIsconnect)
    {
        return Result(false, "Integrating sphere is not connected.");
    }

    for (int i = 0; i < 4; i++)
    {
        lsapi_ApiCall_Device_ControlDevice_TempController_Item_Params psParams;
        memset(&psParams, 0, sizeof(psParams));
        psParams.CtlHdr.ApiCallHdr.hdr.StructSize = sizeof(psParams);
        psParams.CtlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice;
        psParams.CtlHdr.DeviceClass = lsapi_device_DeviceClass_TempController;
        psParams.CtlHdr.SubCmd = lsapi_device_Control_TempController_Item;
        psParams.CtlHdr.hDevice = m_hDeviceFan[i]; // Handle from ::lsapi_ApiCall_Device_ConnectDevice

        psParams.Set = open; // Instruct the API to process 'Value'
        lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr *)&psParams);

        if (rslt != lsapi_Result_Success)
        {
            return open ? Result(false, "Integrating sphere open failed.") : Result(false, "Integrating sphere close failed.");
        }
    }
    return Result();
}

Result IntegratingSphereTCPModel::connectDetect(lsapi_device_DeviceInfo deviceInfo)
{
    // Use the Catalog's info to connect to the device      
    lsapi_ApiCall_Device_ConnectDevice_Params connParams;
    memset(&connParams, 0, sizeof(connParams));
    connParams.ApiCallHdr.hdr.StructSize = sizeof(lsapi_ApiCall_Device_ConnectDevice_Params);
    connParams.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ConnectDevice;

    connParams.DvcInfo = deviceInfo;   // Grab the info straight from the catalog       
    lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&connParams);
    if (LSAPISUCCESS(rslt))
    {
        g_hDevice[3] = connParams.hDevice;
        return Result();
    }

    return Result(false, "Integrating sphere detector device connect failed.");
}

Result IntegratingSphereTCPModel::getLuminance(float &luminance, Color color)
{
    color = m_lastColor;
    if(color == W){
        return Result(false, "Integrating sphere is not red,green,blue.", -1);
    }

    lsapi_ApiCall_Device_ControlDevice_Detector_Item_Params detParams;
    memset(&detParams, 0, sizeof(detParams));
    detParams.CtlHdr.ApiCallHdr.hdr.StructSize = sizeof(detParams);
    detParams.CtlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice;
    detParams.CtlHdr.DeviceClass = lsapi_device_DeviceClass_Detector;
    detParams.CtlHdr.SubCmd = lsapi_device_Control_Detector_Item;
    detParams.CtlHdr.hDevice = g_hDevice[m_diviceIndexDetect];
    // Handle from ::lsapi_ApiCall_Device_ConnectDevice

    detParams.Item = lsapi_device_Detector_Item_Read;
    detParams.Set = false;          // Instruct the API to populate 'Value'
    lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&detParams);
    if (!LSAPISUCCESS(rslt))
    {
        return Result(false, "Integrating sphere query luminance failed.");
    }

    if(color == R){
        luminance = detParams.Value * RCalibrationFactor;
    }
    else if (color == G) {
        luminance = detParams.Value * GCalibrationFactor;
    }
    else if (color == B) {
        luminance = detParams.Value * BCalibrationFactor;
    }
    return Result();
}

QList<QString> IntegratingSphereTCPModel::getColorCurrentByConfig(const QString& enColor)
{
    QList<float> currentList;
    for (ISSolution sol : solutionsVec)
    {
        if (enColor.toUpper() == sol.colorFocus.toUpper())
        {
            currentList = sol.currentOutputs;
            break;
        }
    }

    QList<QString> strList;
    for (float current : currentList){
        strList.append(QString::number(current, 'f', 3));
    }
    return strList;
}

Result IntegratingSphereTCPModel::Connect()
{
    int startTime = QDateTime::currentMSecsSinceEpoch();
    if (bIsconnect)
    {
        return Result();
    }

    LoggingWrapper::instance()->debug("Integrating sphere connect start.");

    QString msg = "Integrating sphere connect failed.";
    if (LoadDll())
    {
        if (OpenAPI())
        {

            int m_PowerSupplyNum = 0;
            int m_LampNum = 0;
            //lsapi_device_DeviceClassT deviceClass;
            // Step 1: Use the Catalog to locate the device
            memset(&catParams, 0, sizeof(catParams));
            catParams.ApiCallHdr.hdr.StructSize = sizeof(catParams);
            catParams.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_CatalogDeviceClass;
            catParams.Options = lsapi_device_CatalogOption_Connectable;
            catParams.ClassToFind = lsapi_device_DeviceClass_Generic;
            lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&catParams);
            if (LSAPISUCCESS(rslt))
            {
                
                for (int i = 0; i < catParams.ConnectableDeviceCount; i++) {
                    lsapi_ApiCall_Device_ConnectDevice_Params connParams;
                    memset(&connParams, 0, sizeof(connParams));
                    connParams.ApiCallHdr.hdr.StructSize = sizeof(lsapi_ApiCall_Device_ConnectDevice_Params);
                    connParams.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ConnectDevice;

                    connParams.DvcInfo = catParams.ConnectableDevices[i];

                    lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&connParams);

                    switch (catParams.ConnectableDevices[i].DeviceClass)
                    {
                    case lsapi_device_DeviceClass_Lamp:// Lamp ????

                        if (LSAPISUCCESS(rslt))
                        {
                            l_hDevice[m_LampNum] = connParams.hDevice;
                            m_LampNum++;
                        }
                        break;
                    case lsapi_device_DeviceClass_PowerSupply:// ¦Ì??¡ä????
                    case lsapi_device_DeviceClass_Detector:
    
                        if (LSAPISUCCESS(rslt))
                        {
                            g_hDevice[m_PowerSupplyNum] = connParams.hDevice;
                            m_PowerSupplyNum++;
                        }
                        break;

                    }                                  
                }
   
                if (m_PowerSupplyNum == 4 && m_LampNum == 3)
                {
                    bIsconnect = true;

                    Result res = setDeviceEnable(true);
                    if (!res.success)
                    {
                        return res;
                    }

                    setShutterTurn( "R", false, false);
                    setShutterTurn("G", false, false);
                    setShutterTurn("B", false, false);
                    emit connectStatus(true, "");
                    emit refreshData(0, 0, 0);

                    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
                    LoggingWrapper::instance()->debug(QString("Integrating sphere connect time: %1 ms.").arg(takeTime));
                    return Result();
                }
                else
                {
                    msg = "Integrating sphere connect error, device(lsapi_device_DeviceClass_PowerSupply) count is less 4, or device(lsapi_device_DeviceClass_Lamp) count is less 3";
                }
            }
            else
            {
                msg = "Integrating sphere locate the device failed.";
            }
        }
        else
        {
            msg = "Integrating sphere open API failed.";
        }
    }
    else
    {
        msg = "Integrating sphere load Dll failed.";
    }
    bIsconnect = false;

    emit connectStatus(false, msg);
    return Result(false, msg.toStdString());
}
bool IntegratingSphereTCPModel::SetCurrentOutput(lsapi_DeviceHandleT hDevice, double fValue)
{
    if (bIsconnect)
{
    lsapi_ApiCall_Device_ControlDevice_PowerSupply_Item_Params psParams;
    memset(&psParams, 0, sizeof(psParams));
    psParams.CtlHdr.ApiCallHdr.hdr.StructSize = sizeof(psParams);
    psParams.CtlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice;
    psParams.CtlHdr.DeviceClass = lsapi_device_DeviceClass_PowerSupply;
    psParams.CtlHdr.SubCmd = lsapi_device_Control_PowerSupply_Item;

    psParams.CtlHdr.hDevice = hDevice;       // Handle from ::lsapi_ApiCall_Device_ConnectDevice

    psParams.Item = lsapi_device_powersupply_Item_Current_Output;
    psParams.Set = true;            // Instruct the API to process 'Value'
    psParams.Value = fValue;
    lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&psParams);
    if (rslt == 0)
        return true;
    else
        return false;
}
    else
        return false;
}
bool IntegratingSphereTCPModel::SetVoltageOutput(lsapi_DeviceHandleT hDevice, double fValue)
{
    if (bIsconnect)
{
    lsapi_ApiCall_Device_ControlDevice_PowerSupply_Item_Params psParams; 
    memset(&psParams, 0, sizeof(psParams));
    psParams.CtlHdr.ApiCallHdr.hdr.StructSize = sizeof(psParams);
    psParams.CtlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice; 
    psParams.CtlHdr.DeviceClass = lsapi_device_DeviceClass_PowerSupply; 
    psParams.CtlHdr.SubCmd = lsapi_device_Control_PowerSupply_Item;
    psParams.CtlHdr.hDevice = hDevice;       // Handle from ::lsapi_ApiCall_Device_ConnectDevice
    psParams.Item = lsapi_device_powersupply_Item_Voltage_Output;

    psParams.Set = true;            // Instruct the API to process 'Value'
    psParams.Value = fValue;          // The desired output voltage
    lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&psParams);
    if(LSAPISUCCESS(rslt)){ 
        // Output voltage was successfully set
       
    }
    if (rslt == 0)
        return true;
        else
            return false;
    }
    else
        return false;
}
double IntegratingSphereTCPModel::GetCurrentOutput(lsapi_DeviceHandleT hDevice)
{
    double measuredCurrent = 0.0;
    if (bIsconnect)
    {
    if (hDevice != NULL)
    {
        lsapi_ApiCall_Device_ControlDevice_PowerSupply_Item_Params psParams;
        memset(&psParams, 0, sizeof(psParams));
        psParams.CtlHdr.ApiCallHdr.hdr.StructSize = sizeof(psParams);
        psParams.CtlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice;
        psParams.CtlHdr.DeviceClass = lsapi_device_DeviceClass_PowerSupply;
        psParams.CtlHdr.SubCmd = lsapi_device_Control_PowerSupply_Item;
        psParams.CtlHdr.hDevice = hDevice;       // Handle from ::lsapi_ApiCall_Device_ConnectDevice
        psParams.Item = lsapi_device_powersupply_Item_Current_Measured;

            psParams.Set = false;           // Instruct the API to populate 'Value'
            lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&psParams);
            if (LSAPISUCCESS(rslt)) 
            { 
                measuredCurrent = psParams.Value; 
            }
        }
    }
    return measuredCurrent;
}
double IntegratingSphereTCPModel::GetVoltageOutput(lsapi_DeviceHandleT hDevice)
{
    double measuredVoltage = 0.0;
    if (bIsconnect)
    {
    if (hDevice != NULL)
    {
        lsapi_ApiCall_Device_ControlDevice_PowerSupply_Item_Params psParams;
        memset(&psParams, 0, sizeof(psParams));
        psParams.CtlHdr.ApiCallHdr.hdr.StructSize = sizeof(psParams);
        psParams.CtlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice;
        psParams.CtlHdr.DeviceClass = lsapi_device_DeviceClass_PowerSupply;
        psParams.CtlHdr.SubCmd = lsapi_device_Control_PowerSupply_Item;
        psParams.CtlHdr.hDevice = hDevice;       // Handle from ::lsapi_ApiCall_Device_ConnectDevice

        psParams.Item = lsapi_device_powersupply_Item_Voltage_Measured;

            psParams.Set = false;           // Instruct the API to populate 'Value'
            lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&psParams);
            if (LSAPISUCCESS(rslt))
            {
                measuredVoltage = (double)psParams.Value;
            }
        }
    }
    return measuredVoltage;
    
}
Result IntegratingSphereTCPModel::SetVoltageCompliance(lsapi_DeviceHandleT hDevice, double fValue)
{
    if (fValue < 1) {
        return Result();
    }

    if (!bIsconnect) {
        return Result(false, "Set labsphere voltage compliance error, it is not connectd.");
    }

    static QMap<lsapi_DeviceHandleT, bool> isSetMap;
    if (isSetMap.contains(hDevice) && isSetMap.value(hDevice)) {
        return Result();
    }

    lsapi_ApiCall_Device_ControlDevice_PowerSupply_Item_Params psParams;
    std::memset(&psParams, 0, sizeof(psParams));
    psParams.CtlHdr.ApiCallHdr.hdr.StructSize = sizeof(psParams);
    psParams.CtlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice;
    psParams.CtlHdr.DeviceClass = lsapi_device_DeviceClass_PowerSupply;
    psParams.CtlHdr.SubCmd = lsapi_device_Control_PowerSupply_Item;
    psParams.CtlHdr.hDevice = hDevice;       // Handle from ::lsapi_ApiCall_Device_ConnectDevice

    psParams.Item = lsapi_device_powersupply_Item_Voltage_Compliance;
    psParams.Set = true;            // Instruct the API to process 'Value'
    psParams.Value = fValue;   // The desired compliance voltage
    lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&psParams);
    if (LSAPISUCCESS(rslt)) {
        isSetMap[hDevice] = true;
        LoggingWrapper::instance()->debug(QString("SetVoltageCompliance: %1").arg(fValue));
        return Result();
    }
    else {
        return Result(false, "Set labsphere voltage compliance error.");
    }
}
double IntegratingSphereTCPModel::GetVoltageCompliance(lsapi_DeviceHandleT hDevice)
{
    return 0.0f;

    if (!bIsconnect) {
        return 0;
    }

    lsapi_ApiCall_Device_ControlDevice_PowerSupply_Item_Params psParams;
    std::memset(&psParams, 0, sizeof(psParams));
    psParams.CtlHdr.ApiCallHdr.hdr.StructSize = sizeof(psParams);
    psParams.CtlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice;
    psParams.CtlHdr.DeviceClass = lsapi_device_DeviceClass_PowerSupply;
    psParams.CtlHdr.SubCmd = lsapi_device_Control_PowerSupply_Item;
    psParams.CtlHdr.hDevice = hDevice;       // Handle from ::lsapi_ApiCall_Device_ConnectDevice

    psParams.Item = lsapi_device_powersupply_Item_Voltage_Compliance;
    psParams.Set = false;           // Instruct the API to populate 'Value'
    lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&psParams);
    double complianceVoltage = 0;
    if (LSAPISUCCESS(rslt)) {
        complianceVoltage = (double)psParams.Value;
        LoggingWrapper::instance()->debug(QString("GetVoltageCompliance success: %1").arg(complianceVoltage));
        return complianceVoltage;
    }
    else {
        LoggingWrapper::instance()->debug(QString("GetVoltageCompliance error: %1").arg(complianceVoltage));
        return 0;
    }
}
bool IntegratingSphereTCPModel::TurnOnLamp(lsapi_DeviceHandleT hDevice)
{
    if (bIsconnect)
{
    if (hDevice != NULL)
    {
        lsapi_ApiCall_Device_ControlDeviceHdr ctlHdr; memset(&ctlHdr, 0, sizeof(ctlHdr));
        ctlHdr.ApiCallHdr.hdr.StructSize = sizeof(ctlHdr);
        ctlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice;
        ctlHdr.DeviceClass = lsapi_device_DeviceClass_Lamp;
        ctlHdr.SubCmd = lsapi_device_Control_Lamp_TurnOn;
        ctlHdr.hDevice = hDevice;       // Handle from ::lsapi_ApiCall_Device_ConnectDevice
        lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&ctlHdr);
        if (LSAPISUCCESS(rslt))
            return true;
        else
            return false;
    }
    else
        return false;
    }
    else
        return false;
}
bool IntegratingSphereTCPModel::TurnOffLamp(lsapi_DeviceHandleT hDevice)
{
    if (bIsconnect)
{
    if (hDevice != NULL)
    {
        lsapi_ApiCall_Device_ControlDeviceHdr ctlHdr;
        memset(&ctlHdr, 0, sizeof(ctlHdr));
        ctlHdr.ApiCallHdr.hdr.StructSize = sizeof(ctlHdr);
        ctlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice;
        ctlHdr.DeviceClass = lsapi_device_DeviceClass_Lamp;
        ctlHdr.SubCmd = lsapi_device_Control_Lamp_TurnOff;
        ctlHdr.hDevice = hDevice;       // Handle from ::lsapi_ApiCall_Device_ConnectDevice
        lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&ctlHdr);
        if (LSAPISUCCESS(rslt))
            return true;
        else
            return false;
    }
    else
        return false;
    }
    else
        return false;
}
bool IntegratingSphereTCPModel::ControlOutputEnable(lsapi_DeviceHandleT hDevice, bool Enable)
{
    if (bIsconnect)
{
    lsapi_ApiCall_Device_ControlDevice_PowerSupply_Item_Params psParams; 
    memset(&psParams, 0, sizeof(psParams));
    psParams.CtlHdr.ApiCallHdr.hdr.StructSize = sizeof(psParams);
    psParams.CtlHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Device_ControlDevice;
    psParams.CtlHdr.DeviceClass = lsapi_device_DeviceClass_PowerSupply; 
    psParams.CtlHdr.SubCmd = lsapi_device_Control_PowerSupply_Item; 
    psParams.CtlHdr.hDevice = hDevice;       // Handle from ::lsapi_ApiCall_Device_ConnectDevice
    psParams.Item = lsapi_device_powersupply_Item_OutputEnabled;
    psParams.Set = true;            // Instruct the API to process 'Value'
    psParams.Value = Enable;          // Enable output
    lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&psParams);
    if(LSAPISUCCESS(rslt)){  
        // Output was successfully enabled
        return true;
        }
        else
            return false;
    }
    else
        return false;
}
void IntegratingSphereTCPModel::Disconnect()
{
    if (bIsconnect)
    {
        setShutterTurn("W",0);
        setAllCurrentOutputZero();
        if (g_pfnApiCall != NULL)
        {
            for (int i = 0; i < 3; i++)
            {
                lsapi_ApiCall_Api_Utility_ReleaseHandle_Params rlsParams;
                memset(&rlsParams, 0, sizeof(rlsParams));
                rlsParams.UtilsHdr.ApiCallHdr.hdr.StructSize = sizeof(lsapi_ApiCall_Api_Utility_ReleaseHandle_Params);
                rlsParams.UtilsHdr.ApiCallHdr.ApiCall = lsapi_ApiCall_Api_Utilities;
                rlsParams.UtilsHdr.UtilityType = lsapi_api_UtilityType_ReleaseHandle;
                rlsParams.hToRelease = g_hDevice[i];
                lsapi_ResultT rslt = g_pfnApiCall((lsapi_ApiCallHdr*)&rlsParams);
            }
            bIsconnect = false;
        }
    }

    // Close the API
    if (g_pfnApiCall != nullptr)
    {
        lsapi_ApiCallHdr closeHdr;
        memset(&closeHdr, 0, sizeof(closeHdr));
        closeHdr.hdr.StructSize = sizeof(closeHdr);
        closeHdr.ApiCall = lsapi_ApiCall_Api_Close;
        // lsapi_ResultT rslt = lsapi_ApiCall(&closeHdr);
        lsapi_ResultT rslt = g_pfnApiCall(&closeHdr);
        if (LSAPISUCCESS(rslt))
        {
        }
        g_pfnApiCall = nullptr;
    }

    if (hDll != nullptr)
    {
        FreeLibrary(hDll);
        hDll = nullptr;
    }

    bIsconnect = false;
    QString msg = "Integrating sphere disconnected.";
    emit connectStatus(false, msg);
}

bool IntegratingSphereTCPModel::isOpened()
{
    return bIsconnect;
}


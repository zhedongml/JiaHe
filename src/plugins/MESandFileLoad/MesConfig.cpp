#include "MesConfig.h"
#include <QDebug>

MesConfig& MesConfig::instance()
{
	static MesConfig self;
	return self;
}

MesConfig::MesConfig()
{
    m_json = load(FILE_NAME);
}

MesConfig::~MesConfig()
{
}

void MesConfig::getTcpInfo(QString& ip, int& port)
{
    ip = QString::fromStdString(m_json["DataUpload"]["TCP"]["Ip"].get<string>());
    port = m_json["DataUpload"]["TCP"]["Port"].get<int>();
}

bool MesConfig::getSWDebug()
{
    return m_json["DataUpload"]["SWDebug"].get<bool>();
}

QString MesConfig::getSingleLensLoadURL()
{
    string url;
    if(mesHttpTest()){
        url = m_json["MesHttp"]["TestMachine"]["SingleLensLoadURL"].get<string>();
    }else{
        url = m_json["MesHttp"]["ProductionMachine"]["SingleLensLoadURL"].get<string>();
    }
    return QString::fromStdString(url);
}

QString MesConfig::getSingleLensUnloadURL()
{
    string url;
    if (mesHttpTest()) {
        url = m_json["MesHttp"]["TestMachine"]["SingleLensUnloadURL"].get<string>();
    }
    else {
        url = m_json["MesHttp"]["ProductionMachine"]["SingleLensUnloadURL"].get<string>();
    }
    return QString::fromStdString(url);
}

QString MesConfig::getCuttingPreventURL()
{
    string url;
    if (mesHttpTest()) {
        url = m_json["MesHttp"]["TestMachine"]["CuttingPreventURL"].get<string>();
    }
    else {
        url = m_json["MesHttp"]["ProductionMachine"]["CuttingPreventURL"].get<string>();
    }
    return QString::fromStdString(url);
}

bool MesConfig::getMesOnline()
{
    return m_json["MesOnline"].get<bool>();
}

QString MesConfig::getMac()
{
    m_jsonParam = load(FILE_NAME_PARAM);
    QString  mac = QString::fromStdString(m_jsonParam["MAC"].get<std::string>());
    return mac;
}

QString MesConfig::getADcode()
{
    m_jsonParam = load(FILE_NAME_PARAM);
    QString param = QString::fromStdString(m_jsonParam["ADcode"].get<std::string>());
    return param;
}

QString MesConfig::getMesToken()
{
    m_jsonParam = load(FILE_NAME_PARAM);
    QString  param = QString::fromStdString(m_jsonParam["mes-token"].get<std::string>());
    return param;
}

bool MesConfig::writeTrayInfo(QString type, QString code)
{
    m_jsonParam = load(FILE_NAME_PARAM);

    if (type == "OK" || type == "TrayCodeOK") {
        m_jsonParam["TrayCodeOK"] = code.toStdString();
    }
    else if (type == "NG" || type == "TrayCodeNG") {
        m_jsonParam["TrayCodeNG"] = code.toStdString();
    }
    std::ofstream ofs(FILE_NAME_PARAM.toStdString());
    ofs << m_jsonParam.dump(4);
    return true;
}

bool MesConfig::writeTray(bool isOK, QString code)
{
    if(isOK){
        return writeTrayInfo("TrayCodeOK", code);
    }else{
        return writeTrayInfo("TrayCodeNG", code);
    }
}

QString MesConfig::getCbpcodeNG()
{
    m_jsonParam = load(FILE_NAME_PARAM);
    QString param = QString::fromStdString(m_jsonParam["cbpcodeNG"].get<std::string>());
    return param;
}

QString MesConfig::getTrayCodeOK()
{
    m_jsonParam = load(FILE_NAME_PARAM);
    QString param = QString::fromStdString(m_jsonParam["TrayCodeOK"].get<std::string>());
    return param;
}

QString MesConfig::getTrayCodeNG()
{
    m_jsonParam = load(FILE_NAME_PARAM);
    QString param = QString::fromStdString(m_jsonParam["TrayCodeNG"].get<std::string>());
    return param;
}

Json MesConfig::load(QString fileName)
{
    std::ifstream jsonFile(fileName.toStdString());
    if (jsonFile.is_open())
    {
        std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();
        return Json::parse(contents);
    }
    else
    {
        qWarning() << QString("Load file error, %1").arg(FILE_NAME);
    }
}

bool MesConfig::mesHttpTest()
{
    return m_json["MesHttp"]["IsTest"].get<bool>();
}


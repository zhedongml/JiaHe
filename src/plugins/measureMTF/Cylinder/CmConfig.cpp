#include "CmConfig.h"
#include <QMutex>
#include <QDebug>
#include <QFile>
#include <fstream>
#include "MLColorimeterMode.h"

CmConfig *CmConfig::m_instance = nullptr;

CmConfig *CmConfig::instance()
{
    if (!m_instance)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance)
        {
            m_instance = new CmConfig();
        }
    }
    return m_instance;
}

CmConfig::CmConfig()
{
    Result ret = loadFile(FILE_NAME);
    if(!ret.success){
        qCritical() << QString::fromStdString(ret.errorMsg);
    }
}

CmConfig::~CmConfig()
{
}

QList<QString> CmConfig::getCylinderNameList()
{
    QMap<QString, int> cylinderMap = getCylinderMap();

    QMap<int, QString> map;
    QMap<QString, int>::Iterator iter;
    for (iter = cylinderMap.begin(); iter != cylinderMap.end(); ++iter)
    {
        map[iter.value()] = iter.key();
    }

    QList<QString> list;
    QMap<int, QString>::Iterator iterT;
    for (iterT = map.begin(); iterT != map.end(); ++iterT)
    {
        list.append(iterT.value());
    }

    return list;
}

QMap<QString, int> CmConfig::getCylinderMap()
{
    Json jsonList = m_json["FilterMap"][0]["PositionList"];

    QMap<QString, int> map;
    for (size_t j = 0; j < jsonList.size(); j++)
    {
        map.insert(QString::fromStdString(jsonList[j]["Name"].get<std::string>()),
                           jsonList[j]["Channel"].get<int>());

    }
    return map;
}

Result CmConfig::getPulsePerCycle(int &steps)
{
    try{
        steps = m_json["FilterMap"][0]["Axis"]["PulsePerCycle"].get<int>();
        return Result();
    }
    catch(exception e){
        return Result(false, "Get Pulse Per Cycle error.");
    }
}

Result CmConfig::loadFile(const QString &fileName)
{
    std::ifstream jsonFile(fileName.toStdString());
    if (!jsonFile.is_open())
    {
        return Result(false, QString("Open config file failed, %1.").arg(fileName).toStdString());
    }

    std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
    jsonFile.close();

    m_json = Json::parse(contents);
    return Result();
}

Result CmConfig::getModuleConfigName(QString &fileName)
{
    QString sn/* = MLColorimeterMode::instance()->getMcSn()*/;
    if (sn.isEmpty()){
        return Result(false, "Get module config name error, sn is null.");
    }

    fileName = QString("./config/mlcolorimeter/ModuleData/%1/config/ModuleConfig.json").arg(sn);
    return Result();
}


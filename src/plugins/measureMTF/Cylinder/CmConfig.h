#pragma once
#include <QString>
#include <QMap>
#include <json.hpp>
#include "Result.h"

using Json = nlohmann::json;

class CmConfig
{
  public:
    static CmConfig *instance();
    ~CmConfig();

    QList<QString> getCylinderNameList();

    //name, channel
    QMap<QString, int> getCylinderMap();

    Result getPulsePerCycle(int &steps);

  private:
    explicit CmConfig();
    Result loadFile(const QString &fileName);
    Result getModuleConfigName(QString &fileName);

  private:
    static CmConfig *m_instance;
    Json m_json;
    const QString FILE_NAME = "./config/mlcolorimeter/EYE1/RXFilterConfig.json";

    Json m_jsonModule;
};

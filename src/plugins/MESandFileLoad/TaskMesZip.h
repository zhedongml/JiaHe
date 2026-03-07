#pragma once
#include "PrjCommon/taskManage/TaskBase.h"
#include <QDebug>
#include "MesCommon.h"

using namespace PrjCommon;

class TaskMesZip : public QObject, public TaskBase
{
    Q_OBJECT
public:
    TaskMesZip(const ExternalTOSPathbody& data, const std::vector<std::string>& dirPaths, const std::vector<std::string>& zipPaths);

    Result execute() override;
    QString taskInfo() override;

    void setStop(std::shared_ptr<std::atomic<bool>> stopFlag) override;

    void onSuccess() override;
    void onFailure() override;

private:
    ExternalTOSPathbody m_data;
    std::vector<std::string> m_dirPaths;
    std::vector<std::string> m_zipPaths;
};


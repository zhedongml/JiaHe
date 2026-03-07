#include "TaskMesZip.h"
#include "MesMode.h"

TaskMesZip::TaskMesZip(const ExternalTOSPathbody& data, const std::vector<std::string>& dirPaths, const std::vector<std::string>& zipPaths):
    m_data(data),
    m_dirPaths(dirPaths),
    m_zipPaths(zipPaths)
{

}

Result TaskMesZip::execute()
{
    Result ret = MesMode::instance().zipUpload(m_data, m_dirPaths, m_zipPaths);
    return ret;
}

QString TaskMesZip::taskInfo()
{
    string pathStr;
    for(string path: m_zipPaths){
        pathStr = pathStr.empty() ? path : pathStr + "--" + path;
    }
    return QString("MES compress and upload file %1 ").arg(QString::fromStdString(pathStr));
}

void TaskMesZip::setStop(std::shared_ptr<std::atomic<bool>> stopFlag)
{

}

void TaskMesZip::onSuccess()
{
}

void TaskMesZip::onFailure()
{
}

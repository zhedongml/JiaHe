#include "MesTaskAsync.h"
#include "TaskMesZip.h"
#include "TaskQdp.h"
#include "MesConfig.h"
#include "MesMode.h"

MesTaskAsync& MesTaskAsync::instance(QObject* parent)
{
    static MesTaskAsync self(parent);
    return self;
}

MesTaskAsync::MesTaskAsync(QObject* parent): QObject(parent)
{
    if(m_managerQdp == nullptr){
        m_managerQdp = new TaskManager(1);
    }
}

MesTaskAsync::~MesTaskAsync()
{
}

Result MesTaskAsync::runQdp(const std::string& sourcePath, const std::string& resultPath)
{
    QdpCsvHeader csvHeader;
    Result ret = QDFData().getQDFData(csvHeader);
    csvHeader.mes_status = MesConfig::instance().getMesOnline() ? 1 : 0;
    csvHeader.test_count = MesMode::instance().getTestCount();

    SummaryCsvHeader sumCsvHeader;
    ret = QDFData().getSummaryData(sumCsvHeader);
    sumCsvHeader.mes_status = MesConfig::instance().getMesOnline() ? 1 : 0;
    if (!ret.success) {
        qDebug() << "Task Async" << QString::fromStdString(ret.errorMsg);
        return ret;
    }

    auto task = QSharedPointer<TaskQdp>::create(csvHeader, sumCsvHeader, sourcePath, resultPath);
    ret = m_managerQdp->submit(task);
    if (!ret.success) {
        MetricsData::instance()->updateDutAutoDPResult("Metrics_ERR");
        qDebug() << "Task Async" << QString::fromStdString(ret.errorMsg);
    }
    return ret;
}

Result MesTaskAsync::uploadZip(const ExternalTOSPathbody& data, const std::vector<std::string>& dirPaths, const std::vector<std::string>& zipPaths)
{
    return Result();
    //auto task = QSharedPointer<TaskMesZip>::create(data, dirPaths, zipPaths);
    //Result ret = m_managerZip.submit(task);
    //if (!ret.success) {
    //    qDebug() << "Task Async" << QString::fromStdString(ret.errorMsg);
    //}
    //return ret;
}

Result MesTaskAsync::stopTask()
{
    //m_managerZip.stopTask();
    m_managerQdp->stopTask();
    return Result();
}

void MesTaskAsync::stopThread()
{
    //m_managerZip.stopThread();
    m_managerQdp->stopThread();
}

Result MesTaskAsync::waitAllTask()
{
    //Result ret = m_managerZip.waitEnd("Upload and compress zip file tasks.");
    Result ret = m_managerQdp->waitEnd("QDP task.");
    return ret;
}

Result MesTaskAsync::judgeEndMetricsTask()
{
    bool isEnd;
    Result ret = m_managerQdp->judgeEnd(isEnd, "QDP task");
    return ret;
}

void MesTaskAsync::notifyResult(Result ret)
{

}

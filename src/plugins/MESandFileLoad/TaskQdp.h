#pragma once
#include "PrjCommon/taskManage/TaskBase.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include "AutoDP/QDFData.h"
#include "AutoDP/ImageProcessor.h"

using namespace PrjCommon;

class TaskQdp : public QObject, public TaskBase
{
    Q_OBJECT
public:
    TaskQdp(const QdpCsvHeader& data, const SummaryCsvHeader& summaryData, const std::string& sourcePath, const std::string& rsultPath);

    Result execute() override;
    QString taskInfo() override;

    void setStop(std::shared_ptr<std::atomic<bool>> stopFlag) override;

    void onSuccess() override;
    void onFailure() override;

    Result zipDir(ProcessingTask proTask, QString& autoDPResult, QString& resultDir, SimpleBandizipCompressor::ZipVolumeInfo& zipVolumeInfo);
    Result updateCsv(const QString& finalResultDir, QdpCsvHeader qdpCsvHeader, QString& newFilePath, QList<QStringList>& initData);
    Result updateSummaryCsv(const QString & filePath, SummaryCsvHeader summaryCsvHeader, QString& newFilePath, QList<QStringList>& initData);
private:
    bool copyDirectory(const QString& sourcePath, const QString& destinationPath);
    std::string moveCSVFilesFromPyAutoDPFolders(const std::string& finalResultDir);
private:
    QdpCsvHeader m_qdpCsvHeader;
    SummaryCsvHeader m_summaryCsvHeader;
    std::string m_sourcePath;
    std::string m_resultPath;
};


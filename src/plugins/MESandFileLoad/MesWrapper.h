#pragma once
#include "PrjCommon/irecipewrapper.h"
#include "BandizipCompressor.h"
#include "ImageProcessor.h"

using namespace CORE;

class MesWrapper : public IRecipeWrapper
{
    Q_OBJECT

public:
    MesWrapper(QObject* parent = nullptr);
    ~MesWrapper();

    void Invoke(QString cmd, QString param) override;
    void notifyStop(bool isstop) override;
    void notifyPause(bool isPause) override;

private:
    Result dataUpload(QString param);
    Result baseInfoDialog(QString param);
    Result singleLensLoad(QString param);
    Result singleLensUnload(QString param);
    Result setCuttingPrevent(QString param);
    Result mesFileCompressor(QString param);
    Result fileCompressor(const std::string& inputDir, const std::string& outputArchive,
    const SimpleBandizipCompressor::CompressionConfig& config);

    Result setAutoDPConfig(QString param);
    Result autoDPResultPath(QString param);
    Result submitAutoDPTask(QString param);
    Result metricsLimitAutoDP(QString param);

private slots:
    void onTaskCompleted(const QString& fileName, const QString& fileBasePath, const QString& csvName);

private:
    ImageProcessor* m_processorWrapper;

    std::string m_autoDPSourceImageDir;
    std::string m_autoDPResultSaveDir;
};


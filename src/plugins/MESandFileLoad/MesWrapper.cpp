#include "MesWrapper.h"
#include "Core/loggingwrapper.h"
#include <QDateTime>
#include <QDebug>
#include "MesMode.h"
#include "QtWidgetsClass.h"
#include "PrjCommon/MetricsCheck.h"
#include "QDFData.h"
#include "MesTaskAsync.h"

MesWrapper::MesWrapper(QObject* parent) : IRecipeWrapper(parent)
{
    MesDialog::instance();
    m_processorWrapper = new ImageProcessor();
    connect(m_processorWrapper, &ImageProcessor::taskCompleted,
        this, &MesWrapper::onTaskCompleted);
    m_processorWrapper->startService();

}

MesWrapper::~MesWrapper()
{
    m_processorWrapper->stopService();
}

void MesWrapper::Invoke(QString cmd, QString param)
{
    int startTime = 0;
    int takeTime = 0;
    qWarning() << "---- takt time ---- [MES " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
        << "] start.";
    startTime = QDateTime::currentMSecsSinceEpoch();

    Result result;
    if (cmd == "DataUpload")
    {
        result = dataUpload(param);
    }
    else if(cmd == "BaseInfoDialog")
    {
        result = baseInfoDialog(param);
    }
    else if (cmd == "SingleLensLoad")
    {
        result = singleLensLoad(param);
    }
    else if (cmd == "SingleLensUnload")
    {
        result = singleLensUnload(param);
    }
    else if (cmd == "setCuttingPrevent")
    {
        result = setCuttingPrevent(param);
    }
    else if (cmd.trimmed() == "FileCompressor")
    {
        result = mesFileCompressor(param);
    }
    else if (cmd.trimmed() == "AutoDPConfig")
    {
        result = setAutoDPConfig(param);
    }
    else if (cmd.trimmed() == "AutoDPResultPath")
    {
        result = autoDPResultPath(param);
    }
    else if (cmd.trimmed() == "AutoDPTask")
    {
        result = submitAutoDPTask(param);
    }
    else if (cmd.trimmed() == "MetricsLimitAutoDP")
    {
        result = metricsLimitAutoDP(param);
    }
    else
    {
        result = Result(false, QString("\"%1\" cmd is mismatch, please check the execution fields.").arg(cmd).toStdString());
    }

    takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "---- takt time ---- [MES " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
        << "] end. times:" << takeTime << " ms...";

    if (!result.success)
    {
        QString message =
            QString("Recipe [%1:%2] run error, %3").arg(cmd).arg(param).arg(QString::fromStdString(result.errorMsg));
        LoggingWrapper::instance()->error(message);

        if (result.errorCode == CORE::Wrapper_Warning)
        {
            emit SendStatusSignal(CORE::Wrapper_Warning, QString::fromStdString(result.errorMsg));
        }
        else
        {
            emit SendStatusSignal(CORE::Wrapper_Error_fatal, QString::fromStdString(result.errorMsg));
        }
    }
    else
    {
        emit SendStatusSignal(Wrapper_Done, "Calibration is done!");
    }
}

void MesWrapper::notifyStop(bool isstop)
{
}

void MesWrapper::notifyPause(bool isPause)
{
}

Result MesWrapper::dataUpload(QString param)
{
    bool isZipData = false;
    bool isFileData = false;
    
    QStringList list = param.split("/");
    for(QString paramstr: list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "zipData")
            {
                isZipData = val.toInt() > 0;
            }
            else if (key == "fileData")
            {
                isFileData = val.toInt() > 0;
            }
        }
    }

    Result ret;
    if(isFileData){
        ret = MesMode::instance().fileDataUpload();
        if(!ret.success){
            return ret;
        }
    }

    if(isZipData){
        ret = MesMode::instance().zipDataUpload();
    }
    return ret;
}

Result MesWrapper::baseInfoDialog(QString param)
{
    Result ret = MesDialog::instance().showDialog();
    if (!ret.success) {
        return ret;
    }
    return Result();
}

Result MesWrapper::singleLensLoad(QString param)
{
    Result ret = MesMode::instance().singleLensLoad();
    return ret;
}

Result MesWrapper::singleLensUnload(QString param)
{
    bool existParam = false;
    bool success = true;
    QStringList list = param.split("/");
    for (QString paramstr : list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "Success")
            {
                success = val.toInt() > 0;
                existParam = true;
            }
        }
    }

    Result ret;
    if(existParam){
        ret = MesMode::instance().singleLensUnload(success);
    }else{
        ret = MesMode::instance().singleLensUnload();
    }
    return ret;
}

Result MesWrapper::setCuttingPrevent(QString param)
{
    QString trayCode;
    QStringList list = param.split("/");
    for (QString paramstr : list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "TrayCode")
            {
                trayCode = val;
            }
        }
    }

    Result ret = MesMode::instance().setCuttingPrevent(trayCode);
    return ret;
}

Result MesWrapper::mesFileCompressor(QString param)
{
    SimpleBandizipCompressor::CompressionConfig config;
    config.volumeSizeMB = 1024;
    config.archiveFormat = "zip";
    config.compressionLevel = 5;
    config.multiThread = true;

    std::string inputPath, outputArchive;

    QStringList list = param.split("/");
    for (QString paramstr : list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "InputPath")
            {
                inputPath = val.toStdString();
            }
            else if (key == "OutputArchive")
            {
                outputArchive = val.toStdString();
            }
            else if (key == "VolumeSize")
            {
                config.volumeSizeMB = val.toInt();
            }
            else if (key == "CompressionLevel")
            {
                config.compressionLevel = val.toInt();
            }
        }
    }

    return fileCompressor(inputPath, outputArchive, config);

}

Result MesWrapper::fileCompressor(const std::string& inputDir, const std::string& outputArchive, const SimpleBandizipCompressor::CompressionConfig& config)
{
    Result rt;
    SimpleBandizipCompressor::CompressionResult result;

    if (fs::is_directory(inputDir)) {
        result = SimpleBandizipCompressor::instance()->CompressDirectory(inputDir, outputArchive, config);
    }
    else {
        result = SimpleBandizipCompressor::instance()->CompressFile(outputArchive, outputArchive, config);
    }
    rt.success = result.success;
    rt.errorMsg = result.errorMessage;
    return rt;
}

Result MesWrapper::setAutoDPConfig(QString param)
{
    Result rt;
    std::string pyAutoDPPath;
    std::string testType;
    std::string projectName;
    std::string configDir;
    std::string tempDir;

    QStringList list = param.split("/");
    for (QString paramstr : list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "TestType")
            {
                testType = val.toStdString();
            }
            else if (key == "ProjectName")
            {
                projectName = val.toStdString();
            }
            else if (key == "PyAutoDPPath")
            {
                pyAutoDPPath = val.toStdString();
            }
            else if (key == "ConfigDir")
            {
                configDir = val.toStdString();
            }
            else if (key == "TempDir")
            {
                tempDir = val.toStdString();
            }
        }
    }
    m_processorWrapper->stopService();
    auto result = m_processorWrapper->setConfig(pyAutoDPPath, testType, projectName, configDir, tempDir, "");
    if (result.success)
    {
        result = m_processorWrapper->startService();
    }
    rt.success = result.success;
    rt.errorMsg = result.errorMessage;
    return rt;
}

Result MesWrapper::autoDPResultPath(QString param)
{
    Result rt;
    m_autoDPSourceImageDir = MetricsData::instance()->getIQRecipeSeqDir().toStdString();

    QStringList list = param.split("/");
    for (QString paramstr : list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "ResultSaveDir")
            {
                m_autoDPResultSaveDir = val.toStdString();
            }
        }
    }

    return Result();
}

Result MesWrapper::submitAutoDPTask(QString param)
{
    ////TODO: test
    //ExternalTOSPathbody data;
    //Result ret = MesMode::instance().sendTcp(data);
    //if (!ret.success) {
    //    return Result(false, "Upload data ZIP error, " + ret.errorMsg);
    //}
    //return Result();

    std::string sourceImageDir = m_autoDPSourceImageDir;
    std::string resultSaveDir = m_autoDPResultSaveDir;

    QStringList list = param.split("/");
    for (QString paramstr : list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "SourceImageDir")
            {
                sourceImageDir = val.toStdString();
            }
            else if (key == "ResultSaveDir")
            {
                resultSaveDir = val.toStdString();
            }

        }
    }

    //auto result = m_processorWrapper->submitProcessingTask(sourceImageDir, resultSaveDir);
    //rt.errorMsg = result.errorMessage;
    //rt.success = result.success;

    Result ret = MesTaskAsync::instance().runQdp(sourceImageDir, resultSaveDir);
    return ret;
}

Result MesWrapper::metricsLimitAutoDP(QString param)
{
    bool checkEnabled = true;
    QString modelName;
    bool liveRefresh = false;
    QStringList list = param.split("/");
    for (QString paramstr : list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "CheckEnabled")
            {
                checkEnabled = val.toInt() > 0;
            }
            else if (key == "ModelName") {
                modelName = val;
            }
            else if (key == "LiveRefresh") {
                liveRefresh = val.toInt() > 0;
            }
        }
    }

    MetricsCheck::instance().setCheckEnabled_autoDP(checkEnabled);
    Result ret = MetricsCheck::instance().setModelName_autoDP(modelName, liveRefresh);
    if (!ret.success)
    {
        return ret;
    }

    //TODO: test
    //QString filePath = "D:\pcdata\Eyebox12345_AutoDP\NWN7Z-L-A2C2_20251120T223514_MetricsTest\NWN7Z-L-A2C2_20251120T223514_MetricsTest_20251120T230115.csv";
    //QString filePath;
    //ret = QDFData().getAutoDpCsv(filePath, "D:\\pcdata\\Eyebox12345_AutoDP\\NWN7Z-L-A2C2_20251120T223514_MetricsTest");
    //if (!ret.success)
    //{
    //    return ret;
    //}
    //ret =  QDFData().updateCsv(filePath);
    return Result();
}

void MesWrapper::onTaskCompleted(const QString& fileName, const QString& fileBasePath, const QString& csvName)
{
    //TODO
    //Called MES Func
}


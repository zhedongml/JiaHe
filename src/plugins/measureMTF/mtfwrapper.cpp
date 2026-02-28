#include "mtfwrapper.h"
#include "GenCSVHeader.h"
#include "PrjCommon/metricsdata.h"
#include "Core/loggingwrapper.h"
#include <QDebug>
#include <QThread>
#include <QUuid>
#include <QDir>

MTFWrapper::MTFWrapper(QObject *parent) : IRecipeWrapper(parent)
{
    NineSquareMTFModel *mtfModel = NineSquareMTFModel::getInstance(this);
    setModel(mtfModel);
}
MTFWrapper::~MTFWrapper()
{
}
void MTFWrapper::setModel(NineSquareMTFModel *model)
{
    m_Model = model;
}

void MTFWrapper::Invoke(QString cmd, QString param)
{
    int startTime = 0;
    int takeTime = 0;
    qWarning() << "---- takt time ---- [MTF " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
               << "] start.";
    startTime = QDateTime::currentMSecsSinceEpoch();

    try
    {
        if (cmd == "New")
        {
            createNewResultFile(param);
        }
        else if (cmd == "OutputDir")
        {
            createOutputDir(param);
        }
        else if (cmd.trimmed() == "NewMTFHistoryTable")
        {
            createMTFTable(param);
        }
        else if (cmd.trimmed() == "Measure")
        {
            LoggingWrapper::instance()->info(QString("MTF calculate start."));
            measure(param);
            LoggingWrapper::instance()->info(QString("MTF calculate end."));
        }
        else if (cmd.trimmed() == "ThruFocus")
        {
            try
            {
                LoggingWrapper::instance()->warn(QString("Auto focus start."));

                // float expectFocus = getExpectFocus(param);
                float expectFocus = 0; // TODO:test
                m_Model->thruFocus(expectFocus);

                LoggingWrapper::instance()->warn(QString("Auto focus end."));

                m_Status = CORE::Wrapper_Done;
                emit SendStatusSignal(m_Status, "thru-foucs done");
            }
            catch (...)
            {
                m_Status = CORE::Wrapper_Error_fatal;
                emit SendStatusSignal(m_Status, "thru-foucs error");
            }
        }
        else if (cmd.trimmed() == "SetROI")
        {
            setROI(param);
        }
        else
        {
            m_Status = CORE::Wrapper_Error_fatal;
            emit SendStatusSignal(m_Status, QString("\"%1\" cmd is mismatch, please check the execution fields.").arg(cmd));
        }
    }
    catch (...)
    {
        // TODO: test
        emit SendStatusSignal(CORE::Wrapper_Warning, "Run exception.");

        //m_Status = CORE::Wrapper_Error_fatal;
        //emit SendStatusSignal(m_Status, "Run exception.");
    }

    takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "---- takt time ---- [MTF " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
               << "] end. times:" << takeTime << " ms...";
}

void MTFWrapper::notifyStop(bool isstop)
{
    m_Model->recipeStop(isstop);
}

bool MTFWrapper::isDirExist(QString fullPath)
{
    QDir dir(fullPath);
    if (dir.exists())
    {
        return true;
    }
    else
    {
        bool ok = dir.mkpath(fullPath);
        return ok;
    }
}

void MTFWrapper::measure(QString param)
{
    bool isAutoMode = false;
    QStringList list = param.split("/");
    m_Model->setIsOffLineImg(false);
    QList<int> roiSelect;
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "image")
            {
                m_Model->setIsOffLineImg(true);
                m_Model->setOfflineImgPath(val);
            }
            else if (key == "Mode")
            {
                if (val == "Auto")
                {
                    isAutoMode = true;
                }
            }
            else if (key == "TemplateImage")
            {
                cv::Mat mat = cv::imread(val.toStdString(), cv::IMREAD_UNCHANGED);
                visionAlgorithm::GetInstance()->DestinationImage = mat;
            }
            else if (key == "Score")
            {
                visionAlgorithm::GetInstance()->MatchScore = val.toDouble();
            }
            else if (key == "Angle")
            {
                visionAlgorithm::GetInstance()->MatchAngle = val.toInt();
            }
            else if (key == "Target")
            {
                visionAlgorithm::GetInstance()->MatchTarget = val.toInt();
            }
            else if (key == "Color")
            {
                m_Model->setLedColor(val.toLower());
            }
            else if (key == "eyebox")
            {
                m_Model->setEyeboxNum(val);
            }
            else if (key == "ROI_select")
            {
                QStringList strList = val.split(",");
                for (QString str : strList)
                {
                    roiSelect.append(str.toInt());
                }
            }
        }
    }
    uint st = QDateTime::currentDateTime().toMSecsSinceEpoch();
    // m_Model->calculateMTF(isAutoMode, roiSelect);
    m_Model->calculateMTFByAutoIdentifyNineCross(isAutoMode, roiSelect);
    uint deltaT = QDateTime::currentDateTime().toMSecsSinceEpoch() - st;
    qWarning() << "---- takt time ---- [MTF]"
               << "calculate MTF done time : " << deltaT << " ms... ";
    // for test
    QThread::sleep(2);
    //LoggingWrapper::instance()->info("mtf calculator done.");
    m_Status = CORE::Wrapper_Done;
    emit SendStatusSignal(m_Status, "mtf calculator done");
}

float MTFWrapper::getExpectFocus(QString param)
{
    return -1;
}

void MTFWrapper::createMTFTable(QString param)
{
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "DutId")
            {
                MetricsData::instance()->setDutSN(val);
            }
        }
    }

    QString dutId = MetricsData::instance()->getDutSN();
    bool ok = GenCSVHeader::createMTFTable(dutId);
    if (ok)
    {
        m_Status = CORE::Wrapper_Done;
        emit SendStatusSignal(m_Status, "create MTF table done");
    }
    else
    {
        m_Status = CORE::Wrapper_Error_fatal;
        emit SendStatusSignal(m_Status, "create MTF table errors");
    }
}

void MTFWrapper::setROI(QString param)
{
    QStringList list = param.split("/");
    ROIPosConfig posConfig;
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "CrossAreas")
            {
                std::vector<CrossAreaConfig> crossAreas;
                QStringList valueList = val.split(";");
                for (QString str : valueList)
                {
                    QStringList strList = str.split(",");
                    CrossAreaConfig cross;
                    cross.FromX = strList[0].toInt();
                    cross.FromY = strList[1].toInt();
                    cross.aoiWidth = strList[2].toInt();
                    cross.aoiHeight = strList[3].toInt();
                    cross.id = 0;

                    crossAreas.push_back(cross);
                }
                MTFModelConfig::instance()->updateCrossAreaConfig(crossAreas);
            }
            else if (key == "ROI_centeroffset")
            {
                posConfig.centerOffset = val.toInt();
            }
            if (key == "ROI_height")
            {
                posConfig.roiHeight = val.toInt();
            }
            if (key == "ROI_width")
            {
                posConfig.roiWidth = val.toInt();
            }
        }
    }

    MTFModelConfig::instance()->setROIPosCfg(posConfig);
    m_Status = CORE::Wrapper_Done;
    LoggingWrapper::instance()->info("mtf set roi done.");
    emit SendStatusSignal(m_Status, "mtf set roi done");
}

void MTFWrapper::createNewResultFile(QString param)
{
    MetricsData::instance()->setDutBarCode("");
    QString csvName = "d:/hydusdata/result";
    QUuid id = QUuid::createUuid();
    QString strGuid = id.toString(QUuid::Id128);
    QStringList list = param.split("/");
    if (list.size() >= 2)
    {
        for each (QString paramstr in list)
        {
            QStringList paramPair = paramstr.split("=");
            if (paramPair.size() == 2)
            {
                QString key = paramPair[0].trimmed();
                QString val = paramPair[1].trimmed();
                if (key == "RootDir")
                {
                    csvName = val;
                }
                else if (key == "DutId")
                {
                    MetricsData::instance()->setDutBarCode(val);
                    m_Model->setDUTBarCode(val);
                }
            }
        }

        if (MetricsData::instance()->getDutBarCode().isEmpty())
        {
            QDateTime current_date_time = QDateTime::currentDateTime();
            QString current_date_str = current_date_time.toString("yyyyMMdd_hhmmss");
            MetricsData::instance()->setDutBarCode(current_date_str);
          //  m_Model->setDUTBarCode(current_date_str);
        }
        int last = csvName.lastIndexOf("\\");
        // QString newSeqPath = rootdir + "/" + currentDate;
        QString newSeqPath = csvName.left(last + 1);
     //   m_Model->matchLogAll = newSeqPath;
        QString csvFileName = csvName.mid(last + 1, csvName.length());
        MetricsData::instance()->setMTFRecipeName(csvFileName + ".csv");
        bool res = isDirExist(newSeqPath);
        MetricsData::instance()->setRecipeSeqDir(newSeqPath);
        if (res)
        {

            QString newcsvNameWithsuffix = csvName + ".csv";
            QFile file(newcsvNameWithsuffix);
            if (!file.exists())
            {
                bool ok = m_Model->createResutCSVFile(newcsvNameWithsuffix);
                if (ok)
                {
                    MetricsData::instance()->setSequenceId(strGuid);
                    m_Status = CORE::Wrapper_Done;
                    LoggingWrapper::instance()->info("new csv file done.");
                    emit SendStatusSignal(m_Status, "new  csv file done");
                }
                else
                {
                    m_Status = CORE::Wrapper_Error_fatal;
                    LoggingWrapper::instance()->error("create csv file error.");
                    emit SendStatusSignal(m_Status, "create csv file error");
                }
            }
            else
            {
                m_Model->createResutCSVFile(newcsvNameWithsuffix);
                m_Status = CORE::Wrapper_Done;
                //LoggingWrapper::instance()->error("csv file already exist.");
                emit SendStatusSignal(m_Status, "csv file already exist");
            }
        }
        else
        {
            m_Status = CORE::Wrapper_Error_fatal;
            LoggingWrapper::instance()->error("new sequence dir error.");
            emit SendStatusSignal(m_Status, "new sequence dir error");
        }
    }
}

Result MTFWrapper::createOutputDir(QString param)
{
    QStringList list = param.split("/");
    if (list.size() >= 2)
    {
        for each (QString paramstr in list)
        {
            QStringList paramPair = paramstr.split("=");
            if (paramPair.size() == 2)
            {
                QString key = paramPair[0].trimmed();
                QString val = paramPair[1].trimmed();
                if (key == "DirName")
                {
                    m_Model->setDUTBarCode(val);
                }
            }
        }
    }

    QString csvDir = MetricsData::instance()->getOutputImageDir() + "\\MTF_Test";
    MetricsData::instance()->setRecipeSeqDir(csvDir);
    bool ret = isDirExist(csvDir);
    if (!ret)
    {
        m_Status = CORE::Wrapper_Error_fatal;
        LoggingWrapper::instance()->error("Mtf dir error.");
        emit SendStatusSignal(m_Status, "Mtf dir error");
        return Result(false, "Mtf dir error");
    }

    QString csvFileName = "MTFresult.csv";
    MetricsData::instance()->setMTFRecipeName(csvFileName);

    QString newcsvNameWithsuffix = csvDir + "\\" + csvFileName;
    QFile file(newcsvNameWithsuffix);
    if (file.exists()){
        m_Status = CORE::Wrapper_Done;
        //LoggingWrapper::instance()->info("MTF csv file already exist.");
        emit SendStatusSignal(m_Status, "MTF csv file already exist");
        return Result();
    }

    bool ok = m_Model->createResutCSVFile(newcsvNameWithsuffix);
    if (!ok)
    {
        m_Status = CORE::Wrapper_Error_fatal;
        LoggingWrapper::instance()->error("MTF create csv file error.");
        emit SendStatusSignal(m_Status, "MTF create csv file error");
        return Result(false, "MTF create csv file error");
    }

    m_Status = CORE::Wrapper_Done;
    LoggingWrapper::instance()->info("MTF new csv file done.");
    emit SendStatusSignal(m_Status, "MTF new  csv file done");
    return Result();
}

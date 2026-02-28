#include "iqtwrapper.h"
#include <QUuid>
#include "GenCSVHeader.h"
#include "PrjCommon/metricsdata.h"
#include "iqtmodel.h"
#include "Core/loggingwrapper.h"
#include <QDateTime>
#include <json.hpp>
#include <log4cplus/fileappender.h>
#include "PrjCommon/Session.h"
#include "MeasureWidget.h"
#include "LumStatistics.h"
//#include "Focus/AutoVidModel.h"
#include "TaskAsync.h"
#include "EtManage.h"

IQTWrapper::IQTWrapper(QWidget *measureWidget, QObject *parent) : IRecipeWrapper(parent)
{
    m_measureWidget = measureWidget;
    IQTModel::instance();
}

IQTWrapper::~IQTWrapper()
{
}

void IQTWrapper::Invoke(QString cmd, QString param)
{
    try
    {
        int startTime = 0;
        int takeTime = 0;
        qWarning() << "---- takt time ---- [IQ " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
                   << "] start.";
        startTime = QDateTime::currentMSecsSinceEpoch();
        Result ret(true, "NULL");
#pragma region MyRegion
        if (cmd == "New")
        {
            ret = createNewResultFile(param);
        }
        //TODO: test
        else if(cmd == "RecipeAsync")
        {
            ret = recipeAsync(param);
        }
        else if (cmd.trimmed() == "History")
        {
            ret = judgeEyebox(param);
        }
        else if (cmd.trimmed() == "SwitchNDFilter")
        {
            ret = switchND(param);
        }
        else if (cmd.trimmed() == "SwitchXYZFilter")
        {
            ret = switchXYZ(param);
        }
        else if (cmd.trimmed() == "SetExposureTime")
        {
            ret = setExpTime(param);
        }
        else if (cmd.trimmed() == "CaptureXYZImage")
        {
            ret = getXyzImage(param, true);
        }
        else if (cmd.trimmed() == "CaptureChromacityImage")
        {
            ret = getxyYImage(param, true);
        }
        else if (cmd.trimmed() == "CalculateOneMetrics")
        {
            ret = calcOneMetris(param);
        }
        else if (cmd.trimmed() == "CaptureYImage")
        {
            ret = captureYImage(param);
        }
        else if (cmd.trimmed() == "NewMetricsTable")
        {
            ret = createMtricTable(param);
        }
        else if (cmd.trimmed() == "NewMetricsHistoryTable")
        {
            ret = createMetricHistoryTable(param);
        }
        else if (cmd.trimmed() == "ExportToCSV")
        {
            ret = exportToCSVFile(param);
        }
        else if (cmd.trimmed() == "TimeCalculate")
        {
            ret = calculateTime(param);
        }
        else if (cmd.trimmed() == "CameraFeature")
        {
            ret = changeCameraFeature(param);
        }
        else if (cmd.trimmed() == "StopGrabbing")
        {
            ret = stopCameraGrabing(param);
        }
        else if (cmd.trimmed() == "CompareMetrics")
        {
            ret = compareMetrics(param);
        }
        else if (cmd.trimmed() == "BrightnessLevel")
        {
            ret = changeBrightnessLevel(param);
        }
        else if (cmd.trimmed() == "CameraConnect")
        {
            ret = CameraConnect(param);
        }
        else if (cmd.trimmed() == "IQType")
        {
            ret = isSLB(param);
        }
        else if (cmd.trimmed() == "CaptureRawImage")
        {
            ret = captureYImage(param);
        }
        else if (cmd.trimmed() == "ImageRotateMode")
        {
            ret = imageRotateMode(param);
        }
        else if (cmd.trimmed() == "CaptureImageFFC")
        {
            ret = captureImageFFC(param);
        }
        else if (cmd.trimmed() == "RX")
        {
            ret = cylinderMirrorChange(param);
        }
        else if (cmd.trimmed() == "OutputDir")
        {
            ret = setOutputDir(param);
        }
        else if (cmd.trimmed() == "AlignmentDir")
        {
            ret = setAlignmentDir(param);
        }
        else if (cmd.trimmed() == "MeasureDir")
        {
            ret = setMeasureDir(param);
        }
        else if (cmd.trimmed() == "EyeboxIndex")
        {
            ret = setEyeboxIndex(param);
        }
        else if (cmd.trimmed() == "ImageNewName")
        {
            ret = setImageNewName(param);
        }
        else if (cmd.trimmed() == "LuminanceStatistics")
        {
            ret = luminanceStatistics(param);
        }
        else if (cmd.trimmed() == "LuminanceIS")
        {
            ret = LuminanceIS(param);
        }
        else if (cmd.trimmed() == "AutoFocus")
        {
            ret = autoFocus(param);
        }
        else if (cmd.trimmed() == "WaitCaptureImageEnd")
        {
            ret = waitCaptureImageEnd(param);
        }
        else if (cmd.trimmed() == "ExposureManage")
        {
            ret = exposureManage(param);
        }
        else
        {
            ret = Result(false, QString("\"%1\" cmd is mismatch, please check the execution fields.").arg(cmd).toStdString());
        }

#pragma endregion

        takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
        qWarning() << "---- takt time ---- [IQ " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
                   << "] end. times:" << takeTime << " ms...";

        if (!ret.success)
        {
            QString message = QString("Recipe [%1:%2] run error, %3").arg(cmd).arg(param).arg(QString::fromStdString(ret.errorMsg));
            LoggingWrapper::instance()->error(message);

            // TODO: test
            //emit SendStatusSignal(CORE::Wrapper_Warning, QString::fromStdString(ret.errorMsg));

            if (ret.errorCode == CORE::Wrapper_Warning)
            {
                emit SendStatusSignal(CORE::Wrapper_Warning, QString::fromStdString(ret.errorMsg));
            }
            else
            {
                emit SendStatusSignal(CORE::Wrapper_Error_fatal, QString::fromStdString(ret.errorMsg));
            }
        }
        else
        {
            emit SendStatusSignal(CORE::Wrapper_Done, QString::fromStdString(ret.errorMsg));
        }
    }
    catch (exception e)
    {
        LoggingWrapper::instance()->error("IQ recipe error," + QString::fromStdString(e.what()));
        emit SendStatusSignal(CORE::Wrapper_Error_fatal, "IQ recipe step error, " + QString::fromStdString(e.what()));
    }
}

void IQTWrapper::notifyStop(bool isstop)
{
    MLColorimeterMode::Instance()->continueRun(!isstop);
    AutoVidModel::instance()->recipeStop(!isstop);

    if(isstop){
        TaskAsync::instance().stopTask();
    }
}

void IQTWrapper::notifyPause(bool isPause)
{
    MLColorimeterMode::Instance()->notifyPause(isPause);
}

bool IQTWrapper::writeFileJson(const char *path, Json json)
{
    std::ofstream ofs(path, std::ios_base::out);
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << json.dump(4);
    ofs.close();
    return true;
}

void IQTWrapper::loadconfig()
{
    std::ifstream jsonFile(m_configPath);
    std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
    jsonFile.close();
    settingJsonObj = Json::parse(contents);
}

void IQTWrapper::updateconfig(Json &Luminance,QStringList list)
{
    for (int i = 2; i < list.size(); i++)
    {
        QStringList paramPair = list.at(i).split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            // Luminance.clear();
            Luminance[key.toStdString()] = val.toInt();
            writeFileJson(m_configPath.c_str(), settingJsonObj);
        }
    }
}

Result IQTWrapper::createNewResultFile(QString param)
{
    MetricsData::instance()->setOutputImageDir("");

    MetricsData::instance()->setDutBarCode("");
    QString csvName = "d:/hydusdata/result";
    QUuid id = QUuid::createUuid();
    QString strGuid = id.toString(QUuid::Id128);
    QStringList list = param.split("/");
    QString metricsOrChroma = list[0].trimmed();
    if (metricsOrChroma == "")
    {
        metricsOrChroma = "Metrics";
    }

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
                }
                else if (key == "ImageNDFilter")
                {
                    MetricsData::instance()->setImageNDFilter(val);
                }
            }
        }
            QDateTime current_date_time = QDateTime::currentDateTime();
            QString current_date_str = current_date_time.toString("yyyyMMdd_hhmmss");
        if (MetricsData::instance()->getDutBarCode().isEmpty())
        {
            //QString current_date_sql_str = current_date_time.toString("yyyyMMddhhmmss");
            MetricsData::instance()->setDutBarCode(current_date_str);
            //GenCSVHeader::SetMetricsHistoryTableTime(current_date_sql_str);
        }

        QString eyeboxlist = MetricsData::instance()->getEyeboxQueue();
        eyeboxlist.remove(',');
        if (eyeboxlist.isEmpty())
        {
            eyeboxlist = (Session::getInstance()->getData()).value("$eyebox$");
        }

        MetricsData::instance()->setCaptureFormatTime(current_date_time);
        int firstIndex = csvName.indexOf("_");
        int lastIndex = csvName.lastIndexOf("_");
        QString dutsn = csvName.mid(firstIndex+1 , lastIndex-firstIndex-1);
        MetricsData::instance()->setDutSN(dutsn);
        int last = csvName.lastIndexOf("\\");
        // QString newSeqPath = rootdir + "/" + currentDate;
        QString newSeqPath = csvName.left(last + 1);
        QString csvFileName = csvName.mid(last + 1, csvName.length());
        MetricsData::instance()->setIQRecipeName(csvFileName + "_" + eyeboxlist + ".csv");
        QString dutIQDir = newSeqPath + MetricsData::instance()->getDutBarCode() + QDir::separator() + "IQ\\";
        bool res = isDirExist(dutIQDir);
        MetricsData::instance()->setMTFImgsDir(dutIQDir);
        MetricsData::instance()->setIQRecipeSeqDir(newSeqPath);
        GenCSVHeader::createMetricsHistoryPathTable(MetricsData::instance()->getDutSN(), dutIQDir);
        
        QString eyeboxId =(Session::getInstance()->getData()).value("$eyebox$");
        //QString eyeboxId;
        if (eyeboxId == NULL)
        {
            eyeboxId = m_box;
        }

        IQTModel::instance()->updatepathItem(dutIQDir,eyeboxId);
        if (res)
        {
            QString newcsvNameWithsuffix = csvName + "_" + eyeboxlist + ".csv";
            QFile file(newcsvNameWithsuffix);
            if (!file.exists())
            {
                bool ok = false;
                if (metricsOrChroma == "Chroma")
                    ok = MLColorimeterMode::Instance()->createResutCSVFile(newcsvNameWithsuffix);
                else
                {
                    ok = IQTModel::instance()->createMetricsResutCSVFile(newcsvNameWithsuffix);
                }
                if (ok)
                {
                    return Result();
                }
                else
                {
                    return Result(false, "create csv file error.");
                }
            }
            else
            {
                return Result();
            }
        }
        else
        {
            return Result(false, "new sequence dir error");
        }
    }
    return Result(false, "Param error.");
}

bool IQTWrapper::isDirExist(QString fullPath)
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

Result IQTWrapper::measure(QString param)
{
    QStringList list = param.split("/");
    bool saveData = true;
    QString name = "";
    int delayTime = 0;
    XyzExposureCache aeCache;

    if (list.size() >= 2)
    {
        // QStringList parmlist = list[1].split("/");
        for each (QString paramstr in list)
        {
            QStringList paramPair = paramstr.split("=");
            if (paramPair.size() == 2)
            {
                QString key = paramPair[0].trimmed();
                QString val = paramPair[1].trimmed();
                if (key == "SaveData")
                {
                    saveData = val.toInt();
                }
                else if (key == "AutoExposure")
                {
                    bool auExp = val.toInt();
                    MLColorimeterMode::Instance()->autoExposure(auExp, val.toInt() > 0);
                }
                else if (key == "Name")
                {
                    name = val;
                }
                else if (key == "delayTime")
                {
                    delayTime = val.toInt();
                }
                else if (checkStrEquals(key, "AutoExposureTime"))
                {
                    QStringList timeList = val.split(",");
                    if (timeList.size() < 3)
                    {
                        return Result(false,
                                      "Auto exposure time number is must 3, for example <AutoExposureTime:10,20,30>");
                    }

                    aeCache.isAuto = true;
                    aeCache.initTime["X"] = timeList.at(0).toInt();
                    aeCache.initTime["Y"] = timeList.at(1).toInt();
                    aeCache.initTime["Z"] = timeList.at(2).toInt();
                }
                else if (checkStrEquals(key, "ManualExposureTime"))
                {
                    QStringList timeList = val.split(",");
                    if (timeList.size() < 3)
                    {
                        return Result(false,
                                      "Manual exposure time number is must 3, for example <AutoExposureTime:10,20,30>");
                    }

                    aeCache.isAuto = false;
                    aeCache.initTime["X"] = timeList.at(0).toInt();
                    aeCache.initTime["Y"] = timeList.at(1).toInt();
                    aeCache.initTime["Z"] = timeList.at(2).toInt();
                }
            }
        }
    }

    return MLColorimeterMode::Instance()->saveImage_xyY(name, aeCache, saveData);
}

Result IQTWrapper::switchND(QString param)
{
    Result ret;
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            // ND0, Empty, ND1
            if (key == "ND")
            {
               ret  = MLColorimeterMode::Instance()->setNdFilter(val);
               if(!ret.success){
                   return ret;
               }
            }
        }
    }
    return Result();
}
Result IQTWrapper::switchXYZ(QString param)
{
    Result ret;
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "XYZ")
            {
                // X, Y, Z, Empty, Block
                ret = MLColorimeterMode::Instance()->setXxzFilter(val);
                if(!ret.success){
                    return ret;
                }
            }
        }
    }
    return ret;
}

Result IQTWrapper::setExpTime(QString param)
{
    Result ret;
    int startTime = 0;
    int takeTime = 0;
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "Auto")
            {
                ret = MLColorimeterMode::Instance()->autoExposure(val.toInt(), val.toFloat() > 0);
                if(!ret.success){
                    return ret;
                }
            }
            else if (key == "Manual")
            {
                ret = MLColorimeterMode::Instance()->setExposureTime(val.toFloat());
                if (!ret.success)
                {
                    return ret;
                }
            }
            else if (key == "OverExposureFactor")
            {
                double eTime = MLColorimeterMode::Instance()->getExposureTime();
                ret = MLColorimeterMode::Instance()->setExposureTime(eTime * val.toFloat());
                if (!ret.success)
                {
                    return ret;
                }
            }
        }
    }
    return ret;
}

Result IQTWrapper::getXyzImage(QString param, bool isCorrectionImage)
{
    QString filename;
    XyzExposureCache aeCache;
    bool saveRawImage = true;
    bool saveSLBYY = false;
    bool luminanceSave = false;
    Result ret = anlyzeCaptureImage(filename, aeCache, saveRawImage, saveSLBYY, luminanceSave, param);
    if (!ret.success)
    {
        return ret;
    }

    return MLColorimeterMode::Instance()->saveImage_XYZ(filename, aeCache, saveRawImage, saveSLBYY);
}

Result IQTWrapper::getxyYImage(QString param, bool isCorrectionImage)
{
    QString filename;
    XyzExposureCache aeCache;
    bool saveRawImage = true;
    bool saveSLBYY = false;
    bool luminanceSave = false;
    Result ret = anlyzeCaptureImage(filename, aeCache, saveRawImage, saveSLBYY, luminanceSave, param);
    if (!ret.success)
    {
        return ret;
    }

    return MLColorimeterMode::Instance()->saveImage_xyY(filename, aeCache, saveRawImage, saveSLBYY);
}

Result IQTWrapper::captureYImage(QString param)
{
    QString filename;
    ImgExposureCache aeCache;
    bool saveRawImage = true;
    Result ret = anlyzeCaptureImage(filename, aeCache, saveRawImage, param);
    if (!ret.success)
    {
        return ret;
    }

    return MLColorimeterMode::Instance()->saveImage_YY(filename, aeCache, saveRawImage);
}

Result IQTWrapper::captureRawImage(QString param)
{
    QString filename;
    ImgExposureCache aeCache;
    bool saveRawImage = true;
    Result ret = anlyzeCaptureImage(filename, aeCache, saveRawImage, param);
    if (!ret.success)
    {
        return ret;
    }

    return MLColorimeterMode::Instance()->saveImage_raw(filename, aeCache);
}

Result IQTWrapper::captureImageFFC(QString param)
{
    QString filename;
    ImgExposureCache aeCache;
    bool saveRawImage = true;
    QString dir;

    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (checkStrEquals(key, "Name"))
            {
                filename = val;
            }
            else if (checkStrEquals(key, "AutoExposureTime"))
            {
                aeCache.isSet = true;

                aeCache.isAuto = true;
                aeCache.initTime = val.toInt();
            }
            else if (checkStrEquals(key, "ManualExposureTime"))
            {
                aeCache.isSet = true;

                aeCache.isAuto = false;
                aeCache.initTime = val.toInt();
            }
            else if (checkStrEquals(key, "SaveRaw"))
            {
                saveRawImage = val.toInt() == 1;
            }
            else if (checkStrEquals(key, "imageInfoDir"))
            {
                dir = val;
            }
        }
    }

    return MLColorimeterMode::Instance()->saveImage_FFC(filename, aeCache, dir);
}

Result IQTWrapper::createMtricTable(QString param)
{
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "Num")
            {
                metricTableNum = val.toInt();
            }
        }
    }
    bool ok = GenCSVHeader::crateMetricsTable(metricTableNum);
    if (ok)
    {
        //GenCSVHeader::writeCsvHeadersToDBAll(metricTableNum);
        GenCSVHeader::writeCsvHeadersToDB(metricTableNum);
        return Result();
    }
    else
    {
        return Result(false, "create Metrics table errors");
    }
}

Result IQTWrapper::createMetricHistoryTable(QString param)
{
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "Num")
            {
                metricTableNum = val.toInt();
            }
            else if (key == "DutId")
            {
                MetricsData::instance()->setDutSN(val);
            }
        }
    }
    GenCSVHeader::writeHeadersToHistoryDB();
    bool ok = GenCSVHeader::createMetricsHistoryTable(MetricsData::instance()->getDutSN());
    if (ok)
    {
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date_sql_str = current_date_time.toString("yyyyMMddhhmmss");
        GenCSVHeader::SetMetricsHistoryTableTime(current_date_sql_str);

        return Result();
    }
    else
    {
        return Result(false, "create Metrics History table errors");
    }
}

Result IQTWrapper::calcOneMetris(QString param)
{
    try
    {
        Result re;
        int index = param.indexOf("/");
        QStringList list = param.split("/");
        QString eyeboxids = "";

        for each (QString paramstr in list)
        {
            QStringList paramPair = paramstr.split("=");
            if (paramPair.size() < 2)
            {             
                continue;
            }

            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key.toLower() == "name") {
                QString name = val;
                int length = name.size();
                int sIndex = index + length;
                QString txtStr = param.mid(sIndex + 1);
                QString line = getTxtLine(name, txtStr);
                saveMetricsInfo(line);

                if (list.size() >= 3)
                {
                    QStringList paramPair = list.at(2).split("=");
                    if (paramPair.size() == 2)
                    {
                        QString key = paramPair[0].trimmed();
                        QString val = paramPair[1].trimmed();
                        if (key == "EyeboxIds")
                        {
                            eyeboxids = val;
                        }
                    }
                    //add by ning to write to the json file
                    loadconfig();
                    if (name == "SetLuminanceInfo")
                    {
                        Json& Luminance = settingJsonObj["RecipeSetLuminanceInfo"];
                        updateconfig(Luminance, list);
                        QString str = "";
                        for (int i = 2; i < list.size() - 1; i++)
                        {
                            str = str + list[i] + "/";
                        }
                        str = str + list[list.size() - 1];
                        MetricsData::instance()->setLuminanceInfo(str);
                        return Result();
                    }
                    if (name == "Luminance")
                    {
                        Json& Luminance = settingJsonObj["Luminance"];
                        updateconfig(Luminance, list);
                        QString str = "";
                        for (int i = 2; i < list.size() - 1; i++)
                        {
                            str = str + list[i] + "/";
                        }
                        str = str + list[list.size() - 1];
                        MetricsData::instance()->setLuminancePara(str);
                    }
                    else if (name.contains("LuminanceEfficiency"))
                    {
                        Json& Luminance = settingJsonObj["LuminanceEfficiency"];
                        updateconfig(Luminance, list);
                        QString str = "";
                        for (int i = 2; i < list.size() - 1; i++)
                        {
                            str = str + list[i] + "/";
                        }
                        str = str + list[list.size() - 1];

                        MetricsData::instance()->setLuminanceEfficiencyPara(str);
                    }
                    else if ((name.contains("Chrominance")))
                    {
                        Json& Luminance = settingJsonObj["Chrominance"];
                        updateconfig(Luminance, list);
                        QString str = "";
                        for (int i = 2; i < list.size() - 1; i++)
                        {
                            str = str + list[i] + "/";
                        }
                        str = str + list[list.size() - 1];

                        MetricsData::instance()->setChrominancePara(str);
                    }
                    // add end
                }
                re = IQTModel::instance()->calulateOneMetricsByRecipe(name, eyeboxids);
            }
        }

        if (!re.success)
        {
            return Result(false, ("Metrics calulated error:" +re.errorMsg),
                         CORE::Wrapper_Warning);
        }
        return Result();
    }
    catch (exception e)
    {
        return Result(false, QString("metrics calulated error:" + QString::fromStdString(e.what())).toStdString(),
                      CORE::Wrapper_Warning);
    }
}
Result IQTWrapper::exportToCSVFile(QString param)
{
    QString csvFileName = MetricsData::instance()->getIQRecipeSeqDir() + MetricsData::instance()->getIQRecipeName();
    IQTModel::instance()->saveRecipeMetricsToCsv(csvFileName, metricTableNum);
    bool ret = IQTModel::instance()->transposeCsv(csvFileName);
    if (!ret)
    {
        return Result(false, "Please remove or rename the existing CSV file from the result folder");
    }
    else
    {
        return Result();
    }
}

Result IQTWrapper::calculateTime(QString param)
{
    QStringList newList = param.split("=");
    QString type = newList[0].remove(0,1);
    if (type == "Start")
        start = QDateTime::currentDateTime();
    if (type == "End")
    {
        QDateTime end = QDateTime::currentDateTime();
        int totalTime = start.secsTo(end); 
        modeTime.insert(newList[1], totalTime);
        LoggingWrapper::instance()->debug(QString("Recipe about %1 Ended----The running totalTime is:%2 seconds")
                                              .arg(newList[1])
                                              .arg(QString::number(totalTime)));
    }

    return Result();
}

Result IQTWrapper::changeCameraFeature(QString param)
{
    Result ret;

    QStringList list = param.split("/");
    int startTime = 0;
    int takeTime = 0;
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key.toLower() == "binning")
            {
                ret = MLColorimeterMode::Instance()->setBinning(val.toInt());
                if(!ret.success){
                    return ret;
                }
            }
            else if (key.toLower() == "binningmode")
            {
                ret = MLColorimeterMode::Instance()->setBinningMode(val);
                if (!ret.success)
                {
                    return ret;
                }
            }
            else if (key.toLower() == "bitdepth")
            {
                ret = MLColorimeterMode::Instance()->setBitDepth(val.toInt());
                if (!ret.success)
                {
                    return ret;
                }
            }
            else if (key.toLower() == "focus")
            {
                ret = MLColorimeterMode::Instance()->setFocus(val.toDouble() * 1000.0f);
                if (!ret.success)
                {
                    return ret;
                }
            }
        }
    }
    return ret;
}

Result IQTWrapper::stopCameraGrabing(QString param)
{
    Result ret;
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if(key == "Stop"){
                bool stopBool = (val.toLower() == "true");
                ret = MLColorimeterMode::Instance()->stopGrabbing(stopBool);
            }
        }
    }

    return ret;
}

Result IQTWrapper::compareMetrics(QString param)
{
    IQTModel::instance()->updateCompareCSVHeaderValues(); 
    return Result();
}

Result IQTWrapper::changeBrightnessLevel(QString param)
{
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed().toLower();
            QString val = paramPair[1].trimmed();
            if (key == "level")
            {
                IQTModel::instance()->changeAlogParam("level", val);
            }
        }
    }

    return Result();
}

Result IQTWrapper::CameraConnect(QString param)
{
    return MLColorimeterMode::Instance()->connect();

    //return ((MeasureWidget *)m_measureWidget)->connectCamera();
}

QString IQTWrapper::getTxtLine(QString name, QString line)
{
    QString str = "name:" + name + line.replace("/", ";");
    return str;
}

Result IQTWrapper::judgeEyebox(QString param)
{
    if (getEyebox(param).isEmpty())
        return Result(false, "get eyebox failed!");
    else
        return Result();
}

QString IQTWrapper::getEyebox(QString param)
{
    QStringList list = param.split("=");
    QString eyebox = list[1];
    m_box = eyebox;
    return eyebox;
}

Result IQTWrapper::saveMetricsInfo(QString line)
{
    QString imgPaths = MetricsData::instance()->getMTFImgsDir();
    QFile infoFile(imgPaths + "info.txt");
    if (!infoFile.open(QIODevice::Append))
    {
        return Result(false, "Save info.txt failed, open file error.");
    }
    infoFile.write(line.toUtf8());
    infoFile.write("\n");
    infoFile.close();
    return Result();
}

Result IQTWrapper::isSLB(QString param)
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
                if (key == "IsSLB")
                {
                    bool saveData = val.toInt() > 0;
                    MetricsData::instance()->setIQSLB(saveData);
                }
            }
        }
    }
    return Result();
}

Result IQTWrapper::imageRotateMode(QString param)
{
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "Type")
            {
                ((MeasureWidget *)m_measureWidget)->setImageRotateMode(val == "SLB");
            }
        }
    }
    return Result();
}

Result IQTWrapper::cylinderMirrorChange(QString param)
{
    QString astigma;
    int degree;
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
                if (key == "astigmia")
                {
                    astigma = val;
                }
                else if (key == "degree")
                {
                    degree = val.toInt();
                }
            }
        }
    }

    return MLColorimeterMode::Instance()->setCylinder(astigma, degree);
}

Result IQTWrapper::setOutputDir(QString param)
{
    QString outputRootDir;
    QString taskNumberName;

    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "outputRootDir")
            {
                outputRootDir = val;
            }
            else if (key == "taskNumberName")
            {
                taskNumberName = val;
            }
        }
    }

    QString eyeType = MetricsData::instance()->getReticleEyeType() == 0 ? "Left" : "Right";
    QString rootPath = outputRootDir + "\\" + taskNumberName + "\\" + 
        QString("%1 samples").arg(eyeType);
    MetricsData::instance()->setOutputRootDir(rootPath);

    QDir dir(rootPath);
    if (!dir.exists())
    {
        if (!dir.mkpath(rootPath))
        {
            return Result(false, "Create root dir failed.");
        }
    }
    return Result();
}

Result IQTWrapper::setAlignmentDir(QString param)
{
    QString dirName;
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "dirName")
            {
                dirName = val;
            }
        }
    }

    QString dirPath = MetricsData::instance()->getOutputRootDir() + "\\" + dirName;
    MetricsData::instance()->setAlignImageDir(dirPath);

    QDir dir(dirPath);
    if (!dir.exists())
    {
        if (!dir.mkpath(dirPath))
        {
            return Result(false, "Create alignment image dir failed.");
        }
    }

    return Result();
}

Result IQTWrapper::setMeasureDir(QString param)
{
    QString projectorSn;
    QString dutSN;
    QString recipeTitle;
    QString time;

    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "projectorSn")
            {
                projectorSn = val;
            }
            else if (key == "dutSN")
            {
                dutSN = val;
            }
            else if (key == "recipeTitle")
            {
                recipeTitle = val;
            }
            else if (key == "time")
            {
                time = val;
            }
        }
    }

    QString eyeType = MetricsData::instance()->getReticleEyeType() == 0 ? "left" : "right";
    QString imagePath = MetricsData::instance()->getOutputRootDir() + "\\" + QString("%1_%2_%3_%4").
        arg(projectorSn).
        arg(dutSN).
        arg(eyeType).
        arg(recipeTitle);

    if (!time.isEmpty())
    {
        imagePath += "_" + time;
    }
    MetricsData::instance()->setOutputImageDir(imagePath);

    QDir dir(imagePath);
    if (!dir.exists())
    {
        if (!dir.mkpath(imagePath))
        {
            return Result(false, "Create output image dir failed.");
        }
    }

    MetricsData::instance()->setDutBarCode(dutSN);
    Result ret = createCsv();
    if (!ret.success)
    {
        return ret;
    }

    ret = MLColorimeterMode::Instance()->loadPatternNameFile();
    return ret;
}

Result IQTWrapper::setEyeboxIndex(QString param)
{
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "index")
            {
                MetricsData::instance()->setEyeboxIndexCurrent(val.toInt());
            }
        }
    }
    return Result();
}

Result IQTWrapper::setImageNewName(QString param)
{
    Result ret = MLColorimeterMode::Instance()->updateOutputImageName();
    return ret;
}

Result IQTWrapper::luminanceStatistics(QString param)
{
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "LuminanceFile")
            {
                MetricsData::instance()->setLuminanceFile(val);
            }
        }
    }
    return Result();
}

Result IQTWrapper::LuminanceIS(QString param)
{
    QString file = MetricsData::instance()->getLuminanceFile();
    QString color = MetricsData::instance()->getColor();
    Result ret = LumStatistics::instance().saveISlumCsv(file, color);
    return ret;
}

Result IQTWrapper::autoFocus(QString param)
{
    QString filename;
    ImgExposureCache aeCache;
    bool saveImage = false;
    Result ret = anlyzeCaptureImage(filename, aeCache, saveImage, param);
    if (!ret.success)
    {
        return ret;
    }

    float diopters;
    ret = AutoVidModel::instance()->recipeAutoFocus(diopters, aeCache, filename, saveImage);
    return ret;
}

Result IQTWrapper::waitCaptureImageEnd(QString param)
{
    Result ret = TaskAsync::instance().waitAllTask();
    return ret;
}

Result IQTWrapper::exposureManage(QString param)
{
    bool customize = false;
    QString dutType;
    bool saveET = false;

    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "customize")
            {
                customize = val.toInt() == 0 ? false : true;
            }
            else if (key == "dutType")
            {
                dutType = val;
            }
            else if (key == "saveET")
            {
                saveET = val.toInt() == 0 ? false : true;
            }
        }
    }

    Result ret = EtManage::instance().setting(customize, saveET, dutType);
    return ret;
}

Result IQTWrapper::recipeAsync(QString param)
{
    bool isAsync = false;
    int time = 0;
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "IsAsync")
            {
                isAsync = val.toInt() == 0 ? false : true;
            }
            else if (key == "time")
            {
                time = val.toInt();
            }
        }
    }

    Result ret;
    if(isAsync){
        ret = RecipeAsync::instance().waitRun(this, &IQTWrapper::testData, time);
    }else{
        ret = RecipeAsync::instance().waitEnd();
    }
    return ret;
}

Result IQTWrapper::testData(int mSecond)
{
    QString threadInfo = QString("[%1] 0x%2")
        .arg(QThread::currentThread()->objectName())
        .arg(reinterpret_cast<quintptr>(QThread::currentThread()), 0, 16);

    qCritical() << "0000" << QThread::currentThreadId() << "start";
    Sleep(mSecond);

    qCritical() << "0000" << QThread::currentThreadId() << "end";
    return Result(false, QString("error %1.").arg(threadInfo).toStdString());
}

bool IQTWrapper::checkStrEquals(QString str0, QString str1)
{
    return str0.toLower() == str1.toLower();
}

Result IQTWrapper::anlyzeCaptureImage(QString &fileName, XyzExposureCache &aeCache, bool &saveRawImage, bool &saveSLBYY, bool &luminanceSave, QString param)
{
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (checkStrEquals(key, "Name"))
            {
                fileName = MetricsData::instance()->getImageNDFilter(val);
            }
            else if (checkStrEquals(key, "AutoExposureTime"))
            {
                QStringList timeList = val.split(",");
                if (timeList.size() < 3)
                {
                    return Result(false, "Auto exposure time number is must 3, for example <AutoExposureTime:10,20,30>");
                }

                aeCache.isAuto = true;
                aeCache.initTime["X"] = timeList.at(0).toInt();
                aeCache.initTime["Y"] = timeList.at(1).toInt();
                aeCache.initTime["Z"] = timeList.at(2).toInt();
            }
            else if (checkStrEquals(key, "ManualExposureTime"))
            {
                QStringList timeList = val.split(",");
                if (timeList.size() < 3)
                {
                    return Result(false, "Manual exposure time number is must 3, for example <AutoExposureTime:10,20,30>");
                }

                aeCache.isAuto = false;
                aeCache.initTime["X"] = timeList.at(0).toInt();
                aeCache.initTime["Y"] = timeList.at(1).toInt();
                aeCache.initTime["Z"] = timeList.at(2).toInt();
            }
            else if (checkStrEquals(key, "SaveRaw"))
            {
                saveRawImage = val.toInt() == 1;
            }
            else if (checkStrEquals(key, "SaveSLBYY"))
            {
                saveSLBYY = val.toInt();
            }
            else if (checkStrEquals(key, "LuminanceSave"))
            {
                luminanceSave = val.toInt() == 1;
            }
        }
    }

    return Result();
}

Result IQTWrapper::anlyzeCaptureImage(QString &fileName, ImgExposureCache &aeCache, bool &saveRawImage, QString param)
{
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (checkStrEquals(key, "Name"))
            {
                fileName = MetricsData::instance()->getImageNDFilter(val);
            }
            else if (checkStrEquals(key, "AutoExposureTime"))
            {
                aeCache.isSet = true;

                aeCache.isAuto = true;
                aeCache.initTime = val.toInt();
            }
            else if (checkStrEquals(key, "ManualExposureTime"))
            {
                aeCache.isSet = true;

                aeCache.isAuto = false;
                aeCache.initTime = val.toInt();
            }
            else if (checkStrEquals(key, "SaveRaw"))
            {
                saveRawImage = val.toInt() == 1;
            }
            else if (checkStrEquals(key, "OverExposureFactor"))
            {
                aeCache.OverExposureFactor = val.toInt();
            }
        }
    }

    return Result();
}

Result IQTWrapper::createCsv()
{
    QString eyeboxlist = MetricsData::instance()->getEyeboxQueue();
    eyeboxlist.remove(',');
    if (eyeboxlist.isEmpty())
    {
        eyeboxlist = (Session::getInstance()->getData()).value("$eyebox$");
    }

    QString csvName = QString("metrics") + "_" + eyeboxlist + ".csv";
    MetricsData::instance()->setIQRecipeName(csvName);

    MetricsData::instance()->setMTFImgsDir(MetricsData::instance()->getOutputImageDir() + "\\");
    MetricsData::instance()->setIQRecipeSeqDir(MetricsData::instance()->getOutputRootDir() + "\\");
    QString dutIQDir = MetricsData::instance()->getOutputRootDir() + "\\";

    QString eyeboxId = (Session::getInstance()->getData()).value("$eyebox$");
    // QString eyeboxId;
    if (eyeboxId == NULL)
    {
        eyeboxId = m_box;
    }

    bool res = isDirExist(dutIQDir);
    if (!res){
        return Result(false, "new output dir error");
    }

    QString newcsvNameWithsuffix = dutIQDir + csvName;
    QFile file(newcsvNameWithsuffix);
    if (!file.exists())
    {
        bool ok = IQTModel::instance()->createMetricsResutCSVFile(newcsvNameWithsuffix);
        if (!ok)
        {
            return Result(false, "create csv file error.");
        }
    }
    return Result();
}

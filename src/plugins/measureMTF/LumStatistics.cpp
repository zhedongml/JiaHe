#include "LumStatistics.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QFileInfo>
#include "integratingSphere/integratingspheretcpmodel.h"
#include <QDir>
#include "IQMetrics/ML_Luminance.h"
using namespace cv;
using namespace MLIQMetrics;

LumStatistics& LumStatistics::instance()
{
    static LumStatistics self;
    return self;
}

LumStatistics::LumStatistics()
{
}

LumStatistics::~LumStatistics()
{
}

Result LumStatistics::saveISluminance(QString XYZFilter, QString lightColor)
{
    float lumIS;
    Result ret = IntegratingSphereTCPModel::getInstance()->getLuminance(lumIS);
    if(!ret.success){
        return ret;
    }

    QString time = QDateTime::currentDateTime().toString("yyyyMMdd_hh:mm:ss.zzz");
    m_lumData.time = time;
    m_lumData.ISLumMap[XYZFilter] = lumIS;
    m_lumData.lightColor = lightColor;
    return Result();
}

Result LumStatistics::saveImagelumCsv(QString file, QString lightColor, cv::Mat image)
{
    if(m_lumData.lightColor == lightColor){
        return Result(false, "Intergrating sphere lumiance is null, light color is " + lightColor.toStdString());
    }

    LuminanceRe re = MLLuminance().getLuminance(image);
    if (!re.flag) {
        return Result(false, re.errMsg);
    }

    m_lumData.imageLum = re.rectMean;

    Result ret = saveCsv(file, m_lumData);
    return ret;
}

Result LumStatistics::saveISlumCsv(QString filePath, QString lightColor)
{
    float lumIS;
    Result ret = IntegratingSphereTCPModel::getInstance()->getLuminance(lumIS);
    if (!ret.success) {
        return ret;
    }

    QString time = QDateTime::currentDateTime().toString("yyyyMMdd_hh:mm:ss.zzz");

    QString firstLine = "Time,ISColor,Luminance(nit)\n";
    ret = createFileIfNotExists(filePath, firstLine);
    if (!ret.success) {
        return ret;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return Result(false, "Open file failed, " + filePath.toStdString());
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    out << QString("%1,%2,%3\n")
        .arg(time)
        .arg(lightColor)
        .arg(QString::number(lumIS, 'f', 2));

    file.close();
    return Result();
}

Result LumStatistics::saveCsv(QString filePath, LumData lumData)
{
    if(!lumData.ISLumMap.contains("X") 
        || !lumData.ISLumMap.contains("Y") 
        || !lumData.ISLumMap.contains("Z"))
    {
        return Result(false, "Save luminance info error, luminance data is missing.");
    }

    QString firstLine = "Time,ISColor,IS_Xfilter_Lum(nit),IS_Yfilter_Lum(nit),IS_Zfilter_Lum(nit),IS_AVG_Lum(nit),MLO_Lum(nit)\n";
    Result ret = createFileIfNotExists(filePath, firstLine);
    if(!ret.success){
        return ret;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return Result(false, "Open file failed, " + filePath.toStdString());
    }

    QTextStream out(&file);
    out.setCodec("UTF-8"); 

    float lumAvgIS = (lumData.ISLumMap["X"] + lumData.ISLumMap["Y"] + lumData.ISLumMap["Z"])/3;
    out << QString("%1,%2,%3,%4,%5,%6,%7\n")
        .arg(lumData.time)
        .arg(lumData.lightColor)
        .arg(QString::number(lumData.ISLumMap["X"], 'f', 2))
        .arg(QString::number(lumData.ISLumMap["Y"], 'f', 2))
        .arg(QString::number(lumData.ISLumMap["Z"], 'f', 2))
        .arg(QString::number(lumAvgIS, 'f', 2))
        .arg(QString::number(lumData.imageLum, 'f', 2));

    file.close();
    return Result();
}

Result LumStatistics::createFileIfNotExists(const QString& filePath, const QString& firstLine)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        QDir dir(fileInfo.absolutePath());
        if (!dir.exists()) {
            if (!dir.mkpath(".")) {
                qDebug() << "Failed to create directory path:" << dir.path();
                return Result(false, "Failed to create directory path:" + dir.path().toStdString());
            }
        }

        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return Result(false, "created file failed, " + filePath.toStdString());
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");

        out << firstLine;
        file.close();
    }

    return Result();
}

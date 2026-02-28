#include "TaskAsync.h"
#include <QDebug>

TaskAsync& TaskAsync::instance(QObject* parent)
{
    static TaskAsync ins(parent);
    return ins;
}

TaskAsync::TaskAsync(QObject* parent) :
    QObject(parent)
{

}

TaskAsync::~TaskAsync()
{
}

//Result TaskAsync::saveImage(const QString& file, cv::Mat image)
//{
//    auto task = QSharedPointer<TaskImageSave>::create(file, image);
//    Result ret = m_managerImage.submit(task);
//    if(!ret.success){
//        qDebug() << "Task Async" << QString::fromStdString(ret.errorMsg);
//    }
//    return ret;
//}
//
//Result TaskAsync::calibrationYY(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
//    CalibrationConfig config, std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> calibrationDatas,
//    QString fileName)
//{
//    auto task = QSharedPointer<TaskCalibrationYY>::create(dataMap, config, calibrationDatas, fileName);
//    Result ret = m_managerCalibrate.submit(task);
//    if (!ret.success) {
//        qDebug() << "Task Async" << QString::fromStdString(ret.errorMsg);
//    }
//    return ret;
//}
//
//Result TaskAsync::calibrationXYZ(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
//    CalibrationConfig config, std::map<ML::MLColorimeter::CalibrationEnum, std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> calibrationDatas, QString fileName, bool saveSLBYY, bool isChroma)
//{
//    auto task = QSharedPointer<TaskCalibrationXYZ>::create(dataMap, config, calibrationDatas, fileName, saveSLBYY, isChroma);
//    Result ret = m_managerCalibrate.submit(task);
//    if (!ret.success) {
//        qDebug() << "Task Async" << QString::fromStdString(ret.errorMsg);
//    }
//    return ret;
//}
//
//Result TaskAsync::calibrationVID(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
//    CalibrationConfig config, std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> calibrationDatas,
//    QString fileName, bool saveImage, AutoVidCallback* callback)
//{
//    auto task = QSharedPointer<TaskCalibrationVid>::create(dataMap, config, calibrationDatas, fileName, saveImage, callback);
//    Result ret = m_managerCalibrate.submit(task);
//    if (!ret.success) {
//        qDebug() << "Task Async" << QString::fromStdString(ret.errorMsg);
//    }
//    return ret;
//}

Result TaskAsync::stopTask()
{
    m_managerImage.stopTask();
    m_managerCalibrate.stopTask();
    return Result();
}


void TaskAsync::stopThread()
{
    m_managerImage.stopThread();
    m_managerCalibrate.stopThread();
}

Result TaskAsync::waitAllTask()
{
    Result ret = m_managerCalibrate.waitEnd("All calibration tasks");
    if (!ret.success) {
        return ret;
    }

    ret = m_managerImage.waitEnd("All image saving tasks");
    return ret;
}

void TaskAsync::notifyResult(Result ret)
{

}

Result TaskAsync::saveImageTest(const QString& file, cv::Mat image)
{
    qDebug() << "111 111 111";

    //cv::Mat src = cv::imread("D:/11/img_mtf_display_w_DUT.tif", -1);
    //for (int i = 0; i < 30; i++) {
    //    //QtConcurrent::run(this, &TaskAsync::saveImage, QString("D:/11/%1.tif").arg(i+1), src);
    //    QThread::msleep(10);
    //}

    //Result ret = m_managerImage.waitEnd("All image saving tasks");
    //qDebug().noquote() << "111 111 111 result:" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << ret.success << "msg:" << QString::fromStdString(ret.errorMsg);
    return Result();
}
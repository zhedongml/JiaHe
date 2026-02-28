#include "SlbManage.h"
#include <QDataStream>
#include "metricsdatabase.h"
#include <QMap>
#include <QByteArray>
#include <QVariant>
#include <QDateTime>
#include <QDebug>
#include <QBuffer>
#include <QImage>
#include <QImageReader>
#include"IQMetrics/MLContrastRatio.h"

Result SlbManage::saveCR(cv::Mat imgP, cv::Mat imgN, QString color, RETICLE_TYPE reticleType)
{
  //  MLImageDetection::MLOptParaChessboard Chessboardpara;
    MLIQMetrics::MLContrastRatio Chessboardpara;
    MLIQMetrics::ContrastRatioRe re=Chessboardpara.getContrastRatio(imgP, imgN);
   // Chessboardpara.SetChessboardImage(imgP, imgN);
   // ChessboardcrRe re = Chessboardpara.GetContrastChessboard();
    return saveMat(re.crMat, color, reticleType);
}

Result SlbManage::saveCR(cv::Mat crMat, QString color, RETICLE_TYPE reticleType)
{
    return saveMat(crMat, color, reticleType);
}

Result SlbManage::queryCR(cv::Mat &crData, QString color, RETICLE_TYPE reticleType)
{
    QMap<QString, QVariant> condition;
    condition.insert(DB_SLB_CR_COLOR, color);
    condition.insert(DB_SLB_CR_TYPE, reticleType);

    MetricsDataBase *db = MetricsDataBase::getInstance();
    QList<QMap<QString, QVariant>> list;
    Result ret = db->queryRecord(list, DB_SLB_CR, condition);
    if (!ret.success)
    {
        return Result(false, QString("Query SLB image failed, color=%1, reticleType=%2, %3")
                                 .arg(color)
                                 .arg(reticleType)
                                 .arg(QString::fromStdString(ret.errorMsg))
                                 .toStdString());
    }
    if (list.size() == 0)
    {
        return Result(false, "Query SLB CR is NULL.");
    }

    QMap<QString, QVariant> record = list.at(list.size() - 1);
    QByteArray data = record[DB_SLB_CR_DATA].toByteArray();
    crData = byteArray2Mat(data);
    return Result();
}

Result SlbManage::saveMat(cv::Mat crData, QString color, RETICLE_TYPE reticleType)
{
    if (crData.data==NULL)
    {
               return Result(false, QString("Save SLB image failed, color=%1, reticleType=%2, sld data is NULL")
                                 .arg(color)
                                 .arg(reticleType)                               
                                 .toStdString());
    }
   
    QByteArray data = mat2ByteArray(crData);

    QMap<QString, QVariant> record;
    record.insert(DB_SLB_CR_COLOR, color);
    record.insert(DB_SLB_CR_TYPE, reticleType);
    record.insert(DB_SLB_CR_DATA, data);
    record.insert(DB_SLB_CR_UPDATE_TIME, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"));

    QMap<QString, QVariant> condition;
    condition.insert(DB_SLB_CR_COLOR, color);
    condition.insert(DB_SLB_CR_TYPE, reticleType);

    MetricsDataBase *db = MetricsDataBase::getInstance();
    Result ret = db->deleteSurplusRecord(DB_SLB_CR, condition, MAX_COUNT);
    if (!ret.success){
        return Result(false, QString("Save SLB image failed, color=%1, reticleType=%2, %3")
                                 .arg(color)
                                 .arg(reticleType)
                                 .arg(QString::fromStdString(ret.errorMsg))
                                 .toStdString());
    }

    return db->insertRecord(DB_SLB_CR, record);

    // QList<QMap<QString, QVariant>> list;
    // Result ret = db->queryRecord(list, DB_SLB_CR, condition);
    // if (!ret.success)
    //{
    //    return Result(false, QString("Save SLB image failed, color=%1, reticleType=%2, %3")
    //                             .arg(color)
    //                             .arg(reticleType)
    //                             .arg(QString::fromStdString(ret.errorMsg))
    //                             .toStdString());
    //}
    //{
    //    return db->updateRecord(DB_SLB_CR, record, condition);
    //}
}

QByteArray SlbManage::mat2ByteArray(const cv::Mat &image)
{
    cv::Mat imageClone = image.clone();
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream << image.type();
    stream << image.rows;
    stream << image.cols;
    const size_t data_size = image.cols * image.rows * image.elemSize();
    QByteArray data = QByteArray::fromRawData((const char *)imageClone.ptr(), data_size);
    stream << data;
    return byteArray;
}

cv::Mat SlbManage::byteArray2Mat(const QByteArray &byteArray)
{
    QDataStream stream(byteArray);
    int matType, rows, cols;
    QByteArray data;
    stream >> matType;
    stream >> rows;
    stream >> cols;
    stream >> data;
    cv::Mat mat(rows, cols, matType, (void *)data.data());
    return mat.clone();
}

void SlbManage::test()
{
    cv::Mat crData = cv::imread("D:/00.tif", -1);
    cv::Rect roi(1362, 891, 50, 69);
    crData = crData(roi);

    Result ret = saveMat(crData, "W", RETICLE_TYPE::CHECKER_BIG);

    cv::Mat retData;
    ret = queryCR(retData, "W", RETICLE_TYPE::CHECKER_BIG);
    qDebug() << ret.success;
}
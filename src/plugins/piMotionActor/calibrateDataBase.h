#ifndef CALIBRATE_DATABASE_H
#define CALIBRATE_DATABASE_H

#include <QObject>
#include <QMutex>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include "PrjCommon/service/ml.h"
#include "Result.h"

using namespace CORE;

struct DutPrNearInfo
{
    int id;
    cv::Point2f dutAngle;
    cv::Point2f prAngle;
    cv::Point3f dutPrNearCoordinate;
};
  // key-ÐÐ
class calibrateDataBase  : public QObject
{
	Q_OBJECT
public:
    static calibrateDataBase *getInstance(QObject *parent = nullptr);
	~calibrateDataBase();
    bool createDutPrNearCalibrateTable();

    Result updateDutPrNearCalibrateValue(cv::Point2f dut, cv::Point2f pr, int eyeType, cv::Point3f col);
    
    Result UpdateDisplayDataInfo(QMap<DUT_EYE_TYPE, QList<DutPrNearInfo>> &info);

    Result deleteRecord(int id);

  private:
    calibrateDataBase(const QString &path, QObject *parent = nullptr);
    bool openDatabase();
    bool closeDatabase();

  private:
    QSqlDatabase m_db;
    static QMutex mutex;
    static calibrateDataBase *self;
    bool isAlreadyOpened = false;
};
#endif

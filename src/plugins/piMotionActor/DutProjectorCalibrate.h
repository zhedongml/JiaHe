#pragma once
#include "PrjCommon/service/ml.h"
#include <QMap>
#include "Result.h"
#include "pimotionactor_global.h"
#include "calibrateDataBase.h"

class PIMOTIONACTOR_EXPORT DutProjectorCalibrate
{
  public:

    static DutProjectorCalibrate *instance();
    ~DutProjectorCalibrate();

    Result queryDatas(QMap<DUT_EYE_TYPE, QList<DutPrNearInfo>> &info);

    Result getDutPrNearCoordinate(DUT_EYE_TYPE eyeType, cv::Point3f &dutPrNearCoordinate);
    Result getDutPrNearCoordinatesByEye(DUT_EYE_TYPE eyeType, QList<DutPrNearInfo> &dutPrNearInfoList);

    Result getDutPrNearCoordinate(DUT_EYE_TYPE eyeType, cv::Point2f dutAngle, cv::Point2f projectorAngle, cv::Point3f &dutPrNearCoordinate);
    bool createDutPrNearCalibrateTable();

    Result updateDutPrNearCalibrateTableValue(cv::Point2f dut, cv::Point2f pr, int eyeType, cv::Point3f col);
    //QMap<DUT_EYE_TYPE, QList<DutPrNearInfo>> getInfoMap();

    Result deleteRecord(int id);

  private:
    DutProjectorCalibrate();

  private:
    static DutProjectorCalibrate *m_instance;
    QMap<DUT_EYE_TYPE, QList<DutPrNearInfo>> m_dutPrNearInfoMap;
    const float UNIT_CONVERSION_T = 1000.0;
};

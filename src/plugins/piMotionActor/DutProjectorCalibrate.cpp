#include "DutProjectorCalibrate.h"
#include <QMutex>
#include "Motion2DModel.h"

DutProjectorCalibrate *DutProjectorCalibrate::m_instance = nullptr;

DutProjectorCalibrate *DutProjectorCalibrate::instance()
{
    if (!m_instance)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance)
        {
            m_instance = new DutProjectorCalibrate();
        }
    }
    return m_instance;
}

DutProjectorCalibrate::DutProjectorCalibrate()
{
    queryDatas(m_dutPrNearInfoMap);
    return;

    //TODO:test data
    {
        DutPrNearInfo info;
        info.dutAngle = cv::Point2f(0, 0);
        info.prAngle = cv::Point2f(0, 0);
        info.dutPrNearCoordinate = cv::Point3f(-52, -11, 79);

        DutPrNearInfo info2;
        info2.dutAngle = cv::Point2f(4, 9);
        info2.prAngle = cv::Point2f(8, 9);
        info2.dutPrNearCoordinate = cv::Point3f(-75.841, 36.34, 92.5);

        QList<DutPrNearInfo> listLeft;
        listLeft.append(info);
        listLeft.append(info2);

        DutPrNearInfo info11;
        info11.dutAngle = cv::Point2f(0, 0);
        info11.prAngle = cv::Point2f(0, 0);
        info11.dutPrNearCoordinate = cv::Point3f(-52.68, -34.765, 81);

        DutPrNearInfo info22;
        info22.dutAngle = cv::Point2f(4, -9);
        info22.prAngle = cv::Point2f(8, -9);
        info22.dutPrNearCoordinate = cv::Point3f(-76.299, -83.916, 163.5);

        QList<DutPrNearInfo> listRight;
        listRight.append(info11);
        listRight.append(info22);

        m_dutPrNearInfoMap[DUT_EYE_LEFT] = listLeft;
        m_dutPrNearInfoMap[DUT_EYE_RIGHT] = listRight;
    }
}

bool DutProjectorCalibrate::createDutPrNearCalibrateTable()
{
    return calibrateDataBase::getInstance()->createDutPrNearCalibrateTable();
}

Result DutProjectorCalibrate::updateDutPrNearCalibrateTableValue(cv::Point2f dut, cv::Point2f pr, int eyeType,
                                                               cv::Point3f col)
{
    Result ret = calibrateDataBase::getInstance()->updateDutPrNearCalibrateValue(dut, pr, eyeType, col);
    if (ret.success)
    {
        ret = queryDatas(m_dutPrNearInfoMap);
    }
    return ret;
}

Result DutProjectorCalibrate::deleteRecord(int id)
{
    return calibrateDataBase::getInstance()->deleteRecord(id);
}

//QMap<DUT_EYE_TYPE, QList<DutPrNearInfo>> DutProjectorCalibrate::getInfoMap()
//{
//    return m_dutPrNearInfoMap;
//}

DutProjectorCalibrate::~DutProjectorCalibrate()
{
}
Result DutProjectorCalibrate::queryDatas(QMap<DUT_EYE_TYPE, QList<DutPrNearInfo>> &info)
{
    return calibrateDataBase::getInstance()->UpdateDisplayDataInfo(info);
}
Result DutProjectorCalibrate::getDutPrNearCoordinate(DUT_EYE_TYPE eyeType, cv::Point3f &dutPrNearCoordinate)
{
    if (!Motion2DModel::getInstance(ACS2DDUT)->Motion2DisConnected())
    {
        return Result(false, "DUT tilt motion is not connected.");
    }

    if (!Motion2DModel::getInstance(ACS2DPro)->Motion2DisConnected())
    {
        return Result(false, "Projector tilt motion is not connected.");
    }

    CORE::ML_Point dutCoord = Motion2DModel::getInstance(ACS2DDUT)->getPosition();
    cv::Point2f dutAngle = cv::Point2f(dutCoord.x / UNIT_CONVERSION_T, dutCoord.y / UNIT_CONVERSION_T);

    CORE::ML_Point prCoord = Motion2DModel::getInstance(ACS2DPro)->getPosition();
    cv::Point2f prAngle = cv::Point2f(prCoord.x / UNIT_CONVERSION_T, prCoord.y / UNIT_CONVERSION_T);
    return getDutPrNearCoordinate(eyeType, dutAngle, prAngle, dutPrNearCoordinate);
}

Result DutProjectorCalibrate::getDutPrNearCoordinatesByEye(DUT_EYE_TYPE eyeType, 
                                                           QList<DutPrNearInfo> &dutPrNearInfoList)
{
    if (!m_dutPrNearInfoMap.contains(eyeType))
    {
        return Result(false,
                      QString("DUT eye type is %1, DUT and projector cant/tilt is not exist near coordinate info, please manual calibrate.")
                          .arg(eyeType).toStdString());
    }

    dutPrNearInfoList = m_dutPrNearInfoMap[eyeType];
    return Result();
}

Result DutProjectorCalibrate::getDutPrNearCoordinate(DUT_EYE_TYPE eyeType, cv::Point2f dutAngle,
                                                     cv::Point2f projectorAngle, cv::Point3f &dutPrNearCoordinate)
{
    if (!m_dutPrNearInfoMap.contains(eyeType))
    {
        return Result(false, "Current DUT and projector cant/tilt is not exist near coordinate info, please manual calibrate.");
    }

    QList<DutPrNearInfo> infoList = m_dutPrNearInfoMap.value(eyeType);
    float min = 0.05;
    for (DutPrNearInfo info : infoList)
    {
        if ((abs(info.dutAngle.x - dutAngle.x) < min) && (abs(info.dutAngle.y - dutAngle.y) < min) &&
            (abs(info.prAngle.x - projectorAngle.x) < min) && (abs(info.prAngle.y - projectorAngle.y) < min))
        {
            dutPrNearCoordinate = info.dutPrNearCoordinate;
            return Result();
        }
    }

    return Result(false, "Current DUT and projector cant/tilt is not exist near coordinate info, please manual calibrate.");
}



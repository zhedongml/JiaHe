#pragma once
#include <QtWidgets/QMainWindow>
#include <QStandardItemModel.h>
#include <QTableView>
#include <QHeaderView>
#include <QStyleFactory>
#include <QApplication>
#include <QVBoxLayout>
#include "PrjCommon/service/ml_camera.h"

#pragma execution_character_set("utf-8")
class DisplayWidget : public QTableView
{
    Q_OBJECT

public:
    DisplayWidget(QWidget* parent = Q_NULLPTR);
public:
    void SetXY(QList<ML::SpotData> list); //XY for all fiducials
    void SetD1(double z); // z for d1
    void SetD2(double z); // z for d2
    void SetD3(double z); // z for d3
    void SetD4(double z); // z for d4
    void SetTxTyTz(double Tx, double Ty, double Tz);

    void SetDeltaData(double vx, double vy, double vz, double vTx, double vTy, double vTz);

public slots:
    void ClearTableContent();
private:
    QStandardItemModel* m_standItemModel; //创建数据模型
};

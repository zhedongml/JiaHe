#pragma once

//#include "Alignment.h"
#include "ui_CalibrateControlWidget.h"
#include "AlignUtils.h"
#include "Alignment3D.h"
#include <QWidget>
#include <plugins\PLCControl\PLCController.h>
#include <QFuture>
#include <QtConcurrent>

using namespace Align;
using namespace CORE;

class CalibrateControlWidget : public QWidget
{
    Q_OBJECT

  public:
    CalibrateControlWidget(QWidget *parent = nullptr);
    ~CalibrateControlWidget();
    void init();
  private slots:
    void on_btnCalibrate_clicked();
    void on_btnSave_clicked();
    void on_btnGo_clicked();
    void EyeIndexChanged(int index);
    Result startCalibrate(AlignConfig alignConfig);
    void handleFinished();

  private:
    Result acsRelMove(const cv::Point3f &relCoord);

  public:
    bool m_devicesConnectSuccess;

  private:
    std::vector<cv::Point3f> m_eyeboxAcsCoords;

  private:
    Ui::CalibrateControlWidgetClass ui;
    QFutureWatcher<void> watcher;
};

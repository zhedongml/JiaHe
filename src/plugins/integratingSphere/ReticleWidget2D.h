#pragma once

#include <QWidget>
#include "ui_ReticleWidget2D.h"
#include <QFutureWatcher>
#include "PrjCommon/service/ml_motion.h"
#include "Core/itoolbox.h"

class ReticleWidget2D : public Core::IToolBox
{
    Q_OBJECT

  public:
    ReticleWidget2D(QString toolboxName, QWidget *parent = nullptr);
    ~ReticleWidget2D();

  private slots:
    void on_btn_connect_clicked();
    void on_btn_disconnect_clicked();
    void on_comboBox_change_activated(const QString &text);
    void on_comboBox_eyeType_activated(int index);
    void on_btn_absCoord_clicked();
    void on_btn_relCoord_clicked();
    void on_btn_home_clicked();

    void on_btn_stop_clicked();

    void threadFinished();
    void threadFinishedConnect();
    void updateCoordinate(float x, float y);
    void updateMotionStatus(MLMotionState status);

    void updateDutConfigSlot();

  private:
      void init();
      void connectStatus(bool status);
      void setEnabled(bool enabled);
      void setConnectEnabled(bool enabled);
      Result deviceConnect();

  private:
    Ui::ReticleWidget2DClass ui;
    QFutureWatcher<Result> watcher;
    QFutureWatcher<Result> watcherConnect;
};

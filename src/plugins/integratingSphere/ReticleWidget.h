#pragma once

#include <QWidget>
#include "ui_ReticleWidget.h"
#include <QFutureWatcher>
#include "PrjCommon/service/ml_motion.h"
#include "Core/itoolbox.h"

class ReticleWidget : public Core::IToolBox
{
    Q_OBJECT

  public:
    ReticleWidget(QString toolboxName, QWidget *parent = nullptr);
    ~ReticleWidget();

  private slots:
    void on_btn_connect_clicked();
    void on_btn_disconnect_clicked();
    void on_comboBox_change_activated(const QString &text);
    void on_btn_absCoord_clicked();
    void on_btn_relCoord_clicked();

    void threadFinished();
    void threadFinishedConnect();
    void updateCoordinate(float x);
    void updateMotionStatus(MLMotionState status);

  private:
      void init();
      void connectStatus(bool status);
      void setEnabled(bool enabled);
      void setConnectEnabled(bool enabled);

  private:
    Ui::ReticleWidgetClass ui;
    QFutureWatcher<Result> watcher;
    QFutureWatcher<Result> watcherConnect;
};

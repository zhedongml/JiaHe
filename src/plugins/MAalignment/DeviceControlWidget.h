#pragma once

#include "MoveWidget.h"
#include "ui_DeviceControlWidget.h"
#include <piMotionActor/Motion2DModel.h>
#include <QFutureWatcher>
#include <QWidget>
#include <Core/itoolbox.h>

class DeviceControlWidget : public Core::IToolBox
{
    Q_OBJECT

  public:
    DeviceControlWidget(QString toolboxName = "", QWidget *parent = nullptr);
    ~DeviceControlWidget();

    float getStep();
    float getSpeed();
    float getAccSpeed();
    float getDecSpeed();
signals:
    void sendDeviceMoveFinshed(bool);
private slots:
    void sliderValueChangedSlot(int value);
    void decSliderValueChangedSlot(int value);
    void accSliderValueChangedSlot(int value);
    void on_btn_absMove_clicked();
    void on_btn_zero_clicked();
    void on_btn_stop_clicked();
    void on_isHoming_clicked();
    Result on_btn_connect_clicked();
    void on_btn_disconnect_clicked();
    void setDisenable(bool isEnable);
    void handleFinished();

    void updateMotion2DStatusSlot(MLMotionState status);
    void updateMotion2DPositionOriginalSlot(float x, float y);

  private:
    void init();
    void initSolider();
    void initCombox();
    void setEnabled(bool enabled);
    Result isHoming();
    bool messageShow();
  private slots:
    void update2d(float x, float y);
    void setDeviceMoveFinshed(bool);

  private:
    Ui::DeviceControlWidgetClass ui;
    QFutureWatcher<Result> watcher;
    bool isMoveOut=false;
    int type;
};

#pragma once

#include <QWidget>
#include "ui_DeviceControlWidget2D.h"
#include "MoveWidget.h"
#include <QFutureWatcher>
#include <plugins/Core/itoolbox.h>

using namespace CORE;

class DeviceControlWidget2D : public Core::IToolBox
{
    Q_OBJECT

public:
    DeviceControlWidget2D(QString toolboxName = "", QWidget* parent = nullptr);
    ~DeviceControlWidget2D();

    float getStep();
    float getSpeed();

private slots:
    void sliderValueChangedSlot(int value);
    void on_btn_absMove_clicked();
    void on_btn_stop_clicked();
    void on_btn_toSafe_clicked();
    Result on_btn_connect_clicked();
    void on_btn_disconnect_clicked();

    void handleFinished();
    void setDisenable(bool isEnable);
    void updateMotionStatusSlot(MLMotionState status);

private:
    void init();
    void initSolider();
    void initCombox();
    void setEnabled(bool enabled);

    Result moveAbs(const cv::Point2f& coord, int type);
    Result moveAbsSafePos(int type);

private:
    Ui::DeviceControlWidgetClass2D ui;
    QFutureWatcher<Result> watcher;
    int m_type;
};

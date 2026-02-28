#pragma once

#include "ui_DistanceSensorWidget.h"
#include <QWidget>
#include "PrjCommon/service/ml_rangefinder.h"
#include "MLKeyenceLRF/MLKeyenceCL.h"
#include "DistanceManage.h"

using namespace Align;

class DistanceSensorWidget : public QWidget, public RFMotionCallback
{
    Q_OBJECT

public:
    DistanceSensorWidget(QWidget *parent = nullptr);
    ~DistanceSensorWidget();

    void setType(MLKeyenceCLType type);
    void setTitle(const QString &title);

private slots:
    void on_btn_connect_clicked();
    void on_btn_disconnect_clicked();
    void stateChangedSlot(bool old_state, bool new_state);
    void positionSlot(double position);

    void on_btn_update_clicked();

private:
    void NotifyMotionStateChanged(bool old_state, bool new_state) override;
    void NotifyMotionPosition(double position) override;

    // 0 unknow, 1 in range, -1 out range
    void setCheckLabel(int status, const QString &text = "");

signals:
    void stateChangedSignal(bool old_state, bool new_state);
    void positionSignal(double position);

  private:
    Ui::DistanceSensorWidgetClass ui;
    MLKeyenceCLType m_type = MLKeyenceCLType::KEYENCE_TYPE_DOWN;
};

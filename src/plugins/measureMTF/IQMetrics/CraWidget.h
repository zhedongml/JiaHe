#pragma once

#include <QWidget>
#include "ui_CraWidget.h"
#include "MetricsWidgetBase.h"
#include "CraDialog.h"

class CraWidget : public MetricsWidgetBase
{
    Q_OBJECT

public:
    CraWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent = nullptr);
    ~CraWidget();

private slots:
    void on_btn_calculate_clicked();
    void on_btn_rotation_clicked();

    void threadFinished();

protected:
    void initClear() override {}

private:
    void initUI();
    void initAccordion(QAccordion* accordion);
    void initMenu(QMenu* menu);
    Result calculateMetrics();
    Result calculateMetricsRotation();

private:
    Ui::CraWidgetClass ui;
    MLIQMetrics::MLFOVOffset offset;
    MLIQMetrics::FovOffsetRe m_offsetRe;
    MLIQMetrics::MLRotation rotation;
    MLIQMetrics::RotationRe m_rotationRe;
    bool m_isCRA;
};

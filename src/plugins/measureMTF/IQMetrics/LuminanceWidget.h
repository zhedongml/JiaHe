#pragma once
#include <windows.h>
#include "ui_LuminanceWidget.h"
#include "MetricsWidgetBase.h"
#include "LuminanceDialog.h"
#include "MLColorimeterCommon.h"

using namespace ML::MLColorimeter;
using namespace MLIQMetrics;

class LuminanceWidget : public MetricsWidgetBase
{
    Q_OBJECT

public:
    LuminanceWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent = nullptr);
    ~LuminanceWidget();

private slots:
    void on_btn_takeImg_clicked();
    void on_btn_calculate_clicked();
    void threadFinished();

protected:
    void initClear() override {}

private:
    void initUI();
    void initAccordion(QAccordion* accordion);
    void initMenu(QMenu* menu);
    Result calculateMetrics();
    Result takeImage(const QString& xyzFilter, CaptureData& data, bool autoExposure = true, float exposure = 1000);
    Result takeImages();

private:
    Ui::LuminanceWidgetClass ui;

    MLIQMetrics::MLLuminance lumi;

    cv::Mat m_drawImage;
    MLIQMetrics::LuminanceRe m_luminance;

    QMap<QString, CaptureData> m_onlineRawMap;
};

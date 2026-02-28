#pragma once

#include <QWidget>
#include "ui_LumEfficiencyWidget.h"
#include "MetricsWidgetBase.h"
//#include "MLAlgorithm/ml_luminance.h"
#include "Result.h"
#include "MLColorimeterCommon.h"

using namespace ML::MLColorimeter;
using namespace MLImageDetection;

class LumEfficiencyWidget : public MetricsWidgetBase
{
    Q_OBJECT

public:
    LumEfficiencyWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent = nullptr);
    ~LumEfficiencyWidget();

private slots:
    void on_btn_takeImg_clicked();
    void on_btn_calculate_clicked();
    void loadImageSlot(const QString& fileName) override;
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
    Ui::LumEfficiencyWidgetClass ui;
    //EfficiencyRe m_calculateResult;
    //MLLuminance m_luminance;

    QMap<QString, CaptureData> m_onlineRawMap;
};

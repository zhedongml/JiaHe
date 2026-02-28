#pragma once

#include <QWidget>
#include "ui_ChromacityWidget.h"
#include "MetricsWidgetBase.h"
#include "Result.h"
#include "MLColorimeterCommon.h"

using namespace ML::MLColorimeter;
using namespace MLImageDetection;

class ChromacityWidget : public MetricsWidgetBase
{
    Q_OBJECT

public:
    ChromacityWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent = nullptr);
    ~ChromacityWidget();

    void iqMetricsModeChange(bool offline) override;

private slots:
    void on_btn_calculate_clicked();
    void on_btn_loadX_clicked();
    void on_btn_loadY_clicked();
    void on_btn_loadZ_clicked();
    void on_btn_takeImg_clicked();
    void on_checkBox_autoExpose_stateChanged();

    void threadFinished();

protected:
    void initClear() override;

private:
    void initUI();
    void initImageUI();
    void initAccordion(QAccordion* accordion);
    void initMenu(QMenu* menu);
    void clear();

    QString loadImage();
    Result xyzImages(QMap<QString, cv::Mat>& imageMap);
    Result judgeData();
    void message(const std::string& info);
    Result takeImage(const QString& xyzFilter, CaptureData& data, bool autoExposure = true, float exposure = 1000);

    Result calculateMetrics();

private:
    Ui::ChromacityWidgetClass ui;

    QString offlineTipBase = "Please load %1 image.";
    QString onlineTipBase = "%1 image is empty.";
    QString onlineTakeImage = "%1 image is captured.";
    const char* LINEEDIT_DATA = "lineedit_data";

    QMap<QString, CaptureData> m_onlineRawMap;

    //ChromacityRE m_calculateResult;
    //MLLuminance m_luminance;
    QMap<QString, cv::Mat> m_xyzMap;
};

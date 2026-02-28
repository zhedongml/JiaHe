#pragma once

#include <QWidget>
#include "ui_GhostWidget.h"
#include "MetricsWidgetBase.h"
//#include "MLAlgorithm/ml_ghostPara.h"
#include "GhostDialog.h"
#include "Result.h"
#include "MLColorimeterCommon.h"

using namespace ML::MLColorimeter;
using namespace MLImageDetection;

class GhostWidget : public MetricsWidgetBase
{
    Q_OBJECT

public:
    GhostWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent = nullptr);
    ~GhostWidget();

    void iqMetricsModeChange(bool offline) override;

private slots:
    void on_btn_calculate_clicked();
    void on_btn_image_clicked();
    void on_btn_overExp_clicked();
    void on_btn_imageOn_clicked();
    void on_btn_overExpOn_clicked();

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
    Result images(QMap<QString, cv::Mat>& imageMap);
    Result judgeData();
    Result takeImage(CaptureData& data);
    Result calculateMetrics();

private:
    Ui::GhostWidgetClass ui;
    QString offlineTipBase = "Please load %1 image.";
    QString onlineTipBase = "%1 is empty.";
    QString onlineTakeImage = "%1 image is captured.";
    const char* LINEEDIT_DATA = "lineedit_data";

    QMap<QString, CaptureData> m_onlineRawMap;

    //MLGhostPara m_algorithm;
    //GhostRe m_calculateResult;
};

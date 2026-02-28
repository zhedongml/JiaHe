#pragma once

#include <QWidget>
#include "ui_ContrastWidget.h"
#include "MetricsWidgetBase.h"
#include"MLContrastRatio.h"
#include "MLColorimeterCommon.h"

using namespace ML::MLColorimeter;
using namespace MLIQMetrics;

class ContrastWidget : public MetricsWidgetBase
{
    Q_OBJECT

public:
    ContrastWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent = nullptr);
    ~ContrastWidget();

    void iqMetricsModeChange(bool offline) override;

private slots:
    void on_btn_calculate_clicked();
    void on_btn_positive_clicked();
    void on_btn_negative_clicked();
    void on_btn_positiveOn_clicked();
    void on_btn_negativeOn_clicked();

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
    Ui::ContrastWidgetClass ui;

    QString offlineTipBase = "Please load %1 chessboard image.";
    QString onlineTipBase = "%1 chessboard image is empty.";
    QString onlineTakeImage = "%1 chessboard image is captured.";
    const char* LINEEDIT_DATA = "lineedit_data";

    QMap<QString, CaptureData> m_onlineRawMap;

    MLIQMetrics::MLContrastRatio  m_algorithm;
    MLIQMetrics::ContrastRatioRe m_calculateResult;
};

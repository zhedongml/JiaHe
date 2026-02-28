#pragma once
#include "ui_HistogramWidget.h"
#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QAction>

struct HistImageInfo
{
    QImage image;
    double minVal;
    double maxVal;
    double meanVal;
    double stddevVal;
    cv::Point minLoc;
    cv::Point maxLoc;
};


class HistogramWidget : public QWidget
{
    Q_OBJECT

  public:
    HistogramWidget(QWidget *parent = nullptr);
    ~HistogramWidget();
    void calculateHistogram(const cv::Mat &image);
    void setAction(QAction *action);

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void sigUpdateHistogram(HistImageInfo);

  public slots:
    void updataHistogramResult(HistImageInfo);

  private:
    Ui::HistogramWidgetClass ui;
    QAction *m_action = nullptr;
};

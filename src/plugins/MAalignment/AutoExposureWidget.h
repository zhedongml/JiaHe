#pragma once

#include "ui_AutoExposureWidget.h"
#include <QWidget>

class AutoExposureWidget : public QWidget
{
    Q_OBJECT

  public:
    AutoExposureWidget(QWidget *parent = nullptr);
    ~AutoExposureWidget();
  private slots:
    void on_autoexposurebtn_clicked();
    void on_SetExposureTime_clicked();
    void ShowGrayValue(int gray);
    void on_exposuretimetxt_returnPressed();

  private:
    Ui::AutoExposureWidgetClass ui;
};

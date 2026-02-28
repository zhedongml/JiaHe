#pragma once
#include <QWidget>
#include "ui_MinMaxStaWidget.h"
#include <opencv2/opencv.hpp>
#include <QAction>

class MinMaxStaWidget : public QWidget
{
    Q_OBJECT

  public:
    MinMaxStaWidget(QWidget *parent = nullptr);
    ~MinMaxStaWidget();

     void setAction(QAction *action);

  protected:
    void closeEvent(QCloseEvent *event) override;

  public slots: 
    void updateMinMaxSta(double, double, cv::Point, cv::Point);


  private:
    Ui::MinMaxStaClass ui;
    QAction *m_action = nullptr;
};

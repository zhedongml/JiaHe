#pragma once

#include "Core/itoolbox.h"
#include "ui_FilterwheelWidget.h"
#include <QFutureWatcher>
#include <QWidget>
#include "Result.h"
class FilterwheelWidget : public QWidget
{
    Q_OBJECT

  public:
    FilterwheelWidget(QWidget *parent = nullptr);
    ~FilterwheelWidget();
    void on_cur_NDbtn_clicked();

    void on_cur_XYZbtn_clicked();
    void filterSetEnable(bool);
signals:
    void sendRefreshBtn(bool);
  private slots:
    void on_openPushButton_clicked();
    void on_xyzcomboBox_clicked();
    void on_ndcomboBox_activated(int index);
    void on_xyzcomboBox_activated(int index);
    void handleFinished();
    void on_startMoveBtn_clicked();
    void on_stop_clicked();
    void on_checkStateTimer_timeout();
    void on_clearAlarm_clicked();

    void connectStatus(bool connected);

  private:
    Ui::FilterwheelWidgetClass ui;

    bool stopFlag;
    QFutureWatcher<void> watcher1;

    QTimer *m_checkStateTimer;
    bool m_isNDAlarm;
    bool m_isXYZAlarm;
};

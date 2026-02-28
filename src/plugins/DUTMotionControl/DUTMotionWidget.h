#pragma once
#include <QWidget>
#include "Result.h"
#include "itoolbox.h"
#include "ui_DUTMotionWidget.h"
#include <QFutureWatcher>
#include <QObject>
#include <QTimer>

class DUTMotionWidget : public Core::IToolBox
{
    Q_OBJECT

  public:
    DUTMotionWidget(QString toolboxName = "", QWidget *parent = nullptr);
    ~DUTMotionWidget();

private:
    void init();
    void setEnabled(bool enabled);


  private slots:
    void on_btn_connect_clicked();
    void on_btn_disconnect_clicked();

    void on_y_home_clicked();
    void on_x_home_clicked();
    void on_z_home_clicked();

    void on_y_refresh_clicked();
    void on_x_refresh_clicked();
    void on_z_refresh_clicked();

    void on_x_sub_clicked();
    void on_y_sub_clicked();
    void on_z_sub_clicked();
    void on_x_add_clicked();
    void on_y_add_clicked();
    void on_z_add_clicked();

    void on_y_move_clicked();
    void on_x_move_clicked();
    void on_z_move_clicked();
    void on_y_stop_clicked();
    void on_x_stop_clicked();
    void on_z_stop_clicked();

    void on_checkStateTimer_timeout();
    void on_clearAlarm_clicked();
    void handleFinished();
    void moveFinished();

    void RadioButtonSwitch(QAbstractButton*);

  private:
    Ui::DUTMotionWidgetClass ui;
    QButtonGroup* m_buttonGroup;
    QFutureWatcher<Result> watcher;
    QFutureWatcher<Result> watcher_;
    bool m_isRotationAlarm = false;
    bool m_isTiltAlarm = false;
    bool m_isTipAlarm = false;
    QTimer *m_checkStateTimer;
};

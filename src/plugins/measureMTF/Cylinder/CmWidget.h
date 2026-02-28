#pragma once

#include "MLColorimeterMode.h"
#include <QWidget>
#include "ui_CmWidget.h"
#include "Core/itoolbox.h"
#include <QString>
#include <json.hpp>
#include <QFutureWatcher>

using Json = nlohmann::json;

class CmWidget : public Core::IToolBox
{
    Q_OBJECT

  public:
    CmWidget(QString toolboxName, QWidget *parent = nullptr);
    ~CmWidget();

  private slots:
    void on_btn_astigma_clicked();
    void on_comboBox_astigma_activated(const QString &text);
    void on_btn_degreeAbs_clicked();
    void on_btn_clearAlarm_clicked();
    void on_btn_stop_clicked();
    void on_btn_refresh_clicked();

    void on_btn_degreeRelReduce_clicked();
    void on_btn_degreeRelAdd_clicked();
    void on_btn_stepRelReduce_clicked();
    void on_btn_stepRelAdd_clicked();

    void threadFinished();
    void connectStatus(bool connected);
  
  private:
    void init();
    void setBtnEnabled(bool enabled);
    Result setAstigma(QString astigma);
    Result degreeAbs(int degree);
    Result degreeRel(int degree);
    Result stepRel(int steps);

    Result refreshData();
  private:
    Ui::CmWidgetClass ui;
    QFutureWatcher<Result> watcher;
};

#pragma once

#include <QWidget>
#include "ui_integratingspherecontroller.h"
#include "integratingspheretcpmodel.h"
#include "Core/itoolbox.h"
#include <QFutureWatcher>

class IntegratingSphereController : public Core::IToolBox
{
	Q_OBJECT

public:
    IntegratingSphereController(QString toolboxName, QWidget *parent = nullptr);
	~IntegratingSphereController();

private slots:
	void on_openspherebtn_clicked();
	void on_closesoftwarebtn_clicked();
	void on_SetLuminance_clicked();
	void on_comboBox_colorSelect_activated(const QString &text);
    void on_btn_currentSet_clicked();
	void on_btn_refresh_clicked();

	void GetColorCurrentToUI();

	void threadFinishedConnect();
    void connectStatus(bool status, QString msg);
    void refreshData(float currentR, float currentG, float currentB);

private:
	void init();

private:
    Ui::IntegratingSphereController ui;
    IntegratingSphereTCPModel *isTcpModel = nullptr;
    QFutureWatcher<Result> watcherConnect;
};

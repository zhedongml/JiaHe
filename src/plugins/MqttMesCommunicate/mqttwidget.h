#pragma once

#include <QWidget>
#include <QDialog>
#include "ui_mqttwidget.h"

class MqttWidget : public QDialog
{
	Q_OBJECT

public:
	MqttWidget(QWidget *parent = nullptr);
	~MqttWidget();

	void initListWidget();

private slots:
	void on_pushButton_connect_clicked();
	void on_pushButton_disconnect_clicked();
	void on_pushButton_publish_clicked();

	void onMqttStateChanged(int nConnectState);
	void onMqttMessageArrived(const std::string& topic, const std::string& message);

private:
	Ui::MqttWidgetClass ui;
};


#pragma once

#include <QWidget>
#include "ui_PolarizerWidget.h"
#include "ThorlabsMode.h"
#include "Core/itoolbox.h"
#include "Result.h"
#include <QFutureWatcher>

class PolarizerWidget : public Core::IToolBox
{
	Q_OBJECT

public:
	PolarizerWidget(QString toolboxName, QWidget* parent = nullptr);
	~PolarizerWidget();

private slots:
	void on_btn_connect_polarizer_clicked();
	void on_btn_disconnect_polarizer_clicked();
	void on_btn_polarizerMove_clicked();

private:
	void handleFinished();

private:
	Ui::PolarizerWidgetClass ui;
	QFutureWatcher<Result> m_watcher;
};

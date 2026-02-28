#pragma once

#include <QDialog>
#include "ui_multicamerdisplaytest.h"
#include "Core/itoolbox.h"
#include "CameraModel.h"

class multicamerdisplaytest : public Core::IToolBox
{
	Q_OBJECT

public:
	multicamerdisplaytest(QString toolboxName = "", QWidget* parent = nullptr);
	~multicamerdisplaytest();
private slots:
	void on_startPushButton_clicked();

	void display1();

	void display2();

	void display3();
		
private:
	Ui::multicamerdisplaytest ui;

	CameraModel* model;
};

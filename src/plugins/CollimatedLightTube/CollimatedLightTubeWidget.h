#pragma once

#include <QMainWindow>
#include "ui_CollimatedLightTubeWidget.h"
#include "CollimatorWidget.h"
#include "CollimatorAngleCal.h"
#include "MLMindVisionCamera.h"
#include <Core/itoolbox.h>
#include "ml_camera.h"

class CollimatedLightTubeWidget : public Core::IToolBox
{
	Q_OBJECT

public:
	CollimatedLightTubeWidget(QString toolboxName = "", QWidget* parent = nullptr);
	~CollimatedLightTubeWidget();

private:
	void initUi();
	void InitMindVersion();
	void ShowAngle(QString angleX, QString angleY);

	Result connectCamera(QString sn);

private slots:
	void on_crossShow_clicked();
	void on_connect_clicked();
	void on_disconnect_clicked();
	//
	//private slots:
	//	void on_btn_connect_clicked();
	//	void on_btn_disconnect_clicked();
	//	void on_btn_read_clicked();
	//	void on_btn_captureImage_clicked();
	//
	//	void connectStatus(bool status, QString msg);

private:
	Ui::CollimatedLightTubeWidgetClass ui;

	CollimatorWidget* collimatorWidget = nullptr;
};

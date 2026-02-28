#pragma once

#include "windows.h"
#include <QWidget>
#include "ui_SLBWidget.h"
#include <Core/itoolbox.h>
#include <QFutureWatcher>
#include "aligmentWidget.h"

class SLBWidget : public Core::IToolBox
{
	Q_OBJECT

public:
    SLBWidget(aligmentWidget *alignWidget, QString toolboxName = "", QWidget *parent = nullptr);
	~SLBWidget();
    Result alignSLB();

private slots:
    void on_btn_connect_clicked();
    void on_btn_disconnect_clicked();

    void on_btn_moveInitPosition_clicked();
    void on_btn_moveAlignSLB_clicked();
	void on_btn_save_clicked();
	void on_btn_stop_clicked();

    void handleFinished();
    void moveHandleFinished();

private:
    void waitConnect(bool flag);
    void init();
    Result connectDevices();
    void btnEnabled(bool enabled);
    Result moveInit();

    Result moveProjectorTilt();
    Result moveMeasureCamera();
    Result setReticleAndIS();

    Result getProjectorTilt(cv::Point2f &tilt);
    Result getMcPosition(cv::Point3f &mcCoord);
    Result moveMeasureCameraSafe();

private:
	Ui::SLBWidgetClass ui;
	aligmentWidget *m_alignWidget;

    QFutureWatcher<Result> m_watcher;
    QFutureWatcher<Result> m_watcherMove;
};

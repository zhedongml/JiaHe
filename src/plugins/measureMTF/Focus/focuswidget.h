#pragma once

#include "MLColorimeterMode.h"
#include <QWidget>
#include "ui_focuswidget.h"
#include "Core/itoolbox.h"
#include <QButtonGroup>
#include "MotionCommon.h"
#include "Result.h"

#include <QFutureWatcher>

class FocusWidget : public Core::IToolBox, public ML::Motion::MLMotionCallback
{
	Q_OBJECT

public:
	FocusWidget(QString toolboxName = "", QWidget* parent = nullptr);
	~FocusWidget();

public:
	void NotifyMotionStateChanged(ML::Motion::MLMotionState old_state, ML::Motion::MLMotionState new_state) {};
	void NotifyMotionPosition(double position);

private slots:
	//void on_connect1dmotion_clicked();
	void on_disconnect1dmotion_clicked();
	void onButtonClicked(int index);
	void on_move_clicked();
	void on_stop_clicked();
	void on_refreshPos_btn_clicked();
	void on_setpath_clicked();
	void on_autoFocus_clicked();
	void on_Addition_clicked();
	void on_subtraction_clicked();

	void connectStatus(bool connected);
	void ThreadFinshed();
	void onStopButtonClicked();
	void onRoughFocusProgress(double progress);
	void onFineFocusProgress(double progress);

signals:
	void liveImageSignal(bool);

private:
	Result motionConnect();
	Result ThroughFocusProcess();

	//bool m_test = false;
	//void threadTest();

private:
	Ui::FocusWidgetClass ui;
	ActuatorBase* m_motion = nullptr;
	QButtonGroup* m_buttonGroup;
	int m_currentButtonIndex;
	QFutureWatcher<Result> m_watcher;
	bool m_processCallback = false;
	bool m_treeSystemRunning = false;
};

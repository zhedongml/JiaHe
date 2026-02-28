#pragma once

#include <QWidget>
#include "ui_MoveWidget.h"
#include "piMotionActor/Motion3DModel.h"
#include <QFutureWatcher>



class MoveWidget : public QWidget
{
	Q_OBJECT

public:
	MoveWidget(QWidget *parent = nullptr);
	~MoveWidget();

	void setTitle(QString title);
	void setEnabled(bool enabled);

private:
	void init();

protected:
	Ui::MoveWidgetClass ui;
};

/*
* front -x，rear x，left y，right -y，up z，down -z
*/
class MotionMoveWidget :public MoveWidget {
	Q_OBJECT
public:
	MotionMoveWidget(QWidget* parent = nullptr);
	~MotionMoveWidget();

	void setParentWidget(QWidget *widget);
signals:
    void sendMoveFinshed(bool);
    void moveEnabled(bool);
private slots:
	void moveXSub();
	void moveXAdd();
	void moveYAdd();
	void moveYSub();
	//void moveUp();
	//void moveDown();

	void update2dMotionPos(float x, float y);
	void handleFinished();

private:
	void init();

private:
	QWidget *m_parent;
	QFutureWatcher<Result> watcher;
    int type;
};


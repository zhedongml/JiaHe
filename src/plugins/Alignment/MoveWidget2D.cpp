#include "MoveWidget2D.h"
#include "DeviceControlWidget.h"
#include <QtConcurrent>
#include <QFuture>
#include <QMessageBox>
#include <plugins/PrjCommon/metricsdata.h>
#include "piMotionActor/MoveLimitJX.h"

MoveWidget2D::MoveWidget2D(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	init();
}

MoveWidget2D::~MoveWidget2D()
{}

void MoveWidget2D::setTitle(QString title)
{
	ui.label_title->setText(title);
}

void MoveWidget2D::setEnabled(bool enabled)
{
	ui.btn_front->setEnabled(enabled);
	ui.btn_left->setEnabled(enabled);
	ui.btn_rear->setEnabled(enabled);
	ui.btn_right->setEnabled(enabled);
}

void MoveWidget2D::init()
{
	ui.label_xValue->setText("0.00");
	ui.label_yValue->setText("0.00");

	ui.btn_front->setText("");
	ui.btn_rear->setText("");
	ui.btn_left->setText("");
	ui.btn_right->setText("");

	ui.btn_front->setFixedWidth(60);
	ui.btn_rear->setFixedWidth(60);
	ui.btn_left->setFixedWidth(60);
	ui.btn_right->setFixedWidth(60);

	ui.btn_front->setMaximumHeight(60);
	ui.btn_rear->setMaximumHeight(60);
	ui.btn_left->setMaximumHeight(60);
	ui.btn_right->setMaximumHeight(60);

	ui.btn_front->setMinimumHeight(1);
	ui.btn_rear->setMinimumHeight(1);
	ui.btn_left->setMinimumHeight(1);
	ui.btn_right->setMinimumHeight(1);

	ui.btn_front->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	ui.btn_rear->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	ui.btn_left->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	ui.btn_right->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);


	//ui.btn_front->setStyleSheet("QPushButton{background-image:url(:/image/image/move_up.png);}");

	//ui.btn_front->setStyleSheet("QPushButton{image:url(:/image/image/move_up.png);border-style:none;}"\
	//	"QPushButton:pressed{image:url(:); }QPushButton:disabled{image:url(:/image/image/move_up_disenabled.png);}");
	//ui.btn_rear->setStyleSheet("QPushButton{image:url(:/image/image/move_down.png);border-style:none;}"\
	//	"QPushButton:pressed{image:url(:); }QPushButton:disabled{image:url(:/image/image/move_down_disenabled.png);}");
	//ui.btn_left->setStyleSheet("QPushButton{image:url(:/image/image/move_left.png);border-style:none;}"\
	//	"QPushButton:pressed{image:url(:); }QPushButton:disabled{image:url(:/image/image/move_left_disenabled.png);}");
	//ui.btn_right->setStyleSheet("QPushButton{image:url(:/image/image/move_right.png);border-style:none;}"\
	//	"QPushButton:pressed{image:url(:); }QPushButton:disabled{image:url(:/image/image/move_right_disenabled.png);}");
	//ui.btn_up->setStyleSheet("QPushButton{image:url(:/image/image/move_up.png);border-style:none;}"\
	//	"QPushButton:pressed{image:url(:); }QPushButton:disabled{image:url(:/image/image/move_up_disenabled.png);}");
	//ui.btn_down->setStyleSheet("QPushButton{image:url(:/image/image/move_down.png);border-style:none;}"\
	//	"QPushButton:pressed{image:url(:); }QPushButton:disabled{image:url(:/image/image/move_down_disenabled.png);}");
}

MotionMoveWidget2D::MotionMoveWidget2D(QWidget* parent)
	:MoveWidget2D(parent)
{
	init();
}

MotionMoveWidget2D::~MotionMoveWidget2D()
{
}

void MotionMoveWidget2D::setParentWidget(QWidget *widget)
{
    m_parent = widget;
}

void MotionMoveWidget2D::moveXSub() 
{
	float value = ((DeviceControlWidget*)m_parent)->getStep();

    emit moveEnabled(false);
	this->setEnabled(false);

	QFuture<Result> future = QtConcurrent::run(this, &MotionMoveWidget2D::moveRel, cv::Point2f(-value, 0));
	watcher.setFuture(future);
}

void MotionMoveWidget2D::moveXAdd()
{
	float value = ((DeviceControlWidget*)m_parent)->getStep();

    emit moveEnabled(false);
    this->setEnabled(false);
    QFuture<Result> future = QtConcurrent::run(this, &MotionMoveWidget2D::moveRel, cv::Point2f(value, 0));
	watcher.setFuture(future);
}

void MotionMoveWidget2D::moveYAdd()
{
	float value = ((DeviceControlWidget*)m_parent)->getStep();

    emit moveEnabled(false);
    this->setEnabled(false);
    QFuture<Result> future = QtConcurrent::run(this, &MotionMoveWidget2D::moveRel, cv::Point2f(0, value));
	watcher.setFuture(future);
}

void MotionMoveWidget2D::moveYSub()
{
	float value = ((DeviceControlWidget*)m_parent)->getStep();

    emit moveEnabled(false);
    this->setEnabled(false);
    QFuture<Result> future = QtConcurrent::run(this, &MotionMoveWidget2D::moveRel, cv::Point2f(0, -value));
	watcher.setFuture(future);
}

void MotionMoveWidget2D::update2dMotionPos(float x, float y)
{
	ui.label_xValue->setText(QString::number(x, 'f', 3));
	ui.label_yValue->setText(QString::number(y, 'f', 3));
}

void MotionMoveWidget2D::handleFinished()
{
    emit moveEnabled(true);
	this->setEnabled(true);

	Result result = watcher.future().result();
    if (!result.success)
    {
        QMessageBox::critical(this, "Device move error", QString::fromStdString(result.errorMsg), QMessageBox::Ok,
                              QMessageBox::NoButton);
    }
}

void MotionMoveWidget2D::init()
{
	connect(ui.btn_front, &QPushButton::clicked, this, &MotionMoveWidget2D::moveYSub);
	connect(ui.btn_rear, &QPushButton::clicked, this, &MotionMoveWidget2D::moveYAdd);
	connect(ui.btn_left, &QPushButton::clicked, this, &MotionMoveWidget2D::moveXSub);
	connect(ui.btn_right, &QPushButton::clicked, this, &MotionMoveWidget2D::moveXAdd);

	connect(Motion2DModel::getInstance(ACS2DDUTPlane), SIGNAL(updateMotion2DPositionSignal(float, float)), this, SLOT(update2dMotionPos(float, float)));
	connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));

	//front -x，rear x，left y，right -y，up z，down -z
	ui.btn_front->setText("Y-");
	ui.btn_rear->setText("Y+");
	ui.btn_left->setText("X-");
	ui.btn_right->setText("X+");

	ui.btn_front->setFixedHeight(60);
    ui.btn_rear->setFixedHeight(60);
    ui.btn_left->setFixedHeight(60);
    ui.btn_right->setFixedHeight(60);
}

Result MotionMoveWidget2D::moveRel(const cv::Point2f &coord)
{
    return MoveLimitJX::dutMoveRelNoCollision(coord);
}
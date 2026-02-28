#include "MoveWidget.h"
#include "DeviceControlWidget.h"
#include <QtConcurrent>
#include <QFuture>
#include <QMessageBox>
#include <plugins/PrjCommon/metricsdata.h>
#include "piMotionActor/MoveLimitJX.h"
#include "LimitMove.h"

MoveWidget::MoveWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	init();
}

MoveWidget::~MoveWidget()
{}

void MoveWidget::setTitle(QString title)
{
	ui.label_title->setText(title);
}

void MoveWidget::setEnabled(bool enabled)
{
	ui.btn_down->setEnabled(enabled);
	ui.btn_front->setEnabled(enabled);
	ui.btn_left->setEnabled(enabled);
	ui.btn_rear->setEnabled(enabled);
	ui.btn_right->setEnabled(enabled);
	ui.btn_up->setEnabled(enabled);
}

void MoveWidget::init()
{
	ui.label_xValue->setText("0.00");
	ui.label_yValue->setText("0.00");
	ui.label_zValue->setText("0.00");

	ui.btn_front->setText("");
	ui.btn_rear->setText("");
	ui.btn_left->setText("");
	ui.btn_right->setText("");
	ui.btn_up->setText("");
	ui.btn_down->setText("");

	ui.btn_front->setFixedWidth(60);
	ui.btn_rear->setFixedWidth(60);
	ui.btn_left->setFixedWidth(60);
	ui.btn_right->setFixedWidth(60);
	ui.btn_up->setFixedWidth(60);
	ui.btn_down->setFixedWidth(60);

	ui.btn_front->setMaximumHeight(60);
	ui.btn_rear->setMaximumHeight(60);
	ui.btn_left->setMaximumHeight(60);
	ui.btn_right->setMaximumHeight(60);
	ui.btn_up->setMaximumHeight(60);
	ui.btn_down->setMaximumHeight(60);

	ui.btn_front->setMinimumHeight(1);
	ui.btn_rear->setMinimumHeight(1);
	ui.btn_left->setMinimumHeight(1);
	ui.btn_right->setMinimumHeight(1);
	ui.btn_up->setMinimumHeight(1);
	ui.btn_down->setMinimumHeight(1);

	ui.btn_front->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	ui.btn_rear->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	ui.btn_left->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	ui.btn_right->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	ui.btn_up->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	ui.btn_down->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	//ui.btn_front->setFixedSize(60, 60);
	//ui.btn_rear->setFixedSize(60, 60);
	//ui.btn_left->setFixedSize(60, 60);
	//ui.btn_right->setFixedSize(60, 60);
	//ui.btn_up->setFixedSize(60, 60);
	//ui.btn_down->setFixedSize(60, 60);

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

MotionMoveWidget::MotionMoveWidget(QWidget* parent)
	:MoveWidget(parent)
{
	init();
}

MotionMoveWidget::~MotionMoveWidget()
{
}

void MotionMoveWidget::setParentWidget(QWidget* widget)
{
	m_parent = widget;
}

void MotionMoveWidget::moveXSub()
{
	float value = ((DeviceControlWidget*)m_parent)->getStep();

	emit moveEnabled(false);
	this->setEnabled(false);

	QFuture<Result> future = QtConcurrent::run(this, &MotionMoveWidget::moveRel, cv::Point3f(-value, 0, 0), type);
	watcher.setFuture(future);
}

void MotionMoveWidget::moveXAdd()
{
	float value = ((DeviceControlWidget*)m_parent)->getStep();

	emit moveEnabled(false);
	this->setEnabled(false);
	QFuture<Result> future = QtConcurrent::run(this, &MotionMoveWidget::moveRel, cv::Point3f(value, 0, 0), type);
	watcher.setFuture(future);
}

void MotionMoveWidget::moveYAdd()
{
	float value = ((DeviceControlWidget*)m_parent)->getStep();

	emit moveEnabled(false);
	this->setEnabled(false);
	QFuture<Result> future = QtConcurrent::run(this, &MotionMoveWidget::moveRel, cv::Point3f(0, value, 0), type);
	watcher.setFuture(future);
}

void MotionMoveWidget::moveYSub()
{
	float value = ((DeviceControlWidget*)m_parent)->getStep();

	emit moveEnabled(false);
	this->setEnabled(false);
	QFuture<Result> future = QtConcurrent::run(this, &MotionMoveWidget::moveRel, cv::Point3f(0, -value, 0), type);
	watcher.setFuture(future);
}

void MotionMoveWidget::moveUp()
{
	float value = ((DeviceControlWidget*)m_parent)->getStep();

	emit moveEnabled(false);
	this->setEnabled(false);
	QFuture<Result> future = QtConcurrent::run(this, &MotionMoveWidget::moveRel, cv::Point3f(0, 0, value), type);
	watcher.setFuture(future);
}

void MotionMoveWidget::moveDown()
{
	float value = ((DeviceControlWidget*)m_parent)->getStep();

	emit moveEnabled(false);
	this->setEnabled(false);
	QFuture<Result> future = QtConcurrent::run(this, &MotionMoveWidget::moveRel, cv::Point3f(0, 0, -value), type);
	watcher.setFuture(future);
}

void MotionMoveWidget::update3dMotionPos(float x, float y, float z)
{
	m_motion3DPos.x = x;
	m_motion3DPos.y = y;
	m_motion3DPos.z = z;
	ui.label_xValue->setText(QString::number(x, 'f', 3));
	ui.label_yValue->setText(QString::number(y, 'f', 3));
	ui.label_zValue->setText(QString::number(z, 'f', 3));
}

void MotionMoveWidget::handleFinished()
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

void MotionMoveWidget::init()
{
	connect(ui.btn_front, &QPushButton::clicked, this, &MotionMoveWidget::moveYSub);
	connect(ui.btn_rear, &QPushButton::clicked, this, &MotionMoveWidget::moveYAdd);
	connect(ui.btn_left, &QPushButton::clicked, this, &MotionMoveWidget::moveXSub);
	connect(ui.btn_right, &QPushButton::clicked, this, &MotionMoveWidget::moveXAdd);
	connect(ui.btn_up, &QPushButton::clicked, this, &MotionMoveWidget::moveDown);
	connect(ui.btn_down, &QPushButton::clicked, this, &MotionMoveWidget::moveUp);
	type = MetricsData::instance()->getACSType();

	connect(Motion3DModel::getInstance(type), SIGNAL(updateMotion3DPositionSignal(float, float, float)), this, SLOT(update3dMotionPos(float, float, float)));
	connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));

	//front -x，rear x，left y，right -y，up z，down -z
	ui.btn_front->setText("Y-");
	ui.btn_rear->setText("Y+");
	ui.btn_left->setText("X-");
	ui.btn_right->setText("X+");
	ui.btn_up->setText("Z-");
	ui.btn_down->setText("Z+");

	ui.btn_front->setFixedHeight(60);
	ui.btn_rear->setFixedHeight(60);
	ui.btn_left->setFixedHeight(60);
	ui.btn_right->setFixedHeight(60);
	ui.btn_up->setFixedHeight(60);
	ui.btn_down->setFixedHeight(60);

	QPixmap pixmap(":/image/img/direction.png"); 
	ui.direction->setPixmap(pixmap);
}

Result MotionMoveWidget::moveRel(const cv::Point3f& coord, int type)
{
	//std::string msg = AAProcess::LimitMove::getInstance()->motion3DMoveRel(coord, motion3DType(type));
	//if (!msg.empty())
	//{
	//	return Result(false, msg);
	//}
	//return Result();
	Result ret = Motion3DModel::getInstance(type)->Motion3DMoveRel(coord.x * 1000, coord.y * 1000, coord.z * 1000);

	return ret;
}
#include "MoveWidget.h"
#include "DeviceControlWidget.h"
#include <QtConcurrent>
#include <QFuture>
#include <PrjCommon/metricsdata.h>
#include <QMessageBox>

MoveWidget::MoveWidget(QWidget *parent)
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
	ui.btn_front->setEnabled(enabled);
	ui.btn_left->setEnabled(enabled);
	ui.btn_rear->setEnabled(enabled);
	ui.btn_right->setEnabled(enabled);
}

void MoveWidget::init()
{
	ui.label_xValue->setText("0.00");
	ui.label_yValue->setText("0.00");

	ui.btn_front->setText("");
	ui.btn_rear->setText("");
	ui.btn_left->setText("");
	ui.btn_right->setText("");

	ui.btn_front->setFixedWidth(80);
	ui.btn_rear->setFixedWidth(80);
	ui.btn_left->setFixedWidth(80);
	ui.btn_right->setFixedWidth(80);

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

    ui.btn_front->setIcon(QIcon(":/image/image/up.png"));
    ui.btn_rear->setIcon(QIcon(":/image/image/down.png"));
    ui.btn_left->setIcon(QIcon(":/image/image/left.png"));
    ui.btn_right->setIcon(QIcon(":/image/image/right.png"));

	//QPixmap pixmap(":/image/image/direction.png");
	//ui.direction->setPixmap(pixmap);
}

MotionMoveWidget::MotionMoveWidget(QWidget* parent)
	:MoveWidget(parent)
{
	init();
}

MotionMoveWidget::~MotionMoveWidget()
{
}

void MotionMoveWidget::setParentWidget(QWidget *widget)
{
    m_parent = widget;
}

void MotionMoveWidget::moveXSub() {
	this->setEnabled(false);
    emit moveEnabled(false);
	float value = ((DeviceControlWidget*)m_parent)->getStep();
	
	QFuture<Result> future =
        QtConcurrent::run(Motion2DModel::getInstance(type), &Motion2DModel::Motion2DMoveRel, -value, 0);
	watcher.setFuture(future);
}

void MotionMoveWidget::moveXAdd()
{
	this->setEnabled(false);
    emit moveEnabled(false);
	float value = ((DeviceControlWidget*)m_parent)->getStep();

	QFuture<Result> future =
        QtConcurrent::run(Motion2DModel::getInstance(type), &Motion2DModel::Motion2DMoveRel, value, 0);
	watcher.setFuture(future);
}

void MotionMoveWidget::moveYAdd()
{
	this->setEnabled(false);
    emit moveEnabled(false);
	float value = ((DeviceControlWidget*)m_parent)->getStep();

	QFuture<Result> future =
        QtConcurrent::run(Motion2DModel::getInstance(type), &Motion2DModel::Motion2DMoveRel, 0, value);
	watcher.setFuture(future);
}

void MotionMoveWidget::moveYSub()
{
	this->setEnabled(false);
    emit moveEnabled(false);
	float value = ((DeviceControlWidget*)m_parent)->getStep();
    QFuture<Result> future =
        QtConcurrent::run(Motion2DModel::getInstance(type), &Motion2DModel::Motion2DMoveRel, 0, -value);
	watcher.setFuture(future);
}

//void MotionMoveWidget::moveUp()
//{
//	this->setEnabled(false);
//	float value = ((DeviceControlWidget*)m_parent)->getStep();
//	QFuture<Result> future = QtConcurrent::run(Motion2DModel::getInstance(), &Motion2DModel::Motion3DMoveRel, 0, 0, value * 1000);
//	watcher.setFuture(future);
//}
//
//void MotionMoveWidget::moveDown()
//{
//	this->setEnabled(false);
//	float value = ((DeviceControlWidget*)m_parent)->getStep();
//	QFuture<Result> future = QtConcurrent::run(Motion2DModel::getInstance(), &Motion2DModel::Motion3DMoveRel, 0, 0, -value * 1000);
//	watcher.setFuture(future);
//}

void MotionMoveWidget::update2dMotionPos(float x, float y)
{
    if (Motion2DModel::getInstance(type)->Motion2DisConnected())
    {
        ui.label_xValue->setText(QString::number(x, 'f', 3));
        ui.label_yValue->setText(QString::number(y, 'f', 3));
	}
}

void MotionMoveWidget::handleFinished()
{
	this->setEnabled(true);
    emit sendMoveFinshed(true);
    emit moveEnabled(true);

    Result result = watcher.future().result();
    if (!result.success)
    {
        QMessageBox::critical(this, "Device move error", QString::fromStdString(result.errorMsg), QMessageBox::Ok,
                              QMessageBox::NoButton);
    }
}

void MotionMoveWidget::init()
{
    connect(ui.btn_front, &QPushButton::clicked, this, &MotionMoveWidget::moveYAdd);
	connect(ui.btn_rear, &QPushButton::clicked, this, &MotionMoveWidget::moveYSub);
	connect(ui.btn_left, &QPushButton::clicked, this, &MotionMoveWidget::moveXSub);
	connect(ui.btn_right, &QPushButton::clicked, this, &MotionMoveWidget::moveXAdd);
    //connect(ui.btn_up, &QPushButton::clicked, this, &MotionMoveWidget::moveDown);
    //connect(ui.btn_down, &QPushButton::clicked, this, &MotionMoveWidget::moveUp);
    type = MetricsData::instance()->getMotion2DType();
	
    connect(Motion2DModel::getInstance(type), SIGNAL(updateMotion2DPositionSignal(float, float)), this,
            SLOT(update2dMotionPos(float, float)));
	connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));

	//front -x，rear x，left y，right -y，up z，down -z
	ui.btn_front->setText("Y+");
	ui.btn_rear->setText("Y-");
	ui.btn_left->setText("X-");
	ui.btn_right->setText("X+");

	ui.btn_front->setFixedHeight(80);
    ui.btn_rear->setFixedHeight(80);
    ui.btn_left->setFixedHeight(80);
    ui.btn_right->setFixedHeight(80);
}


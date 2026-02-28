#include "DeviceControlWidget.h"
#include <QtConcurrent>
#include <QMessageBox>
#include <plugins/PrjCommon/metricsdata.h>
#include <piMotionActor//ConfigItem.h>
#include "piMotionActor/MoveLimitJX.h"
#include "LimitMove.h"

DeviceControlWidget::DeviceControlWidget(QString toolboxName, QWidget* parent)
	: IToolBox(toolboxName, parent)
{
	if (toolboxName == "Motion3D Imaging")
	{
		MetricsData::instance()->setACSType(withCamera);
		type = withCamera;
	}
	else if (toolboxName == "Motion3D Dut")
	{
		MetricsData::instance()->setACSType(withDUT);
		type = withDUT;

	}
	ui.setupUi(this);
	init();
}

DeviceControlWidget::~DeviceControlWidget()
{
}

float DeviceControlWidget::getStep()
{
	return ui.comboBox_step->currentText().toFloat();
}

float DeviceControlWidget::getSpeed()
{
	return ui.lineEdit_speed->text().toFloat();
}

void DeviceControlWidget::setDisenable(bool isEnable)
{
	ui.btn_absMove->setEnabled(isEnable);
	ui.btn_zero->setEnabled(isEnable);
}

void DeviceControlWidget::updateMotion3DStatusSlot(MLMotionState status)
{
	if (status == MLMotionState::kMLStatusDisConnected)
	{
		Motion3DModel::getInstance(type)->Motion3DUnsubscribeMotionEvent(MLMotionEvent::kPositionChanged);
		ui.label_status->setText("Not Connected.");
	}
	else
	{
		ui.label_status->setText("Connected.");

		Motion3DModel::getInstance(type)->Motion3DSubscribeMotionEvent(MLMotionEvent::kPositionChanged);
		Motion3DModel::getInstance(type)->SetSpeed(getSpeed());
	}
}

void DeviceControlWidget::sliderValueChangedSlot(int value) {
	Motion3DModel::getInstance(type)->SetSpeed(value);
	ui.lineEdit_speed->setText(QString::number(value));
}

void DeviceControlWidget::on_btn_absMove_clicked()
{
	if (ui.lineEdit_x->text().trimmed() == ""
		|| ui.lineEdit_y->text().trimmed() == ""
		|| ui.lineEdit_z->text().trimmed() == "")
	{
		return;
	}
	cv::Point3f coord;
	coord.x = ui.lineEdit_x->text().toFloat();
	coord.y = ui.lineEdit_y->text().toFloat();
	coord.z = ui.lineEdit_z->text().toFloat();

	ui.widget_motion->setEnabled(false);
	setEnabled(false);
	QFuture<Result> future = QtConcurrent::run(this, &DeviceControlWidget::moveAbs, coord, type);
	watcher.setFuture(future);
}

void DeviceControlWidget::on_btn_zero_clicked()
{
	setEnabled(false);
	QFuture<Result> future = QtConcurrent::run(Motion3DModel::getInstance(type), &Motion3DModel::acsMovezeroSqeue);
	watcher.setFuture(future);
}

void DeviceControlWidget::on_btn_stop_clicked()
{
	Motion3DModel::getInstance(type)->Motion3DMoveStop();
}

void DeviceControlWidget::on_btn_toSafe_clicked()
{
	setEnabled(false);
	QFuture<Result> future = QtConcurrent::run(this, &DeviceControlWidget::moveAbsSafePos, type);
	watcher.setFuture(future);
}

void DeviceControlWidget::handleFinished()
{
	this->setEnabled(true);

	Result result = watcher.future().result();
	if (!result.success)
	{
		QMessageBox::critical(this, "Device move error", QString::fromStdString(result.errorMsg), QMessageBox::Ok,
			QMessageBox::NoButton);
	}
}

void DeviceControlWidget::init()
{
	ui.btn_zero->hide();
	ui.widget_motion->setTitle("");
	ui.widget_motion->setParentWidget(this);
	ui.label_status->setText("Not Connected");
	ui.label_status->setWordWrap(true);
	if (type == withCamera)
	{
		ui.groupBox->setTitle("Motion3D Imaging");
	}
	else if (type == withDUT)
	{
		ui.groupBox->setTitle("Motion3D Dut");
	}
	connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));

	initSolider();
	initCombox();
	bool v = connect(ui.widget_motion, &MotionMoveWidget::moveEnabled, this, [=](bool isEnable)
		{ setDisenable(isEnable); });

	connect(Motion3DModel::getInstance(type), SIGNAL(updateMotion3DStatusSignal(MLMotionState)), this, SLOT(updateMotion3DStatusSlot(MLMotionState)));
}

void DeviceControlWidget::initSolider()
{
	QSlider* pSlider = ui.hSlider_speed;
	pSlider->setOrientation(Qt::Horizontal);
	pSlider->setMinimum(1);
	pSlider->setMaximum(20);
	pSlider->setSingleStep(1);
	pSlider->setPageStep(1);

	cv::Point3f speed = MoveLimitConfigJX::instance()->getMotion3DMoveInitSpeed();
	pSlider->setValue(speed.x);
	//pSlider->setValue(RobotModel::getInstance()->getLinespeedNormal());

	ui.lineEdit_speed->setText(QString::number(pSlider->value()));
	ui.lineEdit_speed->setEnabled(false);
	connect(pSlider, &QSlider::valueChanged, this, &DeviceControlWidget::sliderValueChangedSlot);
}

Result DeviceControlWidget::moveAbs(const cv::Point3f& coord, int type)
{
	//return MoveLimitJX::moveAbsNoCollision(coord, motion3DType(type));
	std::string msg = AAProcess::LimitMove::getInstance()->motion3DMoveAbsSync(coord, motion3DType(type));
	if (!msg.empty())
		return Result(false, msg);
	return Result();
}

Result DeviceControlWidget::moveAbsSafePos(int type)
{
	//return MoveLimitJX::moveAbsToSafe(motion3DType(type));
	/*std::string msg = AAProcess::LimitMove::getInstance()->moveAbsToSafePos(motion3DType(type));
	if(!msg.empty())
		return Result(false, msg);*/
	return Result();
}

void DeviceControlWidget::initCombox()
{
	ui.comboBox_step->addItem("0.001");
	ui.comboBox_step->addItem("0.01");
	ui.comboBox_step->addItem("0.1");
	ui.comboBox_step->addItem("0.5");
	ui.comboBox_step->addItem("1");
	ui.comboBox_step->addItem("5");
	ui.comboBox_step->addItem("10");
	ui.comboBox_step->addItem("20");
	ui.comboBox_step->addItem("30");

	ui.comboBox_step->setCurrentIndex(3);
}

void DeviceControlWidget::setEnabled(bool enabled)
{
	ui.btn_absMove->setEnabled(enabled);
	ui.btn_zero->setEnabled(enabled);
	ui.widget_motion->setEnabled(enabled);
}

Result DeviceControlWidget::on_btn_connect_clicked()
{
	QString ipEtc = ConfigItem::instance()->getMotion3DIpAndOther(type);
	Result result = Motion3DModel::getInstance(type)->connectAndInit(ipEtc);
	if (!result.success)
	{
		std::string errMsg = result.errorMsg;
		int index = errMsg.find_first_of(":");
		result.errorMsg = errMsg.substr(0, index);
		ui.label_status->setText("Disconnected." + QString::fromStdString(result.errorMsg));
		return result;
	}

	ui.label_status->setText("Connected");
	return Result();
}

void DeviceControlWidget::on_btn_disconnect_clicked()
{
	Motion3DModel::getInstance(type)->disconnect();
	ui.label_status->setText("Not Connected");
}




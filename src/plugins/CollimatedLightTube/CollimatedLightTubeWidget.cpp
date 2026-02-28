#include "CollimatedLightTubeWidget.h"
#include "CollimatedLightTubeMode.h"
#include "pluginsystem/Services.h"
#include "CollimatedConfig.h"
#include "PLCControl/PLCController.h"
#include <QThread>
#include <QMessageBox>

CollimatedLightTubeWidget* widget = nullptr;

CollimatedLightTubeWidget::CollimatedLightTubeWidget(QString toolboxName, QWidget* parent) :IToolBox(toolboxName, parent)
{
	ui.setupUi(this);

	widget = this;
	initUi();
	InitMindVersion();
}

CollimatedLightTubeWidget::~CollimatedLightTubeWidget()
{
	if (collimatorWidget)
	{
		delete collimatorWidget;
	}
}

void CollimatedLightTubeWidget::initUi()
{
	QFont font1("Microsoft YaHei", 13, 75);
	ui.collimatorX->setStyleSheet("color:green;");
	ui.collimatorY->setStyleSheet("color:green;");
	ui.collimatorX->setFont(font1);
	ui.collimatorY->setFont(font1);
	ui.label_state->setText("Not connected.");
}

void CollimatedLightTubeWidget::ShowAngle(QString angleX, QString angleY)
{
	ui.groupBox_8->setTitle("Collimator(X, Y),unit(s)");
	ui.collimatorX->setText(angleX);
	ui.collimatorY->setText(angleY);
}

void CollimatedLightTubeWidget::InitMindVersion()
{
	CORE::MLCamera* camPtr = CollimatedLightTubeMode::GetInstance()->GetCamera();
	collimatorWidget = new CollimatorWidget(this);
	collimatorWidget->hide();

	connect(CollimatedLightTubeMode::GetInstance(), &CollimatedLightTubeMode::updateCollimatorAngle, this, &CollimatedLightTubeWidget::ShowAngle);
}

void CollimatedLightTubeWidget::on_crossShow_clicked()
{
	PLCController::instance()->collimatorLight(true);
	collimatorWidget->setHidden(false);
	collimatorWidget->setFixedSize(QSize(600, 600));
	collimatorWidget->ZoomInit();
	collimatorWidget->ShowCenterCross();
	collimatorWidget->show();
}

void CollimatedLightTubeWidget::on_connect_clicked()
{
	Result ret = connectCamera(QString::fromStdString(CollimatedConfig::instance()->GetCollimatorSn()));
	if (!ret.success)
	{
		ui.label_state->setText("Connected Failed.");
		QMessageBox::warning(NULL, "Error", "Collimated Light Tube connect error", QMessageBox::Yes);
	}
	else
	{
		ui.label_state->setText("Connected.");
	}
}

void CollimatedLightTubeWidget::on_disconnect_clicked()
{
	bool ret = CollimatedLightTubeMode::GetInstance()->DisConnect();
	if (ret)
		ui.label_state->setText("DisConnected.");
	else
		ui.label_state->setText("DisConnected Failed.");
}

Result CollimatedLightTubeWidget::connectCamera(QString sn)
{
	if (sn.isEmpty())
	{
		return Result(false, "Camera connection failed, SN is empty.");
	}

	Result ret = CollimatedLightTubeMode::GetInstance()->Connect(sn.toStdString().c_str());
	if (!ret.success)
	{
		CollimatedLightTubeMode::GetInstance()->DisConnect();
		QThread::msleep(1000);

		ret = CollimatedLightTubeMode::GetInstance()->Connect(sn.toStdString().c_str());
		if (!ret.success)
		{
			return ret;
		}
	}

	CollimatedLightTubeMode::GetInstance()->SubscribeCameraEvent(MLCameraEvent::kFrameReceived);
	CollimatedLightTubeMode::GetInstance()->SubscribeCameraEvent(CORE::MLCameraEvent::kGrayLevel);

	return Result();
}
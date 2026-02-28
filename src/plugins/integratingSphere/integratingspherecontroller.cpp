#include "integratingspherecontroller.h"
#include <QtConcurrent>
#include "PrjCommon/WaitWidget.h"
#include <QMessageBox>
#include "Core/icore.h"
#include "loggingwrapper.h"

IntegratingSphereController::IntegratingSphereController(QString toolboxName, QWidget *parent)
    : IToolBox(toolboxName, parent)
{
	ui.setupUi(this);
	isTcpModel = IntegratingSphereTCPModel::getInstance(this);
	ui.RadioRed->setChecked(true);
	connect(ui.RadioRed, SIGNAL(clicked()), this, SLOT(GetColorCurrentToUI()));
	connect(ui.RadioGreen, SIGNAL(clicked()), this, SLOT(GetColorCurrentToUI()));
	connect(ui.RadioBlue, SIGNAL(clicked()), this, SLOT(GetColorCurrentToUI()));
	connect(ui.RadioRGB, SIGNAL(clicked()), this, SLOT(GetColorCurrentToUI()));

    connect(&watcherConnect, SIGNAL(finished()), this, SLOT(threadFinishedConnect()));

	ui.text_G->setText("");
    ui.text_R->setText("");
    ui.text_B->setText("");
	ui.label_luminance->setText("");

	ui.label_status->setWordWrap(true);
    connect(isTcpModel, SIGNAL(connectStatus(bool, QString)), this, SLOT(connectStatus(bool, QString)));

	init();
}

IntegratingSphereController::~IntegratingSphereController()
{
    if (isTcpModel->isOpened()){
        isTcpModel->Disconnect();
	}
}

void IntegratingSphereController::on_openspherebtn_clicked()
{
    WaitWidget::instance()->startAnimation();
    QFuture<Result> future = QtConcurrent::run(isTcpModel, &IntegratingSphereTCPModel::Connect);
    watcherConnect.setFuture(future);
}

void IntegratingSphereController::on_closesoftwarebtn_clicked()
{
	isTcpModel->Disconnect();
    refreshData(0,0,0);
}

void IntegratingSphereController::on_SetLuminance_clicked()
{
	// Get value
	double fValue = ui.LuminanceValue->text().toDouble();
	// Get color 
	Color color;
	if (ui.RadioRed->isChecked())
	{
		color = Color::R;
		QString sLed1 =QString::number((fValue /isTcpModel->RCalibrationFactor));
		ui.text_R->setText(sLed1);
		ui.text_G->setText(0);
		ui.text_B->setText(0);
	}
	else if (ui.RadioGreen->isChecked())
	{
		color = Color::G;
		QString sLed2 = QString::number((fValue / isTcpModel->GCalibrationFactor));
		ui.text_G->setText(sLed2);
		ui.text_R->setText(0);
		ui.text_B->setText(0);
	}
	else if (ui.RadioBlue->isChecked())
	{
		color = Color::B;
		QString sLed3 = QString::number((fValue / isTcpModel->BCalibrationFactor));
		ui.text_B->setText(sLed3);
		ui.text_R->setText(0);
		ui.text_G->setText(0);
	}
	else if(ui.RadioRGB->isChecked())
	{
		color = Color::W;
		QString sLed1 = QString::number((fValue*0.3) / isTcpModel->RCalibrationFactor);
		QString sLed2 = QString::number((fValue * 0.65) / isTcpModel->GCalibrationFactor);
		QString sLed3 = QString::number((fValue * 0.05) / isTcpModel->BCalibrationFactor);

		ui.text_R->setText(sLed1);
		ui.text_G->setText(sLed2);
		ui.text_B->setText(sLed3);
	}
	isTcpModel->SetColorIlluminate(color, fValue);
}

void IntegratingSphereController::on_comboBox_colorSelect_activated(const QString &text)
{
	//TODO:test
 //   {
 //       isTcpModel->setFan(false);
 //       return;
	//}

    QVector<ISSolution> list = isTcpModel->getSolutions();
    int index = ui.comboBox_colorSelect->currentIndex();
    ISSolution solution = list.at(index);
	Result ret = isTcpModel->setSolution(solution);
	if (!ret.success)
    {
        QMessageBox::critical(this, "Error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                             QMessageBox::NoButton);
	}
}

void IntegratingSphereController::on_btn_currentSet_clicked()
{
    // TODO:test
    //{
    //    isTcpModel->setFan(true);
    //    return;
    //}

    QMap<QString, float> map;
    map["R"] = ui.doubleSpinBox_currentR->value();
    map["G"] = ui.doubleSpinBox_currentG->value();
    map["B"] = ui.doubleSpinBox_currentB->value();
    Result ret = isTcpModel->setCurrentOutput(map);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                              QMessageBox::NoButton);
    }
}

void IntegratingSphereController::on_btn_refresh_clicked()
{
	refreshData(0, 0, 0);
}

void IntegratingSphereController::GetColorCurrentToUI()
{
	if (ui.RadioRed->isChecked())
	{
		double fValue = isTcpModel->GetColorCurrent(Color::R);
		ui.text_R->setText(QString::number(fValue));
		ui.LuminanceValue->setValue(fValue * isTcpModel->RCalibrationFactor);
	}
	else if (ui.RadioGreen->isChecked())
	{
		double fValue = isTcpModel->GetColorCurrent(Color::G);
		ui.text_G->setText(QString::number(fValue));
		ui.LuminanceValue->setValue(fValue * isTcpModel->GCalibrationFactor);
	}
	else if (ui.RadioBlue->isChecked())
	{
		double fValue = isTcpModel->GetColorCurrent(Color::B);
		ui.text_B->setText(QString::number(fValue));
		ui.LuminanceValue->setValue(fValue * isTcpModel->BCalibrationFactor);
	}
	else if (ui.RadioRGB->isChecked())
	{
		double fR = isTcpModel->GetColorCurrent(Color::R);
		double fG = isTcpModel->GetColorCurrent(Color::G);
		double fB = isTcpModel->GetColorCurrent(Color::B);
		ui.text_R->setText(QString::number(fR));
		ui.text_G->setText(QString::number(fG));
		ui.text_B->setText(QString::number(fB));
		ui.LuminanceValue->setValue(fR* isTcpModel->RCalibrationFactor+fG*isTcpModel->GCalibrationFactor+fB*isTcpModel->BCalibrationFactor);
	}
}

void IntegratingSphereController::threadFinishedConnect()
{
    WaitWidget::instance()->stopAnimation();

    Result ret = watcherConnect.result();
    if (!ret.success)
    {
        ui.label_status->setText(QString::fromStdString(ret.errorMsg));
    }
}

void IntegratingSphereController::connectStatus(bool status, QString msg)
{
    if (status)
    {
        ui.label_status->setText("Connected.");
    }
    else
    {
		QString message = "Not connected.";
        if (!msg.isEmpty()){
            message = msg;
		}
        ui.label_status->setText(message);
    }
}

void IntegratingSphereController::init()
{
	ui.groupBox_luminanceControl->hide();

	ui.doubleSpinBox_currentR->setMinimum(0);
    ui.doubleSpinBox_currentR->setMaximum(2);
    ui.doubleSpinBox_currentG->setMinimum(0);
    ui.doubleSpinBox_currentG->setMaximum(2);
    ui.doubleSpinBox_currentB->setMinimum(0);
    ui.doubleSpinBox_currentB->setMaximum(2);

	QVector<ISSolution> list = isTcpModel->getSolutions();
    for (ISSolution solution : list){
        ui.comboBox_colorSelect->addItem(solution.name);
	}

	connect(isTcpModel, SIGNAL(refreshData(float, float, float)), this, SLOT(refreshData(float, float, float)));
    connect(Core::ICore::instance(), &Core::ICore::coreAboutToClose, this, [this]() {
        if (isTcpModel->isOpened())
        {
            isTcpModel->Disconnect();
        }
    });
}

void IntegratingSphereController::refreshData(float currentR, float currentG, float currentB)
{
	currentR = IntegratingSphereTCPModel::getInstance()->GetColorCurrent(R);
	currentG = IntegratingSphereTCPModel::getInstance()->GetColorCurrent(G);
	currentB = IntegratingSphereTCPModel::getInstance()->GetColorCurrent(B);

    ui.text_R->setText(QString::number(currentR, 'f', 5));
    ui.text_G->setText(QString::number(currentG, 'f', 5));
    ui.text_B->setText(QString::number(currentB, 'f', 5));

	int count = 0;
	currentR > 0.000001 ? count++ : count;
	currentG > 0.000001 ? count++ : count;
	currentB > 0.000001 ? count++ : count;
	Color color=W;

	if (count == 0) {
		ui.label_luminance->setText("0");
	}
	else if (count > 1) {
		ui.label_luminance->setText("---");
	}
	else {
		if (currentR > 0.000001) color = Color::R;
		if (currentG > 0.000001) color = Color::G;
		if (currentB > 0.000001) color = Color::B;
		float lumIS;
		Result ret = IntegratingSphereTCPModel::getInstance()->getLuminance(lumIS,color);
		if (!ret.success) {
			LoggingWrapper::instance()->error("Interface displays incorrect labsphere luminance, " + QString::fromStdString(ret.errorMsg));
			return;
		}
		ui.label_luminance->setText(QString::number(lumIS, 'f', 3));
	}
}

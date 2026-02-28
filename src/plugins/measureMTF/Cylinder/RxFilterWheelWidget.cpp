#include "RxFilterWheelWidget.h"
#include "MLColorimeterMode.h"
#include <QMessageBox>

using namespace ML::MLColorimeter;
using ML::MLFilterWheel::MLRXFilterConfiguation;
RxFilterWheelWidget::RxFilterWheelWidget(QString toolBoxName, QWidget *parent) : IToolBox(toolBoxName, parent)
{
    ui.setupUi(this);
    initUI();
}
RxFilterWheelWidget::~RxFilterWheelWidget() {}

void RxFilterWheelWidget::initUI()
{
	std::map<int, ML::MLColorimeter::ModuleConfig> cig = MLColorimeterMode::Instance()->GetBinocular()->ML_GetModulesConfig();
	MLRXFilterConfiguation rxFilterConfig;
	for (const auto& pair : cig) {
		if (pair.second.Name.find("Module") != std::string::npos) {
			rxFilterConfig = pair.second.RXFilterConfig;
			break;
		}
	}
	std::vector<std::pair<std::string, int>> nameVec(
		rxFilterConfig.positionName_List.begin(),
		rxFilterConfig.positionName_List.end());

	std::sort(nameVec.begin(), nameVec.end(), Mycmp());
	for (const auto& i : nameVec) {
        if (QString::fromStdString(i.first) == "Clear")
        {
            ui.cyl_box->addItem(QString::fromStdString(i.first));
            ui.rx_cylinder->addItem(QString::fromStdString(i.first));
        }
        else
        {
            ui.cyl_box->addItem(QString::fromStdString(i.first).left(i.first.length() - 1));
            ui.rx_cylinder->addItem(QString::fromStdString(i.first).left(i.first.length() - 1));
        }
	}

	QVector<QStringList> sphericalCSV = MLColorimeterMode::Instance()->GetMonocular()->ML_GetBusinessManageConfig()->GetModuleCSVFile()["RX_Spherical"];

	for (int i = 1; i < sphericalCSV.size(); i++) {
		QString item = sphericalCSV[i][0].left(sphericalCSV[i][0].size() - 1);
		ui.rx_sphere->addItem(item);
	}
	ui.rx_sphere->setCurrentText("0");
	QObject::connect(ui.cyl_box, &QComboBox::currentTextChanged, this, &RxFilterWheelWidget::setCylinder);
	QObject::connect(ui.axis_box, &QComboBox::currentTextChanged, this, &RxFilterWheelWidget::setAxis);

    connect(MLColorimeterMode::Instance(), SIGNAL(connectStatus(bool)), this, SLOT(updateConnectStatus(bool)));
	ui.connectStatus->setText("Not Connected");
}

void RxFilterWheelWidget::on_refresh_btn_clicked()
{
    refresh(RefreshType::Status);
}

void RxFilterWheelWidget::refresh(RefreshType type)
{
    RXCombination rx = MLColorimeterMode::Instance()->GetRX();
    if (type == RefreshType::Status)
    {
        ui.cylinder_status->setText(QString::number(rx.Cylinder));
        ui.axis_status->setText(QString::number(rx.Axis));
        ui.sphere_status->setText(QString::number(rx.Sphere));
    }
    else if (type == RefreshType::Setting)
    {
        ui.cyl_box->setCurrentText(QString::number(rx.Cylinder));
        ui.axis_box->setCurrentText(QString::number(rx.Axis));
        ui.rx_cylinder->setCurrentText(QString::number(rx.Cylinder));
        ui.rx_axis->setCurrentText(QString::number(rx.Axis));
        ui.rx_sphere->setCurrentText(QString::number(rx.Sphere));
    }
}
void RxFilterWheelWidget::on_setrx_btn_clicked()
{
    RXCombination rx;
    rx.Cylinder = ui.rx_cylinder->currentText().toDouble();
    rx.Sphere = ui.rx_sphere->currentText().toDouble();
    rx.Axis = ui.rx_axis->currentText().toInt();
    Result res = MLColorimeterMode::Instance()->SetRXSync(rx);
    if (!res.success)
    {
        QMessageBox::critical(this, "Set RX Error", QString::fromStdString(res.errorMsg));
        return;
    }
    refresh(RefreshType::Status);
}

void RxFilterWheelWidget::setAxis(QString degree)
{
    QString channels = ui.cyl_box->currentText();
    if (channels != "Clear")
        channels += "d";
    Result res = MLColorimeterMode::Instance()->SetCylinder(channels, degree.toInt());
    if (!res.success)
    {
        QMessageBox::critical(this, "Set Axis Error", QString::fromStdString(res.errorMsg));
        return;
    }
    refresh(RefreshType::Status);
}

void RxFilterWheelWidget::setCylinder(QString channels)
{
	if (channels != "Clear")
		channels += "d";
	Result res = MLColorimeterMode::Instance()->SetCylinder(channels, ui.axis_box->currentText().toInt());
	if (!res.success)
	{
		QMessageBox::critical(this, "Set Cylinder Error", QString::fromStdString(res.errorMsg));
        return;
	}
	refresh(RefreshType::Status);
}

void RxFilterWheelWidget::updateConnectStatus(bool connect) {
    if (connect) 
    {
        on_setrx_btn_clicked();
        ui.connectStatus->setText("Connected");
    } else {
        ui.connectStatus->setText("Not Connected");
    }
}

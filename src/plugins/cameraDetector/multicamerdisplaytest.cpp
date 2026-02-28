#include "multicamerdisplaytest.h"

multicamerdisplaytest::multicamerdisplaytest(QString toolboxName, QWidget* parent)
	: IToolBox(toolboxName, parent)
{
	ui.setupUi(this);

	connect(ui.start, &QPushButton::clicked, this, &multicamerdisplaytest::on_startPushButton_clicked);

	model = new CameraModel();
	

	connect(model, &CameraModel::updateCameraStreamSignal, this, &multicamerdisplaytest::display1);
	connect(model, &CameraModel::updateCameraStreamSignal, this, &multicamerdisplaytest::display2);
	connect(model, &CameraModel::updateCameraStreamSignal, this, &multicamerdisplaytest::display3);
}

multicamerdisplaytest::~multicamerdisplaytest()
{}

void multicamerdisplaytest::display1()
{
	ui.label_display1->setPixmap(QPixmap::fromImage(model->GetQImage()));
}

void multicamerdisplaytest::display2()
{
	ui.label_display2->setPixmap(QPixmap::fromImage(model->GetQImage()));
}

void multicamerdisplaytest::display3()
{
	ui.label_display3->setPixmap(QPixmap::fromImage(model->GetQImage()));
}

void multicamerdisplaytest::on_startPushButton_clicked()
{
	model->connect("01RW6");

	model->SubscribeCameraEvent(MLCameraEvent::kFrameReceived);
}
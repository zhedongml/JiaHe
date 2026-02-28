#include "mqttwidget.h"
#include "MqttModel.h"

MqttWidget::MqttWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.LineIP->setIP("127.0.0.1");
	if (MqttModel::getInstance()->isConnected())
	{
		ui.ptnConnect->setEnabled(false);
		ui.ptnDisConnect->setEnabled(true);
		initListWidget();
	}
	else
	{
		ui.ptnConnect->setEnabled(true);
		ui.ptnDisConnect->setEnabled(false);
	}

	ui.listWidget_sub->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.listWidget_sub->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.listWidget_sub->setDragDropMode(QAbstractItemView::NoDragDrop);
	ui.listWidget_sub->setEditTriggers(QAbstractItemView::NoEditTriggers);

	connect(ui.ptnConnect, &QPushButton::clicked, this, &MqttWidget::on_pushButton_connect_clicked);
	connect(ui.ptnDisConnect, &QPushButton::clicked, this, &MqttWidget::on_pushButton_disconnect_clicked);
	connect(ui.ptnPub, &QPushButton::clicked, this, &MqttWidget::on_pushButton_publish_clicked);

	connect(MqttModel::getInstance(), &MqttModel::onSignalMqttStateChanged, this, &MqttWidget::onMqttStateChanged);
	connect(MqttModel::getInstance(), &MqttModel::onSignalMqttMessageArrived, this, &MqttWidget::onMqttMessageArrived);
}

MqttWidget::~MqttWidget()
{
}

void MqttWidget::initListWidget()
{
	// Çå¿ÕÁÐ±í
	ui.listWidget_sub->clear();
	QStringList topics = MqttModel::getInstance()->getSubscribeTopics();
	ui.listWidget_sub->addItems(topics);
}

void MqttWidget::on_pushButton_connect_clicked()
{
	ui.ptnConnect->setEnabled(false);
	ui.ptnDisConnect->setEnabled(false);
	MqttModel::getInstance()->connectMqttServer();
}

void MqttWidget::on_pushButton_disconnect_clicked()
{
	ui.ptnConnect->setEnabled(false);
	ui.ptnDisConnect->setEnabled(false);
	MqttModel::getInstance()->disconnectMqtt();
}

void MqttWidget::on_pushButton_publish_clicked()
{
	MqttModel::getInstance()->processSendMessage(std::string("D:/dsa/test.csv"));
}

void MqttWidget::onMqttStateChanged(int nConnectState)
{
	switch (nConnectState)
	{
	case 0:
	case 1:
		ui.ptnConnect->setEnabled(true);
		ui.ptnDisConnect->setEnabled(false);
		break;
	case 2:
		ui.ptnConnect->setEnabled(false);
		ui.ptnDisConnect->setEnabled(true);
		initListWidget();
		break;
	default:
		break;
	} 
}

void MqttWidget::onMqttMessageArrived(const std::string& topic, const std::string& message)
{
	ui.plainTextrecv->appendPlainText(QString::fromStdString("Topic: " + topic + "\nMessage: " + message + "\n"));
}


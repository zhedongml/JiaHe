#include "MqttConfigAction.h"
#include "mqttwidget.h"

MqttConfigAction::MqttConfigAction(QString id, QString parentId, QObject* parent)
	: Core::IAction(id, parentId, parent)
{
	connect(this, &Core::IAction::triggered, this, &MqttConfigAction::onSlotConfigMqtt);
	setText(tr("Mqtt Config"));

}

MqttConfigAction::~MqttConfigAction()
{
	
}

void MqttConfigAction::onSlotConfigMqtt()
{
	MqttWidget* widget = new MqttWidget();
	widget->setAttribute(Qt::WA_DeleteOnClose);
	widget->setWindowModality(Qt::ApplicationModal);
	widget->exec();
}
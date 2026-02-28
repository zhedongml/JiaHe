#pragma once

#include <Core/IAction.h>

class MqttConfigAction : public Core::IAction
{
	Q_OBJECT
public:
	explicit MqttConfigAction(QString id, QString parentId, QObject* parent = nullptr);
	~MqttConfigAction();

private slots:
	void onSlotConfigMqtt();
};


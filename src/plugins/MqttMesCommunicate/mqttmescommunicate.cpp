#include "mqttmescommunicate.h"
#include "MqttModel.h"
#include "MqttConfigAction.h"
#include <Core/coreconstants.h>
#include "MqttMes4Recipe.h"
#include "behaviortree_cpp_v3/bt_factory.h"

BT_REGISTER_NODES(factory)
{
	RegisterNodes(factory);
}

MqttMesCommunicate::MqttMesCommunicate()
{
}

MqttMesCommunicate::~MqttMesCommunicate()
{
}

bool MqttMesCommunicate::initialize(const QStringList& arguments, QString* errorString)
{
	MqttModel::getInstance();
	MqttMes4Recipe::getInstance();
	return true;
}

void MqttMesCommunicate::extensionsInitialized()
{
	MqttConfigAction* mqttAction = new MqttConfigAction(Core::Constants::TOGGLE_MQTT_CONFIG, Core::Constants::M_TOOLS);
}
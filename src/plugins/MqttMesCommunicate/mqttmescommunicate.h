#pragma once

#include "mqttmescommunicate_global.h"
#include <pluginsystem/iplugin.h>

class MQTTMESCOMMUNICATE_EXPORT MqttMesCommunicate : public ExtensionSystem::IPlugin
{
    Q_OBJECT
	Q_PLUGIN_METADATA(IID "MLPlugins" FILE "MqttMesCommunicate.json")
public:
    explicit MqttMesCommunicate();
    ~MqttMesCommunicate();

    bool initialize(const QStringList& arguments, QString* errorString);
    void extensionsInitialized();
};

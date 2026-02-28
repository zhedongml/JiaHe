#pragma once
#include <pluginsystem/iplugin.h>

class IntegratingSpherePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MLPlugins" FILE "IntegratingSphere.json")

  public:
    IntegratingSpherePlugin();
    ~IntegratingSpherePlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage) override;

    void extensionsInitialized() override;
};

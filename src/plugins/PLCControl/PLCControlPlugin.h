#pragma once

#include "plccontrol_global.h"
#include "pluginsystem/iplugin.h"
#include "PLCController.h"

class PLCCONTROL_EXPORT PLCControlPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MLPlugins" FILE "PLCControl.json")

  public:
    PLCControlPlugin();
    ~PLCControlPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage) override;

    void extensionsInitialized() override;

private:
    PLCController *m_PLCController;
};

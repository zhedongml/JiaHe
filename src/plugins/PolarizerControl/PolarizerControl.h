#pragma once
#include "polarizercontrol_global.h"
#include "../pluginsystem/iplugin.h"
#include <QObject>

class POLARIZERCONTROL_EXPORT PolarizerControl : public ExtensionSystem::IPlugin
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID "MLPlugins" FILE "PolarizerControl.json")
public:
    PolarizerControl();
    ~PolarizerControl();

    bool initialize(const QStringList& arguments, QString* errorMessage) override;

    void extensionsInitialized() override;
};

#pragma once

#include "PrjCommon/service/ml.h"
#include "pluginsystem/pluginspec.h"
#include <pluginsystem/iplugin.h>
#include <plugins/Core/icore.h>
#include "mesandfileload_global.h"

using namespace ExtensionSystem;
using namespace CORE;

class MESANDFILELOAD_EXPORT MesPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MLPlugins" FILE "Mes.json")

public:
    MesPlugin();
    ~MesPlugin(); 

    bool initialize(const QStringList& arguments, QString* errorMessage) override;

    void extensionsInitialized() override;

};


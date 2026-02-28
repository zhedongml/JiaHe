#pragma once

#include "collimatedlighttube_global.h"
#include <QObject>
#include <pluginsystem/iplugin.h>
#include "CollimatedLightTube4Recipe.h"

class COLLIMATEDLIGHTTUBE_EXPORT CollimatedLightTube : public ExtensionSystem::IPlugin
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID "MLPlugins" FILE "CollimatedLightTube.json")
public:
    CollimatedLightTube();
    ~CollimatedLightTube();

    bool initialize(const QStringList& arguments, QString* errorMessage) override;

    void extensionsInitialized() override;
};

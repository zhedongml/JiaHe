#pragma once

#include "decoder_global.h"
#include <pluginsystem/iplugin.h>

class DECODER_EXPORT Decoder : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MLPlugins" FILE "Decoder.json")
public:
    Decoder();
    ~Decoder();

    bool initialize(const QStringList& arguments, QString* errorMessage) override;

    void extensionsInitialized() override;
};

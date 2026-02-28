#pragma once

#include "mtfplugin_global.h"
#include "pluginsystem/iplugin.h"
class MTFMeasureMode;
class MTFPLUGIN_EXPORT MTFPlugin: public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MLPlugins" FILE "MTFPlugin.json")
    public:
        MTFPlugin();
        bool initialize(const QStringList& arguments, QString* errorMessage) override;
        void extensionsInitialized() override;
    private:
        MTFMeasureMode* m_mtfMeasureMode = nullptr;
};

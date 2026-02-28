#pragma once

#include <qglobal.h>
#include "pluginsystem/iplugin.h"

namespace Core {
    class MainWindow;
    class CorePlugin : public ExtensionSystem::IPlugin
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "MLPlugins" FILE "Core.json")
    public:
        CorePlugin();
        ~CorePlugin() override;

        static CorePlugin* instance();

        bool initialize(const QStringList& arguments, QString* errorMessage = nullptr) override;
        void extensionsInitialized() override;
        bool delayedInitialize() override;
        ShutdownFlag aboutToShutdown() override;
    private:
        MainWindow* m_mainWindow = nullptr;
    };
}
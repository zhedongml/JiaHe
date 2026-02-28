#ifndef MINDVERSIONPLUGIN_H
#define MINDVERSIONPLUGIN_H

#include "mlmindvisioncamera_global.h"
#include "MLMindVisionCamera.h"
#include <QPointer>
#include "../pluginsystem/iplugin.h"

using namespace CORE;

namespace MLMindVersionCamera {
    namespace Internal {

        class MindVersionPlugin : public ExtensionSystem::IPlugin
        {
            Q_OBJECT
                Q_PLUGIN_METADATA(IID "MLPlugins" FILE "MLMindVisionCamera.json")

        public:
            MindVersionPlugin();
            ~MindVersionPlugin() override;

            bool initialize(const QStringList& arguments, QString* errorString) override;
            void extensionsInitialized() override;
            ShutdownFlag aboutToShutdown() override;

        private:
            void triggerAction();
            QPointer<MLCamera> cameraService = nullptr;
        };

    } // namespace Internal
} // namespace MLMindVersionCamera

#endif // MINDVERSIONPLUGIN_H

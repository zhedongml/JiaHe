#pragma once

#include "pimotionactor_global.h"
#include <pluginsystem/iplugin.h>
#include "MLPIController1D.h"

namespace PiMotionActor {
    namespace Internal {
        class PIMOTIONACTOR_EXPORT piMotionActorPlugin:public ExtensionSystem::IPlugin
        {
            Q_OBJECT
                Q_PLUGIN_METADATA(IID "MLPlugins" FILE "piMotionActor.json")

        public:
            piMotionActorPlugin();
            ~piMotionActorPlugin();
            bool initialize(const QStringList& arguments, QString* errorString) override;
            void extensionsInitialized() override;
            ShutdownFlag aboutToShutdown() override;

        private:
            void triggerAction();
            CoreMotion* motionService = nullptr;
            CoreMotion *motionService_MV = nullptr;
        };
    }
}
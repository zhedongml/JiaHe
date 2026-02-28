#ifndef CAMERADETECTORPLUGIN_H
#define CAMERADETECTORPLUGIN_H

#include "cameradetector_global.h"

#include <pluginsystem/iplugin.h>
#include "ml_vimba_camera.h"
#include <QPointer>

using namespace CORE;

namespace CameraDetector {
namespace Internal {

class CameraDetectorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MLPlugins" FILE "CameraDetector.json")

public:
    CameraDetectorPlugin();
    ~CameraDetectorPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
    QPointer<MLCamera> cameraService = nullptr;
    QPointer<MLCamera> camera2Service = nullptr;
};

} // namespace Internal
} // namespace CameraDetector

#endif // CAMERADETECTORPLUGIN_H

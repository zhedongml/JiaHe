#pragma once
//#include <QObject>
#include <iplugin.h>
#include "dutmotioncontrol_global.h"
#include "DUTMotionWidget.h"

class DUTMOTIONCONTROL_EXPORT DUTMotionPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MLPlugins" FILE "DUTMotionControl.json")

  public:
    DUTMotionPlugin();
    ~DUTMotionPlugin();

    bool initialize(const QStringList& arguments, QString* errorMessage) override;

    void extensionsInitialized() override;
};

#pragma once
#include "alignment_global.h"
#include "PrjCommon/service/ml.h"
#include "pluginsystem/pluginspec.h"
#include <pluginsystem/iplugin.h>
#include <plugins\Alignment\FiducialConfigWidget.h>
#include <plugins/Core/icore.h>

using namespace ExtensionSystem;
using namespace CORE;

class AlignWidget;
class CalibrateMode;
class FiducialCalibrateWidget;

class AlignPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MLPlugins" FILE "Alignment.json")

  public:
    AlignPlugin();
    ~AlignPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage) override;

    void extensionsInitialized() override;

    CalibrateMode* getCalibrateMode();

  private:
    Core::ICore *m_coreImpl = nullptr;
    CalibrateMode *m_calibrateMode = nullptr;
};

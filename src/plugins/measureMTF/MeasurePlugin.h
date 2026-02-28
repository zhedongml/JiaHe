#pragma once

#include "measuremtf_global.h"
#include <pluginsystem/iplugin.h>

class MeasureMode;
class MEASUREMTF_EXPORT MeasurePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MLPlugins" FILE "Measure.json")
  public:
    MeasurePlugin();
    ~MeasurePlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage) override;

    void extensionsInitialized() override;

  private:
    MeasureMode *m_measureMode = nullptr;
};

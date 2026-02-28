#pragma once

#include "appoutputpane.h"
#include "measuremtf_global.h"
#include <extensionsystem/iplugin.h>
class MTFMeasureMode;
class MEASUREMTF_EXPORT measureMTF : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Measure.json")
  public:
    measureMTF();
    bool initialize(const QStringList &arguments, QString *errorMessage) override;

    void extensionsInitialized() override;

  private:
    MTFMeasureMode *m_mtfMeasureMode = nullptr;
    MeasureMent::Internal::AppOutputPane *appoutput;
};

#pragma once
#include "windows.h"
#include "maalignment_global.h"
#include <pluginsystem/iplugin.h>
#include "dutPrCalibrateWidget.h"
#include "aligmentWidget.h"
#include "SLBWidget.h"

class MAalignmentMode;
class MAALIGNMENT_EXPORT MAalignment : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MLPlugins" FILE "MAalignment.json")
  public:
    MAalignment();
    ~MAalignment();
    bool initialize(const QStringList &arguments, QString *errorMessage) override;

    void extensionsInitialized() override;

  private:
    MAalignmentMode *m_alignmentMode = nullptr;
    aligmentWidget *m_alignWidget = nullptr;
    SLBWidget* m_slbWidget = nullptr;
};
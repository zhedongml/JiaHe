#include "PLCControlPlugin.h"
#include "PLCControlBox.h"

bool PLCControlPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    return true;
}

void PLCControlPlugin::extensionsInitialized()
{
    m_PLCController = PLCController::instance();
    PLCControlBox *PLC = new PLCControlBox("PLC");
    PLC->SetModel(m_PLCController);
}

PLCControlPlugin::PLCControlPlugin()
{
}

PLCControlPlugin::~PLCControlPlugin()
{
}

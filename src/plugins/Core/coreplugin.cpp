#include "coreplugin.h"
#include "pluginsystem/pluginmanager.h"
#include "mainwindow.h"
#include "actionmanager/actionmanager.h"
#include "pluginsystem/pluginerroroverview.h"
#include "LoginForm.h"
#include "pluginsystem/pluginspec.h"

using namespace Core;
static CorePlugin* m_instance = nullptr;
CorePlugin::CorePlugin()
{
    m_instance = this;
}
CorePlugin::~CorePlugin()
{
    delete m_mainWindow;
}
CorePlugin* CorePlugin::instance()
{
    return m_instance;
}
bool CorePlugin::initialize(const QStringList& arguments, QString* errorMessage)
{
    LoginForm* login = new LoginForm();
    if (!login->exec()) {
        this->pluginSpec()->addArgument("Login cancel");
        return false;
    }

    new ActionManager(this);
    m_mainWindow = new MainWindow;
    //m_mainMode = new MainMode;
    //ModeManager::activateMode(m_mainMode->id());


    return true;
}
void CorePlugin::extensionsInitialized()
{
    m_mainWindow->extensionsInitialized();
     if (ExtensionSystem::PluginManager::hasError()) {
         auto errorOverview = new ExtensionSystem::PluginErrorOverview(m_mainWindow);
         errorOverview->setAttribute(Qt::WA_DeleteOnClose);
         errorOverview->setModal(true);
         errorOverview->show();
     }
}
bool CorePlugin::delayedInitialize()
{
    return true;
}
ExtensionSystem::IPlugin::ShutdownFlag CorePlugin::aboutToShutdown()
{
    ExtensionSystem::IPlugin::ShutdownFlag shutdownFlag;
    m_mainWindow->aboutToShutdown();
    return shutdownFlag;
}
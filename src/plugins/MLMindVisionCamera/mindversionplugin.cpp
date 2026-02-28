#include "MLMindVisionCamera.h"

#include <Core/icore.h>
#include <Core/icontext.h>
#include <Core/actionmanager/actionmanager.h>
#include <Core/actionmanager/command.h>
#include <Core/actionmanager/actioncontainer.h>
#include <Core/coreconstants.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <pluginsystem/Services.h>
#include "mindversionplugin.h"

using namespace MLMindVersionCamera::Internal;

MindVersionPlugin::MindVersionPlugin()
{
	// Create your members
}

MindVersionPlugin::~MindVersionPlugin()
{
	// Unregister objects from the plugin manager's object pool
	// Delete members
	if (cameraService != nullptr) {
		delete cameraService;
	}
}

bool MindVersionPlugin::initialize(const QStringList& arguments, QString* errorString)
{
	// Register objects in the plugin manager's object pool
	// Load settings
	// Add actions to menus
	// Connect to other plugins' signals
	// In the initialize function, a plugin can be sure that the plugins it
	// depends on have initialized their members.

	Q_UNUSED(arguments)
		Q_UNUSED(errorString)

		//auto action = new QAction(tr("CameraDetector Action"), this);
		//Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID,
		//                                                         Core::Context(Core::Constants::C_GLOBAL));
		//cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+A")));
		//connect(action, &QAction::triggered, this, &CameraDetectorPlugin::triggerAction);

		//Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
		//menu->menu()->setTitle(tr("CameraDetector"));
		//menu->addAction(cmd);
		//Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    QStringList lst;
	lst.append("CORE::MLCamera");
	cameraService = new MLMindVisionCamera;
	QHash<QString, QVariant> props;
	props.insert("mv", "mindversion");
	ExtensionSystem::Internal::ServicesManger::registerService<MLMindVisionCamera>(this, lst, cameraService, props);
	return true;
}

void MindVersionPlugin::extensionsInitialized()
{
	// Retrieve objects from the plugin manager's object pool
	// In the extensionsInitialized function, a plugin can be sure that all
	// plugins that depend on it are completely initialized.
   // AutoExposureWidget* exposureWidget = new AutoExposureWidget("Set Exposure");


	//multicamerdisplaytest* toolbox1 = new multicamerdisplaytest("MultiDislpayDemo");

}

ExtensionSystem::IPlugin::ShutdownFlag MindVersionPlugin::aboutToShutdown()
{
	// Save settings
	// Disconnect from signals that are not needed during shutdown
	// Hide UI (if you add UI that is not in the main window directly)
	return SynchronousShutdown;
}

void MindVersionPlugin::triggerAction()
{
	QMessageBox::information(Core::ICore::mainWindow(),
		tr("Action Triggered"),
		tr("This is an action from CameraDetector."));
}


#include "pimotionactorplugin.h"
#include <Core/icore.h>
#include <Core/icontext.h>
#include <Core/actionmanager/actionmanager.h>
#include <Core/actionmanager/command.h>
#include <Core/actionmanager/actioncontainer.h>
#include <Core/coreconstants.h>
#include <PrjCommon/recipewrapperregistry.h>
#include <QAction>
#include <QMessageBox>
#include <QMenu>
#include "motion3dcontrolwrapper.h"
#include "Motion2DControlWrapper.h"
#include <pluginsystem/Services.h>
#include "behaviortree_cpp_v3/bt_factory.h"

namespace PiMotionActor {
    namespace Internal {
        piMotionActorPlugin::piMotionActorPlugin()
        {
        }
        piMotionActorPlugin::~piMotionActorPlugin()
        {
        }
        bool piMotionActorPlugin::initialize(const QStringList& arguments, QString* errorString)
        {
            // Register objects in the plugin manager's object pool
            // Load settings
            // Add actions to menus
            // Connect to other plugins' signals
            // In the initialize function, a plugin can be sure that the plugins it
            // depends on have initialized their members.

            Q_UNUSED(arguments)
                Q_UNUSED(errorString)

            //    auto action = new QAction(tr("PiMotionActor Action"), this);
            //Core::Command* cmd = Core::ActionManager::registerAction(action, "PiMotionActor::Action",
            //    Core::Context(Core::Constants::C_GLOBAL));
            //cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+Z")));
            //connect(action, &QAction::triggered, this, &piMotionActorPlugin::triggerAction);

            //Core::ActionContainer* menu = Core::ActionManager::createMenu("PiMotionActor::Menu");
            //menu->menu()->setTitle(tr("PiMotionActor"));
            //menu->addAction(cmd);
            //Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

            QStringList lst;
            lst.append("CoreMotion");
            motionService = new MLPIController1D;
            QHash<QString, QVariant> props;
            props.insert("PI", "PI1D");
            ExtensionSystem::Internal::ServicesManger::registerService<MLPIController1D>(this, lst, motionService, props);

            motionService_MV = new MLPIController1D;
            QHash<QString, QVariant> _props;
            _props.insert("PI", "Pi1D");
            ExtensionSystem::Internal::ServicesManger::registerService<MLPIController1D>(this, lst, motionService_MV,_props);
            
            return true;
        }

        void piMotionActorPlugin::extensionsInitialized()
        {
            // Retrieve objects from the plugin manager's object pool
            // In the extensionsInitialized function, a plugin can be sure that all
            // plugins that depend on it are completely initialized.
            Motion3DControlWrapper* wrapper = new Motion3DControlWrapper(this);
            RecipeWrapperRegistry::Instance()->regist("motion3D", wrapper);

            Motion2DControlWrapper* wrapper2D = new Motion2DControlWrapper(this);
            RecipeWrapperRegistry::Instance()->regist("motion2D", wrapper2D);
        }

        ExtensionSystem::IPlugin::ShutdownFlag piMotionActorPlugin::aboutToShutdown()
        {
            // Save settings
            // Disconnect from signals that are not needed during shutdown
            // Hide UI (if you add UI that is not in the main window directly)
            return SynchronousShutdown;
        }

        void piMotionActorPlugin::triggerAction()
        {
          
        }
    }
}
#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QSplitter>

#include "Core/actionmanager/actioncontainer.h"
#include "Core/actionmanager/actionmanager.h"
#include "Core/actionmanager/command.h"
#include "Core/coreconstants.h"
#include "Core/icontext.h"
#include "Core/icore.h"
#include "Core/imode.h"
#include "Core/modemanager.h"
#include "PrjCommon/recipewrapperregistry.h"

#include "RecipeEngineWidget.h"
#include "recipeengine.h"
#include "recipeengineconstants.h"
#include "reciperesultwidget.h"
//#include "RecipeWidget.h"


namespace RecipeEngine
{
	namespace Internal
	{
		class RecipeMode : public Core::IMode
		{
		public:
			~RecipeMode()
			{
			}

			RecipeMode()
			{
				//RecipeResultWidget* resultWidget = new RecipeResultWidget();
				//setResultWidget(resultWidget);
				DispatchRecipeModule* dispatch = new DispatchRecipeModule();
				QWidget* recipeWidget = new RecipeEngineWidget(dispatch);

				//QWidget* recipeWidget = new RecipeWidget(GraphicMode::EDITOR);
				setWidget(recipeWidget);
				//setLogicWidget(recipeWidget);
				setContext(Core::Context("Recipe.MainView"));
				setDisplayName(tr("Recipe"));
				setIcon(QIcon(":/image/recipe.png"));
				setPriority(1);
				setId("Recipe.RecipeMode");
			}
		};
		RecipeEnginePlugin::RecipeEnginePlugin()
		{
			// Create your members
		}

		RecipeEnginePlugin::~RecipeEnginePlugin()
		{
			// Unregister objects from the plugin manager's object pool
			// Delete members
		}

		bool RecipeEnginePlugin::initialize(const QStringList& arguments, QString* errorString)
		{
			// Register objects in the plugin manager's object pool
			// Load settings
			// Add actions to menus
			// Connect to other plugins' signals
			// In the initialize function, a plugin can be sure that the plugins it
			// depends on have initialized their members.

			Q_UNUSED(arguments)
				Q_UNUSED(errorString)

				//auto action = new QAction(tr("RecipeEngine Action"), this);
				//Core::Command *cmd =
				//    Core::ActionManager::registerAction(action, Constants::ACTION_ID, Core::Context(Core::Constants::C_GLOBAL));
				//cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+R")));
				//connect(action, &QAction::triggered, this, &RecipeEnginePlugin::triggerAction);

				//Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
				//menu->menu()->setTitle(tr("RecipeEngine"));
				//menu->addAction(cmd);
				//Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);
				//m_recipeMode = new RecipeMode();
				return true;
		}

		void RecipeEnginePlugin::extensionsInitialized()
		{
			// Retrieve objects from the plugin manager's object pool
			// In the extensionsInitialized function, a plugin can be sure that all
			// plugins that depend on it are completely initialized.
			const QMap<QString, QWidget*>& resultWidgets = RecipeWrapperRegistry::Instance()->getresultWidgets();
			QWidget* mtfResultWidget = resultWidgets.value("MTF");

			//TODO: May
			return;

			if (mtfResultWidget != nullptr)
			{
				QWidget* resWdiget = (QWidget*)m_recipeMode->widget()->findChild<QWidget*>("dataresult");
				QGridLayout* lyt = (QGridLayout*)resWdiget->layout();
				lyt->addWidget(mtfResultWidget);
				mtfResultWidget->show();
			}
		}

		ExtensionSystem::IPlugin::ShutdownFlag RecipeEnginePlugin::aboutToShutdown()
		{
			// Save settings
			// Disconnect from signals that are not needed during shutdown
			// Hide UI (if you add UI that is not in the main window directly)
			return SynchronousShutdown;
		}

		void RecipeEnginePlugin::triggerAction()
		{
			QMessageBox::information(Core::ICore::mainWindow(), tr("Action Triggered"),
				tr("This is an action from RecipeEngine."));
		}

	} // namespace Internal
} // namespace RecipeEngine

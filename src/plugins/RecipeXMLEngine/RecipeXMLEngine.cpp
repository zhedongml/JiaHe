#include <QtWidgets\qmessagebox.h>
#include <QtCore\qpluginloader.h>
#include <QCoreApplication>

#include "RecipeXMLEngine.h"
#include "Core/actionmanager/actioncontainer.h"
#include "Core/actionmanager/actionmanager.h"
#include "Core/actionmanager/command.h"
#include "Core/coreconstants.h"
#include "Core/icontext.h"
#include "Core/icore.h"
#include "Core/loggingwrapper.h"
#include <regex>

namespace RecipeXMLEngine {
	namespace Internal {

		RecipeXMLEnginePlugin::RecipeXMLEnginePlugin()
		{
		}

		RecipeXMLEngine::Internal::RecipeXMLEnginePlugin::~RecipeXMLEnginePlugin()
		{
		}

		bool RecipeXMLEngine::Internal::RecipeXMLEnginePlugin::initialize(const QStringList& arguments, QString* errorString)
		{
			Q_UNUSED(arguments)
				Q_UNUSED(errorString)
				m_recipeXMLMode = new RecipeXMLMode();
			Core::ModeManager::instance()->activateMode("Recipe.RecipeXMLMode");
				return true;
		}

		void RecipeXMLEngine::Internal::RecipeXMLEnginePlugin::extensionsInitialized()
		{
			QString appPath = QCoreApplication::applicationDirPath();
			m_recipeXMLMode->GetRecipe2Plugin()->LoadCustomNode((appPath + "/plugins/registerRecipe").toStdString());
			
			connect(m_recipeXMLMode->GetRecipe2Plugin(), &Recipe2::SendLogSignal, [=](std::string log) {
				static std::regex start_pattern(R"(=+ Node: (\w+) START=+)");
				static std::regex end_pattern(R"(=+ Node: (\w+) END====TIME: ([0-9.]+)=+)");

				std::smatch match;
				if (std::regex_search(log, match, start_pattern)) {
					std::string nodeName = match[1];
					std::string result = "Recipe Node [ " + nodeName + " ] start run ...";
					LoggingWrapper::instance()->info(QString::fromStdString(result));
				}
				else if (std::regex_search(log, match, end_pattern)) {
					std::string nodeName = match[1];
					std::string timeCost = match[2];
					std::string result = "Recipe Node [ " + nodeName + " ] finished ,take times : " + timeCost + "s.";
					LoggingWrapper::instance()->info(QString::fromStdString(result));
				}
				else {
					LoggingWrapper::instance()->info(QString::fromStdString(log));
				}
				});

		}

		void RecipeXMLEngine::Internal::RecipeXMLEnginePlugin::triggerAction()
		{
			QMessageBox::information(Core::ICore::mainWindow(), tr("Action Triggered"),
				tr("This is an action from RecipeEngine."));
		}
	}
}

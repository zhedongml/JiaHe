#pragma once

#include "recipexmlengine_global.h"
#include "pluginsystem/iplugin.h"
#include "Core/modemanager.h"
#include "Core/imode.h"
#include "Recipe2.h"

namespace RecipeXMLEngine {
    namespace Internal {

		class RecipeXMLMode : public Core::IMode
		{
		public:
			Recipe2* recipePlugin = nullptr;

			RecipeXMLMode()
			{
				recipePlugin = new Recipe2();
				//recipePlugin->LoadCustomNode((appPath + "/plugins/registerRecipe").toStdString());
				setWidget(recipePlugin->GetWidget());
				//setLogicWidget(recipeWidget);
				setContext(Core::Context("Recipe.MainView"));
				setDisplayName(tr("RecipeXML"));
				setIcon(QIcon(":/image/recipe.png"));
				setPriority(0);
				setId("Recipe.RecipeXMLMode");
			}

			Recipe2* GetRecipe2Plugin()
			{
				return recipePlugin;
			}

			~RecipeXMLMode() {
				if (recipePlugin != nullptr) {
					delete recipePlugin;
					recipePlugin = nullptr;
				}
			}
		};

        class RECIPEXMLENGINE_EXPORT RecipeXMLEnginePlugin : public ExtensionSystem::IPlugin
        {
            Q_OBJECT
                Q_PLUGIN_METADATA(IID "MLPlugins" FILE "RecipeXMLEngine.json")

        public:
            RecipeXMLEnginePlugin();
            ~RecipeXMLEnginePlugin() override;

            bool initialize(const QStringList& arguments, QString* errorString) override;
            void extensionsInitialized() override;

        private:
            void triggerAction();
            RecipeXMLMode* m_recipeXMLMode = nullptr;
        };
    } 
} 

#pragma once

#include "recipeengine_global.h"
#include "pluginsystem/iplugin.h"
#include "recipeengine_global.h"

namespace RecipeEngine {
namespace Internal {
class RecipeMode;
class RECIPEENGINE_EXPORT RecipeEnginePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "MLPlugins" FILE "RecipeEngine.json")

public:
    RecipeEnginePlugin();
    ~RecipeEnginePlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
    RecipeMode* m_recipeMode = nullptr;
};

} // namespace Internal
} // namespace RecipeEngine

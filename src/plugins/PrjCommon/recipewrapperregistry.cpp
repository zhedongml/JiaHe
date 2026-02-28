#include "recipewrapperregistry.h"
#include "qmutex.h"
RecipeWrapperRegistry* RecipeWrapperRegistry::self = 0;
RecipeWrapperRegistry* RecipeWrapperRegistry::Instance()
{
    if (!self) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new RecipeWrapperRegistry;
        }
    }

    return self;
}

void RecipeWrapperRegistry::regist(QString wrapperName,IRecipeWrapper* wrapper)
{
    repository.insert(wrapperName, wrapper);
}

void RecipeWrapperRegistry::registoryResultWidget(QString name, QWidget* resultWidget)
{
    resultWidgetRepository.insert(name, resultWidget);
}

IRecipeWrapper* RecipeWrapperRegistry::getWrapper(QString wrapperName)
{
    if (repository.contains(wrapperName))
    {
        return repository.value(wrapperName);
    }
    else
        return nullptr;
}

void RecipeWrapperRegistry::notifyStopStatus(bool isstop)
{
    for (auto each : repository.toStdMap()) {
        each.second->notifyStop(isstop);
    }
}

void RecipeWrapperRegistry::notifyPauseStatus(bool issPause)
{
    for (auto each : repository.toStdMap()) {
        each.second->notifyPause(issPause);
    }
}

QMap<QString, QWidget*> RecipeWrapperRegistry::getresultWidgets()
{
    return resultWidgetRepository;
}

RecipeWrapperRegistry::RecipeWrapperRegistry()
{

}

RecipeWrapperRegistry::~RecipeWrapperRegistry()
{
    for (auto each : repository.toStdMap()) {
        delete each.second;
    };
    for (auto each : resultWidgetRepository.toStdMap()) {
        delete each.second;
    };
}

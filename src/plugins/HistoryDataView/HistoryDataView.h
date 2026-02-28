#pragma once

#include "historydataview_global.h"
#include <pluginsystem/iplugin.h>
#include "HistoryImageWidget.h"

class HISTORYDATAVIEW_EXPORT HistoryDataView : public ExtensionSystem::IPlugin
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID "MLPlugins" FILE "HistoryData.json")
public:
    HistoryDataView();
    ~HistoryDataView();
    bool initialize(const QStringList& arguments, QString* errorMessage) override;
    void extensionsInitialized() override;

private slots:
    void ImageTriggerAction(QString path);
    

private:
    HistoryImageWidget *m_historyImageWidget = nullptr;
};

#include "HistoryDataView.h"
#include "HistoryDataWidget.h"
#include "Core\outputpane.h"
#include "pluginsystem\Services.h"
#include <Core\actionmanager\actioncontainer.h>
#include <Core\actionmanager\actionmanager.h>
#include <Core\coreconstants.h>
#include <Core\icore.h>
#include <Core\imode.h>
#include <Core\modemanager.h>
#include <PrjCommon\PrjCommon.h>
#include <QGuiApplication>
#include <QScreen>

HistoryDataView::HistoryDataView()
{
}

HistoryDataView::~HistoryDataView()
{
    //if (m_historyDataMode != nullptr)
    //    delete m_historyDataMode;
}

bool HistoryDataView::initialize(const QStringList& arguments, QString* errorMessage)
{
    Core::Context context("HistoryDataView.MainView");

    connect(Core::PrjCommon::instance(), &Core::PrjCommon::historyimageShow, this, &HistoryDataView::ImageTriggerAction);
    //connect(Core::PrjCommon::instance(), &Core::PrjCommon::historyimageUpdate, [=]() {
    //    if (m_historyImageWidget)
    //    {
    //        emit m_historyImageWidget->directoryChanged();
    //    }});

    
    connect(Core::ICore::instance(), &Core::ICore::coreAboutToClose, [=]() {
        if (m_historyImageWidget)
            m_historyImageWidget->close();
    });
    return true;
}

void HistoryDataView::extensionsInitialized()
{
}

void HistoryDataView::ImageTriggerAction(QString path)
{
    if (!m_historyImageWidget)
    {
        m_historyImageWidget = new HistoryImageWidget;
    }
    m_historyImageWidget->setVisible(true);
    m_historyImageWidget->activateWindow();
    m_historyImageWidget->setWindowTitle("HistoryImage");
    m_historyImageWidget->initSelectInfo();
    m_historyImageWidget->init();
    m_historyImageWidget->handleImageData(path);

    HistoryImageWidget *window = m_historyImageWidget;
    QList<QScreen*> screens = QGuiApplication::screens();
    if (screens.size() >= 2) {
        QScreen* secondScreen = screens.at(1); 
        QRect screenGeometry = secondScreen->availableGeometry();

        int width = screenGeometry.width() / 1;
        int height = screenGeometry.height() / 1;
        window->resize(width, height);

        int x = screenGeometry.x() + (screenGeometry.width() - width) / 2;
        int y = screenGeometry.y() + (screenGeometry.height() - height) / 2;
        window->move(x, y);
    }
    else {
        QScreen* primaryScreen = QGuiApplication::primaryScreen();
        QRect screenGeometry = primaryScreen->availableGeometry();
        int width = screenGeometry.width() / 1;
        int height = screenGeometry.height() / 1;
        window->resize(width, height);
        window->move(screenGeometry.center() - QPoint(width / 2, height / 2));
    }
}

#include "MesConfigAction.h"
#include <QApplication>
#include <QDesktopWidget>
#include "Core/icore.h"
#include <QGuiApplication>
#include <QScreen>
#include "Core/coreconstants.h"
#include "PrjCommon/PrjCommon.h"
#include "PrjCommon/logindata.h"

MesConfigAction::MesConfigAction(QString id, QObject* parent) :
	IAction(id, Constants::M_TOOLS, Core::Constants::G_DEFAULT_FOUR, parent)
{
    setText("MES Config");
    connect(this, SIGNAL(triggered()), this, SLOT(triggeredSlot()));

    connect(Core::ICore::instance(), &Core::ICore::coreAboutToClose, [=]() {
        if (m_widget)
            m_widget->close();
        });

    connect(Core::PrjCommon::instance(), SIGNAL(updateUserLevel()), this, SLOT(updateUserLevel()));
    updateUserLevel();
}

MesConfigAction::~MesConfigAction()
{
}

void MesConfigAction::triggeredSlot()
{
    if (m_widget != nullptr)
    {
        delete m_widget;
        m_widget = nullptr;
    }

    m_widget = new QtWidgetsClass();
    {
        QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();

        int width = screenGeometry.width() / 2;
        int height = screenGeometry.height() / 1.3;
        m_widget->resize(width, height);

        int x = screenGeometry.x() + (screenGeometry.width() - width) / 2;
        int y = screenGeometry.y() + (screenGeometry.height() - height) / 2;
        m_widget->move(x, y);
    }

    m_widget->setVisible(true);
    m_widget->activateWindow();
}

void MesConfigAction::updateUserLevel()
{
}

#include "DutConfigAction.h"
#include <QApplication>
#include <QDesktopWidget>
#include "Core/icore.h"

DutConfigAction::DutConfigAction(QString id, QString parentId, QObject *parent):
	IAction(id, parentId, parent)
{
	setText("DUT Information Config");
	connect(this, SIGNAL(triggered()), this, SLOT(triggeredSlot()));

    connect(Core::ICore::instance(), &Core::ICore::coreAboutToClose, [=]() {
        if (m_dutConfigWidget)
            m_dutConfigWidget->close();
        });
}

DutConfigAction::~DutConfigAction()
{
    if(m_dutConfigWidget){
        delete m_dutConfigWidget;
        m_dutConfigWidget = nullptr;
    }
}

void DutConfigAction::triggeredSlot()
{
    if (m_dutConfigWidget != nullptr)
    {
        delete m_dutConfigWidget;
        m_dutConfigWidget = nullptr;
    }
    m_dutConfigWidget = new FiducialConfigWidget();
    m_dutConfigWidget->setVisible(true);
    m_dutConfigWidget->activateWindow();
    QDesktopWidget* desktop = QApplication::desktop();
    m_dutConfigWidget->move((desktop->width() - m_dutConfigWidget->width()) / 2,
        (desktop->height() - m_dutConfigWidget->height()) / 2);

    m_dutConfigWidget->setVisible(true);
    m_dutConfigWidget->activateWindow();
}
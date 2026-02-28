#include "InitCalibrateCfgAction.h"
#include <QApplication>
#include <QDesktopWidget>
#include "Core/icore.h"

InitCalibrateCfgAction::InitCalibrateCfgAction(QString id, QString parentId, QObject* parent) :
    IAction(id, parentId, parent)
{
    setText("Initial Calibrate Config");
    connect(this, SIGNAL(triggered()), this, SLOT(triggeredSlot()));

    connect(Core::ICore::instance(), &Core::ICore::coreAboutToClose, [=]() {
        if (m_calibrateWidget)
            m_calibrateWidget->close();
        });
}

InitCalibrateCfgAction::~InitCalibrateCfgAction()
{
    if (m_calibrateWidget) {
        delete m_calibrateWidget;
        m_calibrateWidget = nullptr;
    }
}

void InitCalibrateCfgAction::triggeredSlot()
{
    if (m_calibrateWidget != nullptr)
    {
        delete m_calibrateWidget;
        m_calibrateWidget = nullptr;
    }
    m_calibrateWidget = new dutPrCalibrateWidget;
    m_calibrateWidget->setVisible(true);
    m_calibrateWidget->activateWindow();
    m_calibrateWidget->setWindowTitle("Dut And Projector Collide Calibrate");
}
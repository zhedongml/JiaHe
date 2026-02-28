#include "CamereAction.h"
#include <QApplication>
#include <QDesktopWidget>
#include "Core/icore.h"
#include <QGuiApplication>
#include <QScreen>

CamereAction::CamereAction(QString id, QString parentId, QObject *parent):
	IAction(id, parentId, parent)
{
	setText("WebCamera");
	connect(this, SIGNAL(triggered()), this, SLOT(triggeredSlot()));

    connect(Core::ICore::instance(), &Core::ICore::coreAboutToClose, [=]() {
        if (m_camera)
            m_camera->close();
        });
}

CamereAction::~CamereAction()
{
    if(m_camera){
        delete m_camera;
        m_camera = nullptr;
    }
}

void CamereAction::triggeredSlot()
{
    if (m_camera != nullptr)
    {
        delete m_camera;
        m_camera = nullptr;
    }
    m_camera = new Camera();

    {
        QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();

        int width = screenGeometry.width() / 2;
        int height = screenGeometry.height() / 1.3;
        m_camera->resize(width, height);

        int x = screenGeometry.x() + (screenGeometry.width() - width) / 2;
        int y = screenGeometry.y() + (screenGeometry.height() - height) / 2;
        m_camera->move(x, y);
    }

    m_camera->setVisible(true);
    m_camera->activateWindow();
}
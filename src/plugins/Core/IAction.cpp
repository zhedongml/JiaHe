#include "IAction.h"
#include "mainwindow.h"
#include "icore.h"

namespace Core {
	IAction::IAction(QString id, QString parentId, QObject* parent):
		QAction(parent)
	{
		Core::MainWindow* m_mainWindow = (Core::MainWindow*)ICore::mainWindow();
		m_mainWindow->addMenuAction(this, id, parentId);
	}

	IAction::IAction(QString id, QString parentId, QString groupId, QObject* parent) :
		QAction(parent)
	{
		Core::MainWindow* m_mainWindow = (Core::MainWindow*)ICore::mainWindow();
		m_mainWindow->addMenuAction(this, id, parentId, groupId);
	}

	IAction::~IAction()
	{

	}

}

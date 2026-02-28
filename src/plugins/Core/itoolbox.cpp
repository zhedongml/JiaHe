#include "itoolbox.h"
#include "mainwindow.h"
#include "icore.h"
namespace Core {
	IToolBox::IToolBox(QString objName,QObject* parent)
	{
		Core::MainWindow* m_mainWindow = (Core::MainWindow*)ICore::mainWindow();
		if (objName != "") {
			m_mainWindow->addToolBox(this,objName);
		}
		else {
			m_mainWindow->addToolBox(this);
		}
	}

	IToolBox::~IToolBox()
	{
	}
}
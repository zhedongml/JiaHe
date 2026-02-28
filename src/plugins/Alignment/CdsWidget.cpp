#include "CdsWidget.h"

CdsWidget::CdsWidget(QString toolboxName, QWidget *parent) : IToolBox(toolboxName, parent)
{
	ui.setupUi(this);

	ui.widget_down->setType(KEYENCE_TYPE_DOWN);
	ui.widget_down->setTitle("Confocal Distance Sensor");

	ui.widget_up->hide();
	//ui.widget_down->setType(KEYENCE_TYPE_DOWN);
	//ui.widget_down->setTitle("Confocal Distance Sensor - Below");
}

CdsWidget::~CdsWidget()
{}

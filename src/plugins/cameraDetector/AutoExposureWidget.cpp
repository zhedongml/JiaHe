#include "AutoExposureWidget.h"

AutoExposureWidget::AutoExposureWidget(QString toolBoxName,QWidget *parent)
	: IToolBox(toolBoxName,parent)
{
	ui.setupUi(this);
}

AutoExposureWidget::~AutoExposureWidget()
{}

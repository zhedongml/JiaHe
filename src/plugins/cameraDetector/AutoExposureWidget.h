#pragma once

#include "Core/itoolbox.h"
#include "ui_AutoExposureWidget.h"

class AutoExposureWidget : public Core::IToolBox
{
	Q_OBJECT

public:
	AutoExposureWidget(QString toolboxName ="",QWidget * parent = nullptr);
	~AutoExposureWidget();

private:
	Ui::AutoExposureWidgetClass ui;
};

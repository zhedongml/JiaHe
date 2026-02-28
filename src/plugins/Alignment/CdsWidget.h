#pragma once

#include <QWidget>
#include "ui_CdsWidget.h"
#include "Core/itoolbox.h"

class CdsWidget : public Core::IToolBox
{
	Q_OBJECT

public:
	CdsWidget(QString toolboxName, QWidget *parent = nullptr);
	~CdsWidget();

private:
	Ui::CdsWidgetClass ui;
};

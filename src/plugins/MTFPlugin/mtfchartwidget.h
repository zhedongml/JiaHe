#pragma once

#include <QWidget>
#include "ui_mtfchartwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MTFChartWidgetClass; };
QT_END_NAMESPACE

class MTFChartWidget : public QWidget
{
	Q_OBJECT

public:
	MTFChartWidget(QWidget *parent = nullptr);
	~MTFChartWidget();

private:
	Ui::MTFChartWidgetClass *ui;
};

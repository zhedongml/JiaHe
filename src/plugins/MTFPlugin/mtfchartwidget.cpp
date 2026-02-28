#include "mtfchartwidget.h"

MTFChartWidget::MTFChartWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::MTFChartWidgetClass())
{
	ui->setupUi(this);
}

MTFChartWidget::~MTFChartWidget()
{
	delete ui;
}

#include "mtfsettingwidget.h"

MTFSettingWidget::MTFSettingWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::MTFSettingWidgetClass())
{
	ui->setupUi(this);
}

MTFSettingWidget::~MTFSettingWidget()
{
	delete ui;
}

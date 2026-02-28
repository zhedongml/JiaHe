#include "iqmetricwidget.h"

IQMetricWidget::IQMetricWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setFixedHeight(40);
}

IQMetricWidget::~IQMetricWidget()
{}

void IQMetricWidget::setMetricName(QString name)
{
	ui.metricname->setText(name);
}

void IQMetricWidget::setMetricUnit(QString unit)
{
	ui.metricunit->setText(unit);
}

void IQMetricWidget::setMetricValue(QString val)
{
	ui.metricvalue->setText(val);
}

void IQMetricWidget::addSuffixItem(QString val)
{
	ui.suffixcbx->addItem(val);
}
void IQMetricWidget::on_suffixcbx_currentTextChanged(const QString& text) {
	emit suffixTxtChanged(text);
}
QString IQMetricWidget::getSuffix()
{
	return ui.suffixcbx->currentText().trimmed();
}

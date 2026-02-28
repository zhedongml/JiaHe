#pragma once

#include <QWidget>
#include "ui_iqmetricwidget.h"

class IQMetricWidget : public QWidget
{
	Q_OBJECT

public:
	IQMetricWidget(QWidget *parent = nullptr);
	~IQMetricWidget();
	void setMetricName(QString name);
	void setMetricUnit(QString unit);
	void setMetricValue(QString val);
	void addSuffixItem(QString index);
	QString getSuffix();
private:
	Ui::IQMetricWidgetClass ui;
signals:
	void suffixTxtChanged(QString text);
private slots:
	void on_suffixcbx_currentTextChanged(const QString& text);
};

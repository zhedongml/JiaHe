#pragma once

#include <QWidget>
#include "ui_mtfresultwidget.h"
#include "MetricInfo.h"
class MTFResultWidget : public QWidget
{
	Q_OBJECT

public:
	static MTFResultWidget* getInstance(QWidget* parent = nullptr);
	void updateTableWidget(QVector<MetricItem>& rowData);
private:
	Ui::MTFResultWidgetClass ui;
	int aoiCount = 14;
	int maxRowCount = 15;
	QString dir = "V";
	QStringList mtf_h_headers;
	static MTFResultWidget* self;
	MTFResultWidget(QWidget* parent = nullptr);
	~MTFResultWidget();
};

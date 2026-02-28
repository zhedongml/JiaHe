#include "mtfresultwidget.h"
#include "mtfmodelconfig.h"
#include "PrjCommon/metricsdata.h"
MTFResultWidget* MTFResultWidget::self = nullptr;
MTFResultWidget::MTFResultWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	mtf_h_headers.clear();
	mtf_h_headers << "Serial Number" << "Test Time" << "Test Result";
	std::string freq = MTFModelConfig::instance()->getDisplayResultFreq();
	QString freqQStr = QString::fromStdString(freq);
	for (int i = 0; i < aoiCount; i++) {
		QString field = dir + "MTF" + QString::number(i) + "/" + freqQStr;
		mtf_h_headers.append(field);
	}
	ui.mtftablewidget->setColumnCount(mtf_h_headers.size());
	 ui.mtftablewidget->setHorizontalHeaderLabels(mtf_h_headers);
}

MTFResultWidget::~MTFResultWidget()
{
}

MTFResultWidget* MTFResultWidget::getInstance(QWidget* parent)
{
	if (self == nullptr) {
		self = new MTFResultWidget(parent);
	}
	return self;
}

void MTFResultWidget::updateTableWidget(QVector<MetricItem>& rowData)
{
	if (rowData.size() == 0)
		return;
	QTableWidget* tbl = ui.mtftablewidget;
	if (tbl->rowCount() >= 10) {
		tbl->removeRow(0);
	}
	int idx = tbl->rowCount();
	tbl->insertRow(idx);
	bool isPass = true;
	for (int i = 3; i < mtf_h_headers.size(); i++) {
		QString field = mtf_h_headers.at(i);
		bool isNull = true;
		for each (MetricItem data in rowData)
		{
			if (data.metricName.toLower().trimmed() == field.toLower().trimmed()) {
				isNull = false;
				double val = data.value;
				if (val > data.upLimit || val < data.lowLimit) {
					isPass = false;
				}
				QTableWidgetItem* xItem = new QTableWidgetItem(QString::number(val,'f',3));
				tbl->setItem(idx, i, xItem);
			}
		}	
	}
	if (isPass) {
		QTableWidgetItem* xItem = new QTableWidgetItem("Pass");
		tbl->setItem(idx, 2, xItem);
	}
	else {
		QTableWidgetItem* xItem = new QTableWidgetItem("Fail");
		tbl->setItem(idx, 2, xItem);
	}
	QTableWidgetItem* xItem = new QTableWidgetItem(MetricsData::instance()->getDutBarCode());
	tbl->setItem(idx, 0, xItem);

	QDateTime endTime = QDateTime::currentDateTime();
	qint64 endStamp = endTime.toTime_t();
	qint64 startStamp = MetricsData::instance()->getMTFStartTime();
	QString rangeStr = tr("%1s").arg(endStamp - startStamp);
	QTableWidgetItem* timeStr = new QTableWidgetItem(rangeStr);
	tbl->setItem(idx, 1, timeStr);
}

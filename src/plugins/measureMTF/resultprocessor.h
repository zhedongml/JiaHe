#pragma once

#include <QObject>
#include <QVector>
#include "MetricInfo.h"
class ResultProcessor  : public QObject
{
	Q_OBJECT

public:
	ResultProcessor(QObject *parent);
	ResultProcessor();
	~ResultProcessor();
	void saveTOCVSData(QString filePath,QString dutId = "");
	void addMetricItem(MetricItem item);
	void clearMetricVector();
	void setVid(float val);
	void getMTFData(QStringList& mtfData);
private:
	QVector<MetricItem> metricVec;
	void genMTFCsvHeaders();
	QString genMTFUpLimitRow();
	QString genMTFLowLimitRow();
	QString genMTFItemRow();
	int mtfAOINum = 14;
	QStringList mtfHeadersList;
	float vid = 0;
};

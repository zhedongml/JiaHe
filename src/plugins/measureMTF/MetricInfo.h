#pragma once
#include <QString>
struct MetricItem {
	QString metricName = "";
	double upLimit = 0.0;
	double lowLimit = 0.0;
	double value = 0.0;
};
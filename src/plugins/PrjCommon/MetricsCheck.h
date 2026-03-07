#pragma once
#include <QList>
#include "Result.h"
#include "opencv2/opencv.hpp"
#include <QMap>
#include "Result.h"
#include "metricsdata.h"
#include "prjcommon_global.h"

struct McMetricsLimit{
	QString name;
	float up;
	float low;
};

struct McMetricsCheckInfo {
	bool checkEnabled = false;
	QString modelName;
};

class PRJCOMMON_EXPORT MetricsCheck
{
public:
	static MetricsCheck& instance();

	bool limitUpdate();
	void setHeaderList(QStringList headerList, QString eyeboxQueue);
	QStringList getHeaderList();
	bool existHeaderList(QString eyeboxQueue);

	Result setModelName(QString modelName, bool liveRefresh);
	void setCheckEnabled(bool enabled);
	QString getModelName();

	Result setModelName_autoDP(QString modelName, bool liveRefresh);
	void setCheckEnabled_autoDP(bool enabled);
	QString getModelName_autoDP();

	Result getLimitList(const QString &modelName, const QStringList &headerList, QStringList &upList, QStringList& lowList);
	Result getMetricsResult(const QString& modelName, const QStringList& headerList, QStringList& valueList, McMetricsResult &mcResult);

	Result getLimitListAutoDP(const QString& modelName, const QStringList& headerList, QStringList& upList, QStringList& lowList);
	Result getMetricsResultAutoDP(const QString& modelName, const QStringList& headerList, QStringList& valueList, McMetricsResult& mcResult);

private:
	Result readLimit(QString modelName, QMap<QString, McMetricsLimit>& limits);

	Result checkMetrics(const QMap<QString, McMetricsLimit>& limits, const QList<QString>& metricsNames, const QMap<QString, float>& metricsMap, QMap<QString, int>& checkResults);
	Result writeResult(const QList<QString>& metricsNames, QMap<QString, int>& checkResults);

	Result readCsv(QString filePath, QList<QStringList>& data);
	Result writeCsv(QString filePath, QList<QStringList>& data);

	QString getFileName(const QString& modelName);
	Result getKey(const QString &metricsName, QString &key);

private:
	// modelName, header, limit
	QMap<QString, QMap<QString, McMetricsLimit>> m_limitsMap;

	const QString FILE_DIR = "./config/metricsLimit/";
	const float INVALID_VALUE = 99999999;
	const float NULL_VALUE = -99999999;

	bool m_limitUpdate = true;

	QStringList m_headerListCurrent;
	// eyeboxQueue, headerList
	QMap<QString, QStringList> m_headerMap;

	McMetricsCheckInfo m_checkInfo;
	McMetricsCheckInfo m_checkInfo_autoDP;

	const int DECIMAL_NUMBER = 3;
	const QString TEST_RESULT = "TestResult";
	const QString TEST_FAIL_ITEM = "TestFailItem";

};


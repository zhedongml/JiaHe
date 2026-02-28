#pragma once
#include "Result.h"
#include <QString>
#include <QStringList>
#include <QMap>
class GenCSVHeader
{

public:
	static Result writeCsvHeadersToDB();
	static void writeCsvHeadersToDBAll(int n);
	static bool crateMetricsTable(QStringList eyeboxList);
	static void writeHeadersToHistoryDB();
	static bool createMetricsHistoryTable(QString dutId);
	static bool createMetricsHistoryPathTable(QString dutId,QString path);
	static void SetMetricsHistoryTableTime(QString time);
	static bool createMTFTable(QString dutId);
	static void createMTFHeaders(QStringList& mtfTableHeadersList);

	static QList<QString> getMetricsEyeboxIds();

  private:
	static QStringList headersList;
};


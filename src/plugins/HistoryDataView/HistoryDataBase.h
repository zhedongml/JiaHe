#ifndef HISTORY_DATABASE_H
#define HISTORY_DATABASE_H
#include <QObject>
#include <QSqlDatabase>
#include <QStringList>
#include <QMutex>
#include "define.h"

class HistoryDataBase : public QObject
{
	Q_OBJECT
public:
	static HistoryDataBase* getInstance(QObject* parent = nullptr);
	~HistoryDataBase();
	void queryHistoryTableName(DutInfoShowArray& historyDutSNArray);

    void loadHistoryImgPathTableName(DutInfoShowArray &historyDutSNArray);

	void queryTableFormat(IQTableFormatArray* iqTableFormats);

	void DelTimeOutDBData(QString currentTime);

	void DelDiffFormatTable(StringArray tableNameArray);

	GDataMap UpdateImgPathDirInfo(QString dutSn, QString eyebox,QString startTime, QString endTime);

	QStringList getIQMerticsImage(QString dutSn);

	GDataMap UpdateMetricTableShowInfo(QString startTime, QString endTime, QString dutSn, QString eyebox);

	GDataMap UpdateMTFTableShowInfo(QString startTime, QString endTime, QString dutSn, QString eyebox, QString color, QStringList& mtfHeadList);

  private:
	HistoryDataBase(const QString& path, QObject* parent = nullptr);
	bool openDatabase();
    bool closeDatabase();
private:
	static QMutex mutex;
	static HistoryDataBase* self;
	QSqlDatabase m_db;
	bool isAlreadyOpened = false;
};
#endif


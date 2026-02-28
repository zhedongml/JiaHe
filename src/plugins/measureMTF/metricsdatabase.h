#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include "Result.h"

#define DB_SLB_CR "SLB_CR"
#define DB_SLB_CR_ID "ID"
#define DB_SLB_CR_COLOR "COLOR"
#define DB_SLB_CR_TYPE "TYPE"
#define DB_SLB_CR_DATA "DATA"
#define DB_SLB_CR_UPDATE_TIME "UPDATETIME"

class MetricsDataBase  : public QObject
{
	Q_OBJECT

public:
	~MetricsDataBase();
	void beginTransaction();
	void endTransaction();
	static MetricsDataBase* getInstance(QObject* parent = nullptr);
    void updateMetricValue(QString name, double value, QString id = "");
    void updatePathValue(QString dutID,QString path, QString id = "");
	void updateMetricHistoryValue(QString dutID, QString name, double value, QString id = "");
	void insertMTFHistoryValue(QString dutSN, QStringList* mtfData);
    std::vector<QStringList> getCompareMetricKeyValue();
    double queryOneMetricValue(QString n, QString name, QString fieldName);
	double getMetricValue(QString name);
	void delIQMetricDatas();
    void delIQMetricDatas(int n);
    void delTestInfoDatas();

	void insertMetricsItem(QString name, QString unit = "");
    void insertMetricsItem(QString name, float uplimit, float lowlimit, QString eyeboxid, QString unit = "");
 
    QStringList exportToCsv();
    QStringList getAllMetricsToCsv(QString field, const QList<QString> &eyeboxIds = QList<QString>());
    QStringList getAllMetricsToCsv(QString field, QString eyeboxQueue);
	bool createMetricsTable();
    bool createMultiMetricsTable(int metricTableNum);
    bool createMultiMetricsTable(const QList<QString> &metricTableIds);
	bool createMultiMetricsHistoryTable(QString dutId, QStringList* headersList);
	bool createIQMetricsDirectoryHistoryTable(QString dutId, QStringList* headersList);
	bool createMTFHistoryTable(QString dutId, QStringList* headList);
	bool closeDatabase();
	QStringList getTestInfoHeaders();
	QStringList getTestInfoValues();

    Result insertRecord(const QString &table, const QMap<QString, QVariant> record);
    Result updateRecord(const QString &table, const QMap<QString, QVariant> record,
                        const QMap<QString, QVariant> condition);
    Result queryRecord(QList<QMap<QString, QVariant>> &records, const QString &table,
                       const QMap<QString, QVariant> condition, const QList<QString> fields = QList<QString>());
    Result deleteSurplusRecord(const QString &table, const QMap<QString, QVariant> condition, int residueNumber);



	QStringList getOneFieldValues(QString fieldName, QString tableName);
	bool updateOneFieldValue(QString fieldName, QString tableName, QString value);

	bool updateWPCFieldValue(double value, int levelid, QString color, QString xyz = "");
	bool updateWPCFiledValueByFilter(double value, int levelid, QString filter, QString color="");
	QStringList getWPCInfoHeaders();
	QStringList getWPCInfoValues();
	void SetMetricsHistoryTableTime(QString time);

	QMap<QString, QString> getIQTestResult(int num);
    QMap<QString, QString> getIQTestResult(QString eyeboxQueue);
  private:
	MetricsDataBase(const QString& path, QObject* parent = nullptr);
	bool openDatabase();
    Result createSlbCrTable();

	bool isAlreadyOpened = false;
	QString historyTableTime = "";
	QString dutIdStr = "";
private:
	QSqlDatabase m_db;
    std::vector<QStringList> keyinfoVec;
	static QMutex mutex;
	static MetricsDataBase* self;
    bool alreadyCreate = false;
	QMutex m_mutex;
};

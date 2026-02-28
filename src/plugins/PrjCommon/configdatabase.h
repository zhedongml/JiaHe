#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include "prjcommon_global.h"
class QSqlDatabase;
class PRJCOMMON_EXPORT ConfigDataBase : public QObject
{
	Q_OBJECT

public:
	~ConfigDataBase();
	static ConfigDataBase* getInstance(QObject* parent = nullptr);
	void beginTransaction();
	void endTransaction();

	bool updateRecipeFile(const QString& fileName);
	QString readRecipeFile();

	QString getRecipeFileName();
private:
	ConfigDataBase(const QString& path, QObject* parent = nullptr);
	bool openDatabase();
	bool createTables();
private:
	QSqlDatabase m_db;
	static QMutex mutex;
	static ConfigDataBase* self;
};

#pragma once
#include <string>
#include <unordered_map>
#include <QMap>
#include <QDate>
#include <string>
#include "prjcommon_global.h"

class PRJCOMMON_EXPORT Session
{
 public: 
	static Session* getInstance();
    QString get(const QString & key);
	void save(const QString& key, const QString& value);
    void  loadReservedWords();
	QMap<QString, QString> getData();
	~Session();

	void setLastKey(const QString& key);
	QString getLastKey();

private:
	Session() {};
	static Session* s_Instance;

    QMap<QString, QString(Session::*)(void)> m_ReservedWords;
	QMap<QString, QString> m_CustomVariables;
    inline  QString getDate();
	inline  QString getDateTime();

	QString m_lastKey;
};


#include "HistoryDataBase.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QsqlField>
#include <QSet>
#include <QDebug>
#include <LogPlus.h>

QMutex HistoryDataBase::mutex;
HistoryDataBase* HistoryDataBase::self = nullptr;
HistoryDataBase::HistoryDataBase(const QString& path, QObject* parent)
	: QObject(parent)
{
	m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), "historyconfigdb");
	m_db.setDatabaseName(path);
    m_db.close();
}

HistoryDataBase::~HistoryDataBase()
{

}

HistoryDataBase* HistoryDataBase::getInstance(QObject* parent)
{
	if (!self) {
		QMutexLocker locker(&mutex);
		if (!self) {
			self = new HistoryDataBase("./config/mainconfig.db", parent);

		}
	}
	self->setParent(parent);
	return self;
}

bool HistoryDataBase::openDatabase()
{
	if (!isAlreadyOpened)
	{
		bool isOpen = m_db.open();

		if (!isOpen)
		{
			QString logMsg = "Open database is failed, database name is :"+  m_db.databaseName() + m_db.lastError().databaseText();
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
		}
		else
		{
			m_db.exec("PRAGMA synchronous = OFF");
			isAlreadyOpened = true;
		}
	}
	return isAlreadyOpened;
}

bool HistoryDataBase::closeDatabase()
{
	if (isAlreadyOpened)
	{
		m_db.close();
		isAlreadyOpened = false;
	}
	return true;
}
void HistoryDataBase::loadHistoryImgPathTableName(DutInfoShowArray &historyDutSNArray)
{
    QString sql = "SELECT DISTINCT eyebox FROM ";
    DutInfoShow m_dutShow;
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        m_dutShow.eyeboxs.clear();
        for (const auto &tableName : m_db.tables())
        {
            if (tableName.contains("iq_historyDirectory_"))
            {
                m_dutShow.dutSN = tableName.mid(tableName.lastIndexOf("_") + 1).toStdString();
                QString sqlStr = sql + tableName;
                if (!query.exec(sqlStr))
                {
                    QString logMsg =
                        "select  history table name fail: " + query.lastError().text() + ", " + query.lastQuery();
                    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
                    closeDatabase();
                }
                while (query.next())
                {
                    std::string eyebox = query.record().value(0).toString().toStdString();
                    m_dutShow.eyeboxs.push_back(eyebox);
                }
                historyDutSNArray.push_back(m_dutShow);
            }
        }
        closeDatabase();
    }
}

void HistoryDataBase::queryHistoryTableName(DutInfoShowArray& historyDutSNArray)
{
	QString sql = "SELECT DISTINCT eyebox FROM ";
	DutInfoShow m_dutShow;
	if (openDatabase())
	{
		QSqlQuery query(m_db);
		m_dutShow.eyeboxs.clear();
		for (const auto& tableName : m_db.tables())
		{
			if (tableName.contains("iq_history_"))
			{
				m_dutShow.dutSN = tableName.mid(tableName.lastIndexOf("_")+1).toStdString();
				QString sqlStr = sql + tableName;
				if (!query.exec(sqlStr))
				{
					QString logMsg = "select  history table name fail: " + query.lastError().text() + ", " + query.lastQuery();
					LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
					closeDatabase();
				}
				while (query.next())
				{
					std::string eyebox = query.record().value(0).toString().toStdString();
					m_dutShow.eyeboxs.push_back(eyebox);
				}
				historyDutSNArray.push_back(m_dutShow);
			}

		}
		closeDatabase();
	}
}

void HistoryDataBase::queryTableFormat(IQTableFormatArray* iqTableFormats)
{
	QString sql = "SELECT * FROM ";
	QString sqlEnd = " LIMIT 1";
	IQTableFormat m_iqTableFormat;
	if (openDatabase())
	{
		QSqlQuery query(m_db);
		for (const auto& tableName : m_db.tables())
		{
			if (tableName.contains("iq_history_"))
			{
				m_iqTableFormat.tableName = tableName.toStdString();
				QString sqlStr = sql + tableName + sqlEnd;
				if (!query.exec(sqlStr))
				{
					QString logMsg = "query table format is failed: "  + query.lastError().text() + ", " + query.lastQuery();
					LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
					closeDatabase();
				}
				query.next();
				for (int i = 3; i < query.record().count(); i++)
				{
					std::string head = query.record().fieldName(i).toStdString();
					m_iqTableFormat.headers.push_back(head);
				}
				iqTableFormats->push_back(m_iqTableFormat);
			}
		}
		closeDatabase();
	}
}

void HistoryDataBase::DelTimeOutDBData(QString oldTime)
{
	QString sqlS = "DELETE FROM ";
	QString sqlEnd = " WHERE Time < " + oldTime;
	IQTableFormat m_iqTableFormat;
	if (openDatabase())
	{
		QSqlQuery query(m_db);
		for (const auto& tableName : m_db.tables())
		{
			if (tableName.contains("iq_history_") || tableName.contains("MTF_"))
			{
				QString sqlStr = sqlS + tableName + sqlEnd;
				if (!query.exec(sqlStr))
				{
					QString logMsg = "delete timeout format is failed:" + query.lastError().text() + ", " + query.lastQuery();
					LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
					closeDatabase();
				}
			}
		}
		closeDatabase();
	}
}

void HistoryDataBase::DelDiffFormatTable(StringArray tableNameArray)
{
	QString sqlS = "drop table if exists ";
	if (openDatabase())
	{
		QSqlQuery query(m_db);
		for (const auto& tableName : tableNameArray)
		{
			QString sqlStr = sqlS + QString::fromStdString(tableName);
			if (!query.exec(sqlStr))
			{
				QString logMsg = "drop table is failed:" + query.lastError().text() + ", " + query.lastQuery();
				LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
				closeDatabase();
			}
		}
		closeDatabase();
	}
}

GDataMap HistoryDataBase::UpdateImgPathDirInfo(QString dutSn, QString eyebox, QString startTime, QString endTime)
{
    GColumMap m_columMap;
    GDataMap m_dataMap;
    QStringList paths = {};
    QString selectSql;
    if (startTime != "" && endTime != "")
    selectSql = "SELECT * FROM iq_historyDirectory_" + dutSn +" WHERE Time BETWEEN " + "'" + startTime + "'" +
                        " AND " + "'" + endTime + "'";
    else
    {
        selectSql = "SELECT * FROM iq_historyDirectory_"+dutSn;
	}
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        if (!query.exec(selectSql))
        {
            closeDatabase();
            QString logMsg = "select show image from metric history table is failed:" + query.lastError().text() + ", " +
                             query.lastQuery();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
        }
        int j = 0;
        while (query.next())
        {
            m_columMap.clear();
            j++;

            for (int i = 1; i < query.record().count(); i++)
            {
                std::string value = query.record().value(i).toString().toStdString();
                m_columMap.insert(make_pair(i, value));
            }
            m_dataMap.insert(make_pair(j, m_columMap));
        }
        closeDatabase();
    }
    return m_dataMap;
}

QStringList HistoryDataBase::getIQMerticsImage(QString dutSn)
{
    QStringList imgList = {};
    QString selectSql = "SELECT imgName FROM iq_historyImgInfo_" + dutSn ;

    if (openDatabase())
    {
        QSqlQuery query(m_db);
        QString imgList = query.record().value("imgName").toString();
        if (!query.exec(selectSql))
        {
            closeDatabase();
            QString logMsg = "select show image from metric history table is failed:" + query.lastError().text() +
                             ", " + query.lastQuery();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
            return {};
        }
        closeDatabase();
    }
    return imgList;
}

GDataMap HistoryDataBase::UpdateMetricTableShowInfo(QString startTime, QString endTime, QString dutSn, QString eyebox)
{
	GColumMap m_columMap;
	GDataMap m_dataMap;

	QString tableName = "iq_history_" + dutSn;
	QString selectSql = "";
	if (eyebox == "All")
	{
		selectSql = "SELECT * FROM " + tableName + " WHERE Time BETWEEN " + "'" + startTime + "'" + " AND " + "'" + endTime + "'";
	}
	else
	{
		selectSql = "SELECT * FROM " + tableName + " WHERE Time BETWEEN " + "'" + startTime + "'" + " AND " + "'" + endTime + "'" +
			" AND eyebox=" + eyebox;
	}
	
	if (openDatabase())
	{
		QSqlQuery query(m_db);
		if (!query.exec(selectSql))
		{
			closeDatabase();
			QString logMsg = "select show data from metric history table is failed:" + query.lastError().text() + ", " + query.lastQuery();
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
		}
		int j = 0;
		while (query.next())
		{
			m_columMap.clear();
			j++;
			for (int i = 1; i < query.record().count(); i++)
			{
				std::string value = query.record().value(i).toString().toStdString();
				m_columMap.insert(make_pair(i, value));
			}
			m_dataMap.insert(make_pair(j, m_columMap));
		}
		closeDatabase();
	}
	return m_dataMap;
}

GDataMap HistoryDataBase::UpdateMTFTableShowInfo(QString startTime, QString endTime, QString dutSn, QString eyebox, QString color, QStringList& mtfHeadList)
{
	mtfHeadList.clear();
	GColumMap m_columMap;
	GDataMap m_dataMap;
	QString tableName = "MTF_" + dutSn;
	QString selectSql = "SELECT * FROM " + tableName + " WHERE Time BETWEEN " + "'" + startTime + "'" + " AND " + "'" + endTime + "'";
	if (eyebox != "All")
	{
		selectSql = selectSql + " AND " + "eyebox=" + eyebox;
	}
	if (color != "All")
	{
		selectSql = selectSql + " AND " + "color=" + color;
	}
	if (openDatabase())
	{
		QSqlQuery query(m_db);
		if (!query.exec(selectSql))
		{
			closeDatabase();
			QString logMsg = "select show data from mtf table is failed:" + query.lastError().text() + ", " + query.lastQuery();
			LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
		}
		int j = 0;
		while (query.next())
		{
			m_columMap.clear();
			j++;
			for (int i = 0; i < query.record().count(); i++)
			{
				QString name = query.record().fieldName(i);
				std::string value = query.record().value(i).toString().toStdString();
				m_columMap.insert(make_pair(i, value));
				if (j == 1)
				{
					mtfHeadList.push_back(name);
				}
			}
			m_dataMap.insert(make_pair(j, m_columMap));
		}
		closeDatabase();
	}
	return m_dataMap;
}





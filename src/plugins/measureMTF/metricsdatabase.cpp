#include "metricsdatabase.h"
#include "Core/loggingwrapper.h"
#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QSqlField>
#include <plugins/PrjCommon/metricsdata.h>
QMutex MetricsDataBase::mutex;
MetricsDataBase* MetricsDataBase::self = nullptr;
MetricsDataBase::MetricsDataBase(const QString& path,QObject *parent)
	: QObject(parent)
{
	m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), "metricsdb");
	m_db.setDatabaseName(path);
    Result ret = createSlbCrTable();
    if (!ret.success)
    {
        qCritical() << QString::fromStdString(ret.errorMsg);
    }

    m_db.close();
}
MetricsDataBase* MetricsDataBase::getInstance(QObject* parent)
{
	if (!self) {
		QMutexLocker locker(&mutex);
		if (!self) {
			self = new MetricsDataBase("./config/mainconfig.db", parent);
        
		}
	}
	self->setParent(parent);
	return self;
}

void MetricsDataBase::updateMetricValue( QString name, double value,QString id)
{
    QMutexLocker locker(&m_mutex);

    QString sqlStr;
    if (id == "")
    {
        sqlStr = "insert into iqcomparison (name,updatetime,value) values(:name,:updatetime,:value)ON CONFLICT(name) DO UPDATE SET updatetime=:updatetime,value=:value";
    }
    else
    {
        double lowlimit = queryOneMetricValue(id, name, "lowlimit");
        double uplimit = queryOneMetricValue(id, name, "uplimit");

        if (lowlimit == uplimit)
            sqlStr = "UPDATE iqmetrics_" + id + " SET value=:value,updatetime=:updatetime  where name=:name";
        else if (value >= lowlimit && value <= uplimit)
        {
            sqlStr = "UPDATE iqmetrics_" + id + " SET value=:value,updatetime=:updatetime,pass=1  where name=:name";
        }
        else
        {
            sqlStr = "UPDATE iqmetrics_" + id + " SET value=:value,updatetime=:updatetime,pass=0  where name=:name";
        }

        //sqlStr = "insert into iqmetrics_" + id + "(name,updatetime,value) values(:name,:updatetime,:value)ON CONFLICT(name) DO UPDATE SET updatetime=:updatetime,value=:value";
    }
    if (openDatabase()) {
		QSqlQuery query(m_db);
		QDateTime dateTime = QDateTime::currentDateTime();
		QString str = dateTime.toString("yyyy-MM-dd hh:mm:ss");
		query.prepare(sqlStr);
		query.bindValue(":name", name);
		query.bindValue(":value", value);
		query.bindValue(":updatetime", str);
		bool ok = query.exec();
		if (!ok) {
			qCritical() << "update value failed: " << query.lastError() << query.lastQuery();
			closeDatabase();
		}
        else{
            closeDatabase();
        }
	}
}
void MetricsDataBase::updatePathValue(QString dutID, QString path, QString id)
{
    //TODO: not used
    return;

    QString pathDataTime = MetricsData::instance()->getCaptureFormatTime();
    QString sql = "SELECT path FROM iq_historyDirectory_" + dutID + " WHERE time=" + pathDataTime;

    QString sqlinsert = "INSERT INTO iq_historyDirectory_" + dutID + "(Time, eyebox, path) VALUES(:historyTableTime, :id, :path)";

    QString sqlStr = "UPDATE iq_historyDirectory_" + dutID + " SET path = :value where Time = :historyTableTime";
    path.replace("\\\\", "\\");
    if (openDatabase())
    {
        bool sqlOk = false;
        QSqlQuery query(m_db);
        bool ok = query.exec(sql);
        if (ok)
        {
            query.next();
            QString pathData = query.record().value(0).toString();
            if (pathData==NULL)
            {
                ok = false;
            }
        }
        sqlOk = ok;

        if (!sqlOk)
        {
            query.prepare(sqlinsert);
            query.bindValue(":historyTableTime", pathDataTime);
            query.bindValue(":id", id);
            query.bindValue(":path", path.left(path.size() - 1));
            sqlOk = query.exec();
            if (!sqlOk)
            {
                qCritical() << "insert value failed: " << query.lastError().text() << query.lastQuery();
                QString msgLog =
                    "insert iq_history_" + dutID +  " value failed : " + query.lastError().text();
                LoggingWrapper::instance()->error(msgLog);
                closeDatabase();
            }
        }
        else
        {
            query.prepare(sqlStr);
            query.bindValue(":historyTableTime", pathDataTime);
            query.bindValue(":id", id);
            query.bindValue(":value", path);
            sqlOk = query.exec();
            if (!sqlOk)
            {
                qCritical() << "update value failed: " << query.lastError().text() << query.lastQuery();
                QString msgLog =
                    "update iq_history_" + dutID + " value failed : " + query.lastError().text();
                LoggingWrapper::instance()->error(msgLog);
                closeDatabase();
            }
        }
        closeDatabase();
    }
}
double MetricsDataBase::queryOneMetricValue(QString n, QString name, QString fieldName)
{
    QString sqlStr = "select " + fieldName + " from iqmetrics_" + n + " where name = ?";
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        query.prepare(sqlStr);
        query.addBindValue(name);
        bool ok = query.exec();
        if (!ok)
        {
            qCritical() << "get metric value failed: " << query.lastError() << query.lastQuery();
           	closeDatabase();
        }
        else
        {
            double val = -1;
            while (query.next())
            {
                val = query.value(fieldName).toDouble();
            }

            closeDatabase();
            return val;
        }
    }
    return 0.0;
}
std::vector<QStringList> MetricsDataBase::getCompareMetricKeyValue()
{
    QString sqlStr = "select name from iqcomparison order by id";
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        if (!query.exec(sqlStr))
        {
            closeDatabase();
        }

        while (query.next())
        {
            QString fullname = query.value("name").toString();
            QString id = fullname.right(1);
           // QString name = fullname.left(fullname.length()-8);
            QStringList keyinfo;
            keyinfo << id << fullname;
            keyinfoVec.push_back(keyinfo);
        }
        closeDatabase();
    }
    return keyinfoVec;
}

double MetricsDataBase::getMetricValue(QString name)
{
	QString sqlStr = "select value from iqmetrics where name = ?";
	if (openDatabase()) {
		QSqlQuery query(m_db);
		query.prepare(sqlStr);
		query.addBindValue(name);
		bool ok = query.exec();
		if (!ok) {
			qCritical() << "get metric value failed: " << query.lastError() << query.lastQuery();
			closeDatabase();
		}
		else {
			double val = -1;
			while (query.next())
			{
				val= query.value("value").toDouble();
			}
			
			closeDatabase();
			return val;
		}
	}
	return 0.0;
}

void MetricsDataBase::delIQMetricDatas()
{
	QString sqlDelete = QString("delete from iqmetrics");
	if (openDatabase()) {
		QSqlQuery query(m_db);
		bool ok = query.exec(sqlDelete);
		if (!ok) {
			qCritical() << "del metric value failed: " << query.lastError() << query.lastQuery();
			closeDatabase();
		}
	}
	else {
		qCritical() << "open database  failed: ";
	}
}

void MetricsDataBase::delIQMetricDatas(int n)
{
    if (openDatabase())
    {
        QString sqlDelete;
        // for (size_t i = 1; i <= n; i++)
        {
            // sqlDelete = QString("delete value from iqmetrics_") + QString::number(i);
            sqlDelete = QString("update iqmetrics_") + QString::number(n) + " set value = null,pass = null";

            QSqlQuery query(m_db);
            bool ok = query.exec(sqlDelete);
            if (!ok)
            {
                qCritical() << "del IQ metric value failed: " << query.lastError() << query.lastQuery();
                closeDatabase();
            }
        }
        closeDatabase();
    }
    else
    {
        qCritical() << "open database  failed: ";
    }
}

void MetricsDataBase::delTestInfoDatas()
{
    if (openDatabase())
    {
        QString sqlDelete;
        sqlDelete = "update testinfo set value = null where name in('Appraiser', 'Date', 'Time' , 'TestResult', "
                    "'TestFailItem', 'TestTotalTime', 'DutAngle', 'Diopter')";

        QSqlQuery query(m_db);
        bool ok = query.exec(sqlDelete);
        if (!ok)
        {
            qCritical() << "del test info value failed: " << query.lastError() << query.lastQuery();
            closeDatabase();
        }
        else
            closeDatabase();
    }
    else
    {
        qCritical() << "open database  failed: ";
    }
}

void MetricsDataBase::insertMetricsItem(QString name, QString unit)
{
	QString sqlStr = "insert into iqcomparison(name,unit) values(:name,:unit)ON CONFLICT(name) DO UPDATE SET unit=:unit";
	if (openDatabase()) {
		QSqlQuery query(m_db);
		query.prepare(sqlStr);
		query.bindValue(":name", name);
		query.bindValue(":unit", unit);
		bool ok = query.exec();
		if (!ok) {
			qCritical() << "insert iqcomparison metric failed: " << query.lastError() << query.lastQuery();
			closeDatabase();
		}
		else {
			closeDatabase();
		}
	}
}

void MetricsDataBase::insertMetricsItem(QString name, float uplimit, float lowlimit, QString eyeboxid, QString unit)
{
    //QString sqlStr = "insert into iqmetrics_" + QString::number(eyeboxid+1) + "(name, uplimit, lowlimit, unit)values(:name,:uplimit,:lowlimit,:unit) ON CONFLICT(name) DO UPDATE SET uplimit = :uplimit,lowlimit = :lowlimit,unit = :unit ";
    QString sqlStr = "insert into iqmetrics_" + eyeboxid +
                     "(name, uplimit, lowlimit, value,unit)values(:name,:uplimit,:lowlimit,'None',:unit) ON CONFLICT(name) DO "
                     "UPDATE SET uplimit = :uplimit,lowlimit = :lowlimit,value = 'None',unit = :unit ";
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        query.prepare(sqlStr);
        query.bindValue(":name", name);
        query.bindValue(":uplimit", uplimit);
        query.bindValue(":lowlimit", lowlimit);
        query.bindValue(":unit", unit);
        bool ok = query.exec();
        if (!ok)
        {
            qCritical() << "insert metric failed: " << query.lastError() << query.lastQuery();
            closeDatabase();
        }
        else
        {
            closeDatabase();
        }
    }
}

bool MetricsDataBase::openDatabase()
{
	if (!isAlreadyOpened)
	{
		bool isOpen = m_db.open();

		if (!isOpen)
		{
			LoggingWrapper::instance()->error("Open database is failed, database name is :" + m_db.databaseName() + m_db.lastError().databaseText());
		}
		else
		{
			m_db.exec("PRAGMA synchronous = OFF");
			isAlreadyOpened = true;
		}
	}
	return isAlreadyOpened;
}

Result MetricsDataBase::createSlbCrTable()
{
    QString table = DB_SLB_CR;
    if (!openDatabase())
    {
        return Result(false, QString("Create table %1 failed, open Database failed.").arg(table).toStdString());
    }

    QString sql = QString("CREATE TABLE if not exists '%1'").arg(table);
    QString tmp = "('id' INTEGER UNIQUE,'color' TEXT,'type' INTEGER,'data' BLOB,'updatetime' TEXT,PRIMARY KEY('id' "
                  "AUTOINCREMENT))";
    sql = sql + tmp;

    QSqlQuery query(m_db);
    if (!query.exec(sql))
    {
        closeDatabase();
        return Result(false,
                      QString("Create table %1 error: %1").arg(table).arg(query.lastError().text()).toStdString());
    }
    closeDatabase();
    return Result();
}
Result MetricsDataBase::insertRecord(const QString &table, const QMap<QString, QVariant> record)
{
    if (record.size() == 0)
    {
        return Result(false, QString("Insert table %1 failed, record is empty.").arg(table).toStdString());
    }

    if (!openDatabase())
    {
        return Result(false, QString("Insert table %1 failed, open Database failed.").arg(table).toStdString());
    }

    QSqlQuery query(m_db);
    {
        // query.prepare("INSERT INTO dept (id, name, loc) VALUES (:id, :name, :loc)");
        QString fields;
        QString values;
        QMap<QString, QVariant>::const_iterator iter;
        for (iter = record.begin(); iter != record.end(); ++iter)
        {
            if (!fields.isEmpty())
            {
                fields += ",";
                values += ",";
            }
            fields += iter.key();
            values += ":" + iter.key();
        }
        QString insertStr = QString("INSERT INTO %1 (%2) VALUES (%3) ").arg(table).arg(fields).arg(values);

        query.prepare(insertStr);
        for (iter = record.begin(); iter != record.end(); ++iter)
        {
            query.bindValue(":" + iter.key(), iter.value());
        }
    }

    bool ok = query.exec();
    closeDatabase();
    if (ok)
    {
        return Result();
    }
    else
    {
        return Result(
            false, QString("Insert table %1 record error: %1").arg(table).arg(query.lastError().text()).toStdString());
    }
}

Result MetricsDataBase::updateRecord(const QString &table, const QMap<QString, QVariant> record,
                                     const QMap<QString, QVariant> condition)
{
    if (record.size() == 0)
    {
        return Result(false, QString("Update table %1 failed, record is empty.").arg(table).toStdString());
    }

    if (!openDatabase())
    {
        return Result(false, QString("Update table %1 failed, open Database failed.").arg(table).toStdString());
    }
    QSqlQuery query(m_db);
    {
        // query5.prepare("UPDATE rfid_match SET CKH=:ckh,WLMS=:wlms WHERE WZSFM=11");
        QString fields;
        QMap<QString, QVariant>::const_iterator iter;
        for (iter = record.begin(); iter != record.end(); ++iter)
        {
            if (!fields.isEmpty())
            {
                fields += ",";
            }
            fields += iter.key() + "=:" + iter.key();
        }

        QString condStr;
        for (iter = condition.begin(); iter != condition.end(); ++iter)
        {
            if (!condStr.isEmpty())
            {
                condStr += " and ";
            }
            condStr += iter.key() + "='" + iter.value().toString() + "'";
        }

        QString insertStr = QString("UPDATE %1 SET %2 WHERE %3 ").arg(table).arg(fields).arg(condStr);

        query.prepare(insertStr);
        for (iter = record.begin(); iter != record.end(); ++iter)
        {
            query.bindValue(":" + iter.key(), iter.value());
        }
    }

    bool ok = query.exec();
    closeDatabase();
    if (ok)
    {
        return Result();
    }
    else
    {
        return Result(
            false, QString("Update table %1 record error: %1").arg(table).arg(query.lastError().text()).toStdString());
    }
}

Result MetricsDataBase::queryRecord(QList<QMap<QString, QVariant>> &records, const QString &table,
                                    const QMap<QString, QVariant> condition, const QList<QString> fields)
{
    if (condition.size() == 0)
    {
        return Result(false, QString("Query table %1 failed, condition is empty.").arg(table).toStdString());
    }

    if (!openDatabase())
    {
        return Result(false, QString("Query table %1 failed, open Database failed.").arg(table).toStdString());
    }
    QString fieldsCond = "*";
    if (fields.size() > 0)
    {
        fieldsCond = fields.join(",");
    }

    QSqlQuery query(m_db);
    QString sql = QString("select %1 from %2 ").arg(fieldsCond).arg(table);
    int i;

    sql += QString(" where ");
    QMap<QString, QVariant>::const_iterator iterCond;
    for (iterCond = condition.begin(); iterCond != condition.end(); ++iterCond)
    {
        if (iterCond != condition.begin())
        {
            sql += " and ";
        }
        sql += iterCond.key() + " = " + QString("'%1'").arg(iterCond.value().toString());
    }

    // SELECT LastName,FirstName FROM Persons where LastName = 'Tom'
    qInfo() << "Query record ,sql:" << sql;
    bool ok = query.exec(sql);
    if (ok)
    {
        while (query.next())
        {
            QMap<QString, QVariant> data;
            for (int i = 0; i < query.record().count(); ++i)
            {
                data.insert(query.record().field(i).name().toUpper(), query.record().field(i).value());
            }
            records.append(data);
        };
        closeDatabase();
        return Result();
    }
    else
    {
        closeDatabase();
        return Result(
            false, QString("Query table %1 record error: %1").arg(table).arg(query.lastError().text()).toStdString());
    }
}

Result MetricsDataBase::deleteSurplusRecord(const QString &table, const QMap<QString, QVariant> condition,
                                            int residueNumber)
{
    if (!openDatabase())
    {
        return Result(false, QString("Delete table %1 failed, open Database failed.").arg(table).toStdString());
    }

    QSqlQuery query(m_db);
    QString sql;
    {
        // query5.prepare("UPDATE rfid_match SET CKH=:ckh,WLMS=:wlms WHERE WZSFM=11");
        QMap<QString, QVariant>::const_iterator iter;
        QString condStr;
        for (iter = condition.begin(); iter != condition.end(); ++iter)
        {
            if (!condStr.isEmpty())
            {
                condStr += " and ";
            }
            condStr += iter.key() + "='" + iter.value().toString() + "'";
        }

        // DELETE FROM SLB_CR WHERE id NOT IN (SELECT id FROM SLB_CR where color="R" AND type="1" ORDER BY id DESC LIMIT
        // 4) and color="R" AND type="1"
        sql = QString("DELETE FROM %1 WHERE id NOT IN(SELECT id FROM %2 where %3 ORDER BY id DESC LIMIT %4) and %5")
                  .arg(table)
                  .arg(table)
                  .arg(condStr)
                  .arg(residueNumber)
                  .arg(condStr);
    }

    bool ok = query.exec(sql);
    closeDatabase();
    if (ok)
    {
        return Result();
    }
    else
    {
        return Result(
            false, QString("Delete table %1 record error: %1").arg(table).arg(query.lastError().text()).toStdString());
    }

    return Result();
}




MetricsDataBase::~MetricsDataBase()
{}

void MetricsDataBase::beginTransaction()
{
	if (!openDatabase())
		return;
	m_db.transaction();
}

void MetricsDataBase::endTransaction()
{
	if (!openDatabase())
		return;
	m_db.commit();
	closeDatabase();
}
QStringList MetricsDataBase::exportToCsv()
{
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        QStringList content;
        QString sql = QString("select * from iqmetrics");
        if (!query.exec(sql))
        {
			closeDatabase();
			return content;
        }

        /*Loop through the data*/
        int num = 0;
        while (query.next())
        {
            QStringList list;
            QStringList thList;
            for (int i = 0; i < query.record().count(); i++)
            {
                if (0 == num)
                {
                    auto name = query.record().fieldName(i);
                    thList << name;
                }
                list << query.value(i).toString();
            }
            if (0 == num)
                content << thList.join(";") << list.join(";");
            else
				content << list.join(";");
            num++;
        }
		closeDatabase();
        return content;
    }
}

//Used to create table headers and fill in values
QStringList MetricsDataBase::getAllMetricsToCsv(QString field, const QList<QString> &eyeboxIds)
{
	if (openDatabase())
	{
		QSqlQuery query(m_db);
		QStringList content;
        QString lastsql;
        int num = eyeboxIds.size() > 0 ? eyeboxIds.size() : 1;
        for (size_t i = 0; i < num; i++)
        {
            if (eyeboxIds.size() > 0)
            {
                lastsql = "iqmetrics_" + eyeboxIds.at(i) + " order by id ";
            }
            else
            {
                lastsql = "iqcomparison order by id";
            }
            
            QString sql = "select " + field + " from " + lastsql; // QString(" select name from iqmetrics_0 order by id ");
            if (!query.exec(sql))
            {
                closeDatabase();
                return content;
            }
            //int num = 0;
            while (query.next())
            {
                QString item = query.value(field).toString();
                if (field == "name" && num > 1)
                {
                   // content.append(item + "_" + QString::number(i+1)); 
                    content.append(item); 
                }
                else
                {
                    content.append(item);
                }            
            }
        }
		closeDatabase();
		return content;
	}
}

QStringList MetricsDataBase::getAllMetricsToCsv(QString field, QString eyeboxQueue)
{
    if (openDatabase())
    {
        QStringList eyeboxlist = eyeboxQueue.split(",");
        QSqlQuery query(m_db);
        QStringList content;
        QString lastsql;
        for (size_t i = 0; i < eyeboxlist.size(); i++)
        {
            lastsql = "iqmetrics_" + QString::number(eyeboxlist[i].toInt()) + " order by id ";
            QString sql =
                "select " + field + " from " + lastsql; // QString(" select name from iqmetrics_0 order by id ");
            if (!query.exec(sql))
            {
                continue;
                // closeDatabase();
                // return content;
            }
            // int num = 0;
            while (query.next())
            {
                QString item = query.value(field).toString();
                content.append(item);
            }
        }
        closeDatabase();
        return content;
    }
}


bool MetricsDataBase::createMetricsTable()
{
	if (openDatabase())
	{
		QSqlQuery query(m_db);
		QStringList content;
		QString dropsql = "drop table if exists 'iqcomparison'";
		if (!query.exec(dropsql))
		{
			closeDatabase();
			LoggingWrapper::instance()->error("error while drop iqcomparison");
			return false;
		}
		QString sql = QString("CREATE TABLE if not exists 'iqcomparison' ( \
				'id' INTEGER UNIQUE,\
				'name' TEXT NOT NULL UNIQUE,\
				'updatetime' TEXT,\
				'value' NUMERIC,\
				'unit' TEXT,\
				PRIMARY KEY('id' AUTOINCREMENT)\
				)");
		if (!query.exec(sql))
		{
		    closeDatabase();
			LoggingWrapper::instance()->error("error while create iqcomparison table");
			return false;
		}
		closeDatabase();
		return true;
	}
}

bool MetricsDataBase::createMultiMetricsTable(int metricTableNum)
{
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        for (int i = 0; i < metricTableNum; i++)
        {
            QString dropsql = QString("drop table if exists 'iqmetrics_")+ QString::number(i+1) + "'";
            if (!query.exec(dropsql))
            {
                closeDatabase();
                LoggingWrapper::instance()->error("error while drop iqmetric#");
                return false;
            }

            QString sql = QString("CREATE TABLE if not exists 'iqmetrics_") + QString::number(i+1)+QString("' ( \
				'id' INTEGER UNIQUE,\
				'name' TEXT NOT NULL UNIQUE,\
				'uplimit' NUMERIC,\
				'lowlimit' NUMERIC,\
				'updatetime' TEXT,\
				'value' NUMERIC,\
                'pass' NUMERIC,\
				'unit' TEXT,\
				PRIMARY KEY('id' AUTOINCREMENT)\
				)");
            if (!query.exec(sql))
            {
                closeDatabase();
                LoggingWrapper::instance()->error("error while create iqmetrics table");
                return false;
            }
        }

		//Create a comparison table
        createMetricsTable();

        closeDatabase();
        return true;
    }
    return false;
}
bool MetricsDataBase::createMultiMetricsTable(const QList<QString> &metricTableIds)
{
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        for (int i = 0; i < metricTableIds.size(); i++)
        {
            QString dropsql = QString("drop table if exists 'iqmetrics_") + metricTableIds.at(i) + "'";
            if (!query.exec(dropsql))
            {
                closeDatabase();
                LoggingWrapper::instance()->error("error while drop iqmetric#");
                return false;
            }

            QString sql = QString("CREATE TABLE if not exists 'iqmetrics_") + metricTableIds.at(i) + QString("' ( \
				'id' INTEGER UNIQUE,\
				'name' TEXT NOT NULL UNIQUE,\
				'uplimit' NUMERIC,\
				'lowlimit' NUMERIC,\
				'updatetime' TEXT,\
				'value' NUMERIC,\
                'pass' NUMERIC,\
				'unit' TEXT,\
				PRIMARY KEY('id' AUTOINCREMENT)\
				)");
            if (!query.exec(sql))
            {
                closeDatabase();
                LoggingWrapper::instance()->error("error while create iqmetrics table");
                return false;
            }
        }

        // Create a comparison table
        createMetricsTable();

        closeDatabase();
        return true;
    }
    return false;
}
bool MetricsDataBase::closeDatabase()
{
	if (isAlreadyOpened)
	{
		m_db.close();
		isAlreadyOpened = false;
	}
	return true;
}

QStringList MetricsDataBase::getTestInfoHeaders()
{
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        QStringList content;
        QString sql = "select name from testinfo order by id ";

        if (!query.exec(sql))
        {
            closeDatabase();
            return content;
        }
        while (query.next())
        {
            QString item = query.value("name").toString();
            content.append(item);
        }
        closeDatabase();
        return content;
    }
    return QStringList();
}
QStringList MetricsDataBase::getTestInfoValues()
{
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        QStringList content;
        QString sql = "select value from testinfo order by id ";

        if (!query.exec(sql))
        {
            closeDatabase();
            return content;
        }
        while (query.next())
        {
            QString item = query.value("value").toString();
            content.append(item);
        }
        closeDatabase();
        return content;
    }
    return QStringList();
}



QStringList MetricsDataBase::getOneFieldValues(QString fieldName, QString tableName)
{
    if (fieldName.isEmpty() || tableName.isEmpty())
    {
        return QStringList();
    }
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        QStringList content;
        QString sql = "select "+fieldName+" from "+ tableName +" order by id";

        if (!query.exec(sql))
        {
            closeDatabase();
            return content;
        }
        while (query.next())
        {
            QString item = query.value(fieldName).toString();
            content.append(item);
        }
        closeDatabase();
        return content;
    }
    return QStringList();
}

bool MetricsDataBase::updateOneFieldValue(QString fieldName, QString tableName, QString value)
{
    QString sqlStr = "UPDATE "+tableName+ " SET value='"+value+"'  where name='" + fieldName+"'";
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        bool ok = query.exec(sqlStr);
        if (!ok)
        {
            qCritical() << "update feild failed: " << query.lastError() << query.lastQuery();
            closeDatabase();
            return false;
        }
        else
        {
            closeDatabase();
        }
    }

    return true;
}
bool MetricsDataBase::updateWPCFiledValueByFilter(double value, int levelid, QString filter, QString color)
{
    QString sqlStr;
    if (color.isEmpty())
    {
        sqlStr = "UPDATE wpcinfo SET value=:value  where level=:levelid and filter=:filter and color is NULL";
    }
    else
    {
        sqlStr = "UPDATE wpcinfo SET value=:value  where level=:levelid and color=:color and filter=:filter";
    }
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        query.prepare(sqlStr);
        query.bindValue(":value", value);
        query.bindValue(":levelid", levelid);
        query.bindValue(":filter", filter);
        if (!color.isEmpty())
        {
            query.bindValue(":color", color);
        }
        bool ok = query.exec();
        if (!ok)
        {
            qCritical() << "update value failed: " << query.lastError() << query.lastQuery();
            closeDatabase();
            return false;
        }
        else
        {
            closeDatabase();
            return true;
        }
    }
    else
    {
        return false;
    }

}
bool MetricsDataBase::updateWPCFieldValue(double value, int levelid, QString color, QString xyz)
{
    QString sqlStr;
    if (xyz.isEmpty())
    {
        sqlStr= "UPDATE wpcinfo SET value=:value  where level=:levelid and color=:color and xyz is NULL";
    }
    else
    {
        sqlStr = "UPDATE wpcinfo SET value=:value  where level=:levelid and color=:color and xyz=:xyz";
    }
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        query.prepare(sqlStr);
        query.bindValue(":value", value);
        query.bindValue(":levelid", levelid);
        query.bindValue(":color", color);
        if (!xyz.isEmpty())
        {
            query.bindValue(":xyz", xyz);
        }
        bool ok = query.exec();
        if (!ok)
        {
            qCritical() << "update value failed: " << query.lastError() << query.lastQuery();
            closeDatabase();
            return false;
        }
        else
        {
            closeDatabase();
            return true;
        }
    }
    else
    {
        return false;
    }
    
}

QStringList MetricsDataBase::getWPCInfoHeaders()
{
    return getOneFieldValues("name", "wpcinfo");
}

QStringList MetricsDataBase::getWPCInfoValues()
{
    return getOneFieldValues("value","wpcinfo");
}

void MetricsDataBase::SetMetricsHistoryTableTime(QString time)
{
    historyTableTime = time;
}

QMap<QString, QString> MetricsDataBase::getIQTestResult(int metricTableNum)
{
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        QMap<QString, QString> content, nullcontent;
        content["TestResult"] = "pass";
        content["TestFailItem"] = "NONE";
        QString lastsql;
        // for (size_t i = 0; i < metricTableNum; i++)
        {
            lastsql = "iqmetrics_" + QString::number(metricTableNum) + " where pass='0'";
            QString sql = "select name from " + lastsql;
            if (!query.exec(sql))
            {
                closeDatabase();
                return nullcontent;
            }
            QString failItem;
            while (query.next())
            {
                failItem = query.value("name").toString();
                content["TestResult"] = "fail";
                content["TestFailItem"] = failItem;
                break;
            }
        }
        closeDatabase();
        return content;
    }
    return QMap<QString, QString>();
}

QMap<QString, QString> MetricsDataBase::getIQTestResult(QString eyeboxQueue)
{
    QStringList eyeboxlist = eyeboxQueue.split(",");
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        QMap<QString, QString> content, nullcontent;
        content["TestResult"] = "PASS";
        content["TestFailItem"] = "NONE";
        QString lastsql;
        for (size_t i = 0; i < eyeboxlist.size(); i++)
        {
            lastsql = "iqmetrics_" + QString::number(eyeboxlist[i].toInt()) + " where pass='0'";
            QString sql = "select name from " + lastsql;
            if (!query.exec(sql))
            {
                closeDatabase();
                return nullcontent;
            }
            QString failItem;
            while (query.next())
            {
                failItem = query.value("name").toString();
                content["TestResult"] = "FAIL";
                content["TestFailItem"] = failItem;
                break;
            }
        }
        closeDatabase();
        return content;
    }
    return QMap<QString, QString>();
}

bool MetricsDataBase::createMultiMetricsHistoryTable(QString dutId, QStringList* headersList)
{
    // TODO: not used
    return true;

    if (openDatabase())
    {
        QSqlQuery query(m_db);
        /*QString dropsql = QString("drop table if exists 'iq_history_") + dutId + "_" + QString::number(i + 1) + "'";
        if (!query.exec(dropsql))
        {
            closeDatabase();
            QString msgLog = "error while create iq_history_" + dutId + "_" + QString::number(i + 1);
            LoggingWrapper::instance()->error(msgLog);
            return false;
        }*/
        QString heads = "";
        for (int i = 0; i < headersList->size(); i++)
        {
            QString head = headersList->at(i);
            heads += "'" + head + "'" + " NUMERIC" + ",";
        }
        QString sql = QString("CREATE TABLE if not exists 'iq_history_") + dutId + QString("'(") + 
            QString("'id' INTEGER UNIQUE,'Time' TEXT,'eyebox' TEXT,") +
            heads + QString("PRIMARY KEY('id' AUTOINCREMENT)") + QString(")");
        if (!query.exec(sql))
        {
            closeDatabase();
            QString msgLog = "error while create iq_history_" + dutId;
            LoggingWrapper::instance()->error(msgLog);
            return false;
        }
        closeDatabase();
        return true;
    }
}

bool MetricsDataBase::createIQMetricsDirectoryHistoryTable(QString dutId, QStringList *headersList)
{
    //TODO: not used
    return true;

    if (openDatabase())
    {
        QSqlQuery query(m_db);
        /*QString dropsql = QString("drop table if exists 'iq_history_") + dutId + "_" + QString::number(i + 1) + "'";
        if (!query.exec(dropsql))
        {
            closeDatabase();
            QString msgLog = "error while create iq_history_" + dutId + "_" + QString::number(i + 1);
            LoggingWrapper::instance()->error(msgLog);
            return false;
        }*/
        QString heads = "";
        for (int i = 0; i < headersList->size(); i++)
        {
            QString head = headersList->at(i);
            heads += "'" + head + "'" + " NUMERIC" + ",";
        }
        QString sql = QString("CREATE TABLE if not exists 'iq_historyDirectory_") + dutId + QString("'(") +
                      QString("'id' INTEGER UNIQUE,'Time' TEXT,'eyebox' TEXT,'path' TEXT,") + heads +
                      QString("PRIMARY KEY('id' AUTOINCREMENT)") + QString(")");
        if (!query.exec(sql))
        {
            closeDatabase();
            QString msgLog = "error while create iq_historyDirectory_" + dutId;
            LoggingWrapper::instance()->error(msgLog);
            return false;
        }
        closeDatabase();
        alreadyCreate = true;
        return true;
    }
}

void MetricsDataBase::updateMetricHistoryValue(QString dutID, QString name, double value, QString id)
{
    // TODO: not used
    return;

    QString sql = "SELECT count(time) FROM iq_history_" + dutID + " WHERE Time = '" + historyTableTime + "'" + " AND eyebox=" + id;

   /* QString sqlinsert = "INSERT INTO iq_history_" + dutID + "(Time, eyebox," + name +
        ") VALUES(:historyTableTime, :id, :value)";*/

    QString sqlinsert = "INSERT INTO iq_history_" + dutID + "(Time, eyebox," + "'" + name + "'" +
        ") VALUES(:historyTableTime, :id, :value)";

    //QString sqlStr = "UPDATE iq_history_" + dutID + " SET " + name + " = :value where Time = :historyTableTime and eyebox= :id";
    QString sqlStr = "UPDATE iq_history_" + dutID + " SET " + "'" + name + "' = :value where Time = :historyTableTime and eyebox= :id";
    if (openDatabase())
    {
        bool sqlOk = false;
        QSqlQuery query(m_db);
        bool ok = query.exec(sql);
        if (ok)
        {
            query.next();
            int count = query.record().value(0).toInt();
            if (count <= 0)
            {
                ok = false;
            }
        }
        sqlOk = ok;

        if (!sqlOk)
        {
            query.prepare(sqlinsert);
            query.bindValue(":historyTableTime", historyTableTime);
            query.bindValue(":id", id);
            query.bindValue(":value", value);
            sqlOk = query.exec();
            if (!sqlOk)
            {
                qCritical() << "insert value failed: " << query.lastError().text() << query.lastQuery();
                QString msgLog =
                    "insert iq_history_" + dutID + "(" + name + ") value failed : " + query.lastError().text();
                LoggingWrapper::instance()->error(msgLog);
                closeDatabase();
            }
        }
        else
        {
            query.prepare(sqlStr);
            query.bindValue(":historyTableTime", historyTableTime);
            query.bindValue(":id", id);
            query.bindValue(":value", value);
            sqlOk = query.exec();
            if (!sqlOk)
            {
                qCritical() << "update value failed: " << query.lastError().text() << query.lastQuery();
                QString msgLog =
                    "update iq_history_" + dutID + "(" + name + ") value failed : " + query.lastError().text();
                LoggingWrapper::instance()->error(msgLog);
                closeDatabase();
            }
        }
        closeDatabase();
    }
}

bool MetricsDataBase::createMTFHistoryTable(QString dutId, QStringList* headList)
{
    if (openDatabase())
    {
        QSqlQuery query(m_db);
     /*   QString dropsql = QString("drop table if exists 'MTF_") + dutId + "'";
        if (!query.exec(dropsql))
        {
            closeDatabase();
            LoggingWrapper::instance()->error("error while drop MTF_#");
            return false;
        }*/
        QString heads = "";
        QString head1 = headList->at(0);
        heads += "'" + head1 + "'" + " TEXT" + ",";
        QString head2 = headList->at(1);
        heads += "'" + head2 + "'" + " NUMERIC" + ",";
        QString head3 = headList->at(2);
        heads += "'" + head3 + "'" + " TEXT" + ",";

        for (int i = 3; i < headList->size(); i++)
        {
            QString head = headList->at(i);
            heads += "'" + head + "'" + " NUMERIC" + ",";
        }

        QString sql = QString("CREATE TABLE if not exists 'MTF_") + dutId +
            QString("'(") + heads + QString("PRIMARY KEY('Time')") + QString(")");
        if (!query.exec(sql))
        {
            closeDatabase();
            LoggingWrapper::instance()->error("error while create MTF_# table");
            return false;
        }
        closeDatabase();
        return true;
    }
    return false;
}


void MetricsDataBase::insertMTFHistoryValue(QString dutSN, QStringList* mtfData)
{
    
    QString sqlinsert = "INSERT INTO MTF_" + dutSN + " VALUES(";
    QString sqlmtfBegin = "'" + mtfData->at(0) + "'" + "," + mtfData->at(1) + "," + "'" + mtfData->at(2) + "'";
    QString sqlmtfData = "";

    for (int i = 3; i < mtfData->size(); i++)
    {
        sqlmtfData += "," + mtfData->at(i);
    }

    QString sqlinsertAll = sqlinsert + sqlmtfBegin + sqlmtfData + ")";
   
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        bool ok = query.exec(sqlinsertAll);
        if (!ok)
        {
            qCritical() << "insert mtf failed: " << query.lastError() << query.lastQuery();
        }
        closeDatabase();
    }
}
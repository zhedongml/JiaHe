#include "configdatabase.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

QMutex ConfigDataBase::mutex;
ConfigDataBase* ConfigDataBase::self = nullptr;

ConfigDataBase::ConfigDataBase(const QString& path,QObject *parent)
	: QObject(parent)
{
	m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"),"configdb");
	m_db.setDatabaseName(path);
	//createTables();
}

ConfigDataBase::~ConfigDataBase()
{
}

ConfigDataBase* ConfigDataBase::getInstance(QObject* parent)
{
	if (!self) {
		QMutexLocker locker(&mutex);
		if (!self) {
			self = new ConfigDataBase("./config/mainconfig.db", parent);
		}
	}
	return self;
}

void ConfigDataBase::beginTransaction()
{
	if (!m_db.isOpen())
		return;
	m_db.transaction();
}

void ConfigDataBase::endTransaction()
{
	if (!m_db.isOpen())
		return;
	m_db.commit();
}

bool ConfigDataBase::updateRecipeFile(const QString& fileName)
{
	if (openDatabase()) {
		QSqlQuery query(m_db);
		query.prepare(QLatin1String("select id from tbl_recipe"));
		if (!query.exec()) {
			qCritical() << "query recipe files failed," << query.lastError() << query.lastQuery();
			m_db.close();
			return false;
		}
		else {
			int id = -1;
			while (query.next()) {
				id = query.value(0).toInt();
			}

			if (id == -1) {
				QFile recipeFile(fileName);
				if (recipeFile.open(QIODevice::ReadOnly)) {
					const QByteArray& tdata = recipeFile.readAll();
					QByteArray data = qCompress(tdata, 9);
					query.prepare(QLatin1String("insert into  tbl_recipe(filepath,contents) values(:filepath,:contents)"));
					query.bindValue(":filepath", fileName);
					query.bindValue(":contents", data);
					bool ok = query.exec();
					if (!ok) {
						qCritical() << "insert tbl_recipe failed: " << query.lastError() << query.lastQuery();
						m_db.close();
						return false;
					}
				}
				else {
					qCritical() << "read  recipe files failed," << query.lastError() << query.lastQuery();
					m_db.close();
					return false;
				}
				
			}
			else {
				QFile recipeFile(fileName);
				if (recipeFile.open(QIODevice::ReadOnly)) {
					const QByteArray& tdata = recipeFile.readAll();
					QByteArray data = qCompress(tdata, 9);
					query.prepare(QLatin1String("update tbl_recipe set  filepath=?,contents=? where id =?"));
					query.addBindValue(fileName);
					query.addBindValue(data);
					query.addBindValue(id);
					bool ok = query.exec();
					if (!ok) {
						qCritical() << "insert tbl_recipe failed: " << query.lastError() << query.lastQuery();
						m_db.close();
						return false;
					}
				}
				else {

					qCritical() << "read  recipe files failed," << query.lastError() << query.lastQuery();
					m_db.close();
					return false;
				}
				
			}

		}

	}
	return true;
}

QString ConfigDataBase::readRecipeFile()
{
	QString str = "";
	if (openDatabase()) {
		QSqlQuery query(m_db);
		query.prepare(QLatin1String("select id,contents from tbl_recipe order  by id desc limit 1"));
		if (!query.exec()) {
			qWarning().nospace() << "Warning: Failed to prepare config database! ("
				<< query.lastError().driverText() << ")";
			m_db.close();
		}
		else {
			while (query.last()) {
				QByteArray byts = query.value("contents").toByteArray();
				QByteArray data = qUncompress(byts);
				std::string stdStr = data.toStdString();
				str = QString::fromStdString(stdStr);
				break;
			}
			m_db.close();
		}
	}
	return str;
}

QString ConfigDataBase::getRecipeFileName()
{
	QString str = "";
	if (openDatabase()) {
		QSqlQuery query(m_db);
		query.prepare(QLatin1String("select id,filepath from tbl_recipe order  by id desc limit 1"));
		if (!query.exec()) {
			qWarning().nospace() << "Warning: Failed to prepare config database! ("
				<< query.lastError().driverText() << ")";
			m_db.close();
		}
		else {
			while (query.last()) {
				str = query.value("filepath").toString();
				break;
			}
			m_db.close();
		}
	}
	return str;
}


bool ConfigDataBase::openDatabase()
{
	bool isOpen = m_db.open();
	if (!isOpen) {
		qDebug() << "Open database is failed, database name is " <<  m_db.databaseName() << m_db.lastError();
	}
	return isOpen;
}

bool ConfigDataBase::createTables()
{
	if (!openDatabase()) {
		m_db.close();
		return false;
	}

	QSqlQuery query(m_db);
	QString recipetable = "CREATE TABLE if not EXISTS 'tbl_recipe' ("\
		"'id'	INTEGER UNIQUE,"\
		"'filepath'	TEXT,"\
		"'contents'	BLOB,"\
		"PRIMARY KEY('ID' AUTOINCREMENT)"\
		");";
	query.prepare(recipetable);
	if (!query.exec()) {
		qDebug() << QString("Fail to create table %1, ").arg(recipetable) << query.lastError();
	}

	m_db.close();
	return true;
}

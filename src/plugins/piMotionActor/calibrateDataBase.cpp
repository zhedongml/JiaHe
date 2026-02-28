#include "calibrateDataBase.h"
#include <extlib/logplus/LogPlus.h>
#include <QSet>
#include <QDebug>
#include <Core/loggingwrapper.h>

QMutex calibrateDataBase::mutex;
calibrateDataBase *calibrateDataBase::self = nullptr;

calibrateDataBase::calibrateDataBase(const QString &path, QObject *parent)
	: QObject(parent)
{
    m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), "calibrateconfigdb");
    m_db.setDatabaseName(path);
    m_db.close();
}

calibrateDataBase::~calibrateDataBase()
{}

calibrateDataBase *calibrateDataBase::getInstance(QObject *parent)
{
    if (!self)
    {
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new calibrateDataBase("./config/mainconfig.db", parent);
        }
    }
    self->setParent(parent);
    return self;
}
bool calibrateDataBase::openDatabase()
{
    if (!isAlreadyOpened)
    {
        bool isOpen = m_db.open();

        if (!isOpen)
        {
            QString logMsg =
                "Open database is failed, database name is :" + m_db.databaseName() + m_db.lastError().databaseText();
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

bool calibrateDataBase::closeDatabase()
{
    if (isAlreadyOpened)
    {
        m_db.close();
        isAlreadyOpened = false;
    }
    return true;
}

bool calibrateDataBase::createDutPrNearCalibrateTable()
{
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        QString sql = QString("CREATE TABLE if not exists 'DutPrNearCalibrate'('id' INTEGER UNIQUE,'eyeType' NUMERIC,'DutX' FLOAT,'DutY' FLOAT,'ProjectorX' FLOAT,'ProjectorY' FLOAT,'DutPrNearCoordinateX' FLOAT,'DutPrNearCoordinateY' FLOAT,'DutPrNearCoordinateZ' FLOAT, PRIMARY KEY('id' AUTOINCREMENT))");
        if (!query.exec(sql))
        {
            closeDatabase();
            QString msgLog = "error while create DutPrNearCalibrate";
            LoggingWrapper::instance()->error(msgLog);
            return false;
        }
        closeDatabase();
        return true;
    }
    return false;
}
Result calibrateDataBase::updateDutPrNearCalibrateValue(cv::Point2f dut, cv::Point2f pr, int eyeType, cv::Point3f col)
{
    QString sql = "SELECT * FROM DutPrNearCalibrate WHERE eyeType = '" + QString::number(eyeType) + "'" +
                  " AND DutX = '" +
                  QString::number(dut.x) + "'" + " AND DutY = '" + QString::number(dut.y) + "'" + " AND ProjectorX = '" + QString::number(pr.x) + "'" +
                  " AND ProjectorY = '" + QString::number(pr.y) + "'";

    QString sqlinsert = "INSERT INTO DutPrNearCalibrate (eyeType, DutX, DutY, ProjectorX, ProjectorY, DutPrNearCoordinateX, DutPrNearCoordinateY, DutPrNearCoordinateZ ) VALUES(:eyeType, ROUND(:dutx,1), ROUND(:duty,1), ROUND(:prx,1), ROUND(:pry,1), ROUND(:colx,1), ROUND(:coly,1), ROUND(:colz,1))";
    QString sqlStr ="UPDATE DutPrNearCalibrate SET DutPrNearCoordinateX = ROUND(:colx,1), DutPrNearCoordinateY = ROUND(:coly,1), DutPrNearCoordinateZ = ROUND(:colz,1) WHERE eyeType = '" +
                      QString::number(eyeType) + "'" + " AND DutX = ROUND('" + QString::number(dut.x) + "',1)" + " AND DutY = ROUND('" + QString::number(dut.y) + "',1) " + " AND ProjectorX = ROUND('" +
                     QString::number(pr.x) + "',1)" + " AND ProjectorY = ROUND('" +
                     QString::number(pr.y) +"',1)";
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
            query.bindValue(":eyeType", eyeType);
            query.bindValue(":dutx", dut.x);
            query.bindValue(":duty", dut.y);
            query.bindValue(":prx", pr.x);
            query.bindValue(":pry", pr.y);
            query.bindValue(":colx", col.x);
            query.bindValue(":coly", col.y);
            query.bindValue(":colz", col.z);
            sqlOk = query.exec();
            if (!sqlOk)
            {
                QString msgLog = "insert DutPrNearCalibrate value failed : " + query.lastError().text() + query.lastQuery();
                LoggingWrapper::instance()->error(msgLog);
                closeDatabase();
                return Result(false, msgLog.toStdString());
            }
        }
        else
        {
            query.prepare(sqlStr);
            query.bindValue(":colx", col.x);
            query.bindValue(":coly", col.y);
            query.bindValue(":colz", col.z);
            sqlOk = query.exec();
            if (!sqlOk)
            {
                QString msgLog = "Update DutPrNearCalibrate value failed : " + query.lastError().text() + query.lastQuery();
                LoggingWrapper::instance()->error(msgLog);
                closeDatabase();
                return Result(false, msgLog.toStdString());
            }
        }
        closeDatabase();
    }
    else
    {
        return Result(false, "Write DutPrNearCalibrate table failed, open database failed.");
    }
    return Result();
}

Result calibrateDataBase::UpdateDisplayDataInfo(QMap<DUT_EYE_TYPE, QList<DutPrNearInfo>> &Ainfo)
{
    DUT_EYE_TYPE eyeType;
    DutPrNearInfo info;
    int type;
    QList<DutPrNearInfo> pinfo;
    QString selectSql = "SELECT * FROM DutPrNearCalibrate WHERE eyeType = 0";
    QString selectSql2 = "SELECT * FROM DutPrNearCalibrate WHERE eyeType = 1";
    if (openDatabase())
    {
        QSqlQuery query(m_db);
        if (!query.exec(selectSql))
        {
            closeDatabase();
            QString logMsg = "select data from DutPrNearCalibrate table is failed:" + query.lastError().text() +
                             ", " + query.lastQuery();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
            return Result(false, logMsg.toStdString());
        }
        int j = 0;
        while (query.next())
        {
            j++;

            for (int i = 0; i < query.record().count(); i++)
            {
                std::string value = query.record().value(i).toString().toStdString();
                switch (i)
                {
                case 0:
                    info.id = std ::stof(value);
                    break;
                case 1: {
                    type = std ::stof(value);
                    if (!type)
                        eyeType = DUT_EYE_LEFT;
                    else
                        eyeType = DUT_EYE_RIGHT;
                }
                    break;
                case 2:
                    info.dutAngle.x = std ::stof(value);
                    break;
                case 3:
                    info.dutAngle.y = std ::stof(value);
                    break;
                case 4:
                    info.prAngle.x = std ::stof(value);
                    break;
                case 5:
                    info.prAngle.y = std ::stof(value);
                    break;
                case 6:
                    info.dutPrNearCoordinate.x = std::stof(value);
                    break;
                case 7:
                    info.dutPrNearCoordinate.y = std::stof(value);
                    break;
                case 8:
                    info.dutPrNearCoordinate.z = std::stof(value);
                    break;
                default:
                    break;
                }

            }     
            pinfo.append(info);

        }            
        Ainfo.insert((DUT_EYE_TYPE)0, pinfo);
        pinfo.clear();      
        if (!query.exec(selectSql2))
        {
            closeDatabase();
            QString logMsg = "select data from DutPrNearCalibrate table is failed:" + query.lastError().text() + ", " +
                             query.lastQuery();
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
            return Result(false, logMsg.toStdString());
        }
        int y = 0;
        while (query.next())
        {
            y++;

            for (int i = 0; i < query.record().count(); i++)
            {
                std::string value = query.record().value(i).toString().toStdString();
                switch (i)
                {
                case 0:
                    info.id = std ::stof(value);
                    break;
                case 1: {
                    type = std ::stof(value);
                    if (!type)
                        eyeType = DUT_EYE_LEFT;
                    else
                        eyeType = DUT_EYE_RIGHT;
                }
                break;
                case 2:
                    info.dutAngle.x = std ::stof(value);
                    break;
                case 3:
                    info.dutAngle.y = std ::stof(value);
                    break;
                case 4:
                    info.prAngle.x = std ::stof(value);
                    break;
                case 5:
                    info.prAngle.y = std ::stof(value);
                    break;
                case 6:
                    info.dutPrNearCoordinate.x = std::stof(value);
                    break;
                case 7:
                    info.dutPrNearCoordinate.y = std::stof(value);
                    break;
                case 8:
                    info.dutPrNearCoordinate.z = std::stof(value);
                    break;
                default:
                    break;
                }
            }
            pinfo.append(info);

        }    
         Ainfo.insert((DUT_EYE_TYPE)1, pinfo);
         pinfo.clear();
        closeDatabase();
    }
    else
    {
        return Result(false, "Select DutPrNearCalibrate table failed, open database failed.");
    }
    return Result();
}

Result calibrateDataBase::deleteRecord(int id)
{
    if (!openDatabase()){
        return Result(false, "Database open failed.");
    }

    QString sql = QString("DELETE FROM DutPrNearCalibrate WHERE id = %1").arg(id);
    QSqlQuery query(m_db);
    if (!query.exec(sql))
    {
        closeDatabase();
        QString logMsg =
            sql  + ", exec failed:" + query.lastError().text() + ", " +
                         query.lastQuery();
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, logMsg.toStdString().c_str());
        return Result(false, logMsg.toStdString());
    }
    closeDatabase();
    return Result();
}

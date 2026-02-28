#include "ReceiveDataThread.h"
#include "loggingwrapper.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QMessageBox>
#include <QCoreApplication>
#include "MqttConfig.h"

ReceiveDataThread::ReceiveDataThread(QObject*  parent)
	: QThread(parent)
{
	m_msgDateList.clear();
	m_msgFileList.clear();
	m_receivedMsgList.clear();
}

ReceiveDataThread::~ReceiveDataThread()
{
}

void ReceiveDataThread::run()
{
	while (!m_bStopThread)
	{
		QMutexLocker lock(&m_mutex);

		for (int i = 0; i < m_receivedMsgList.keys().size(); i++)
		{
			QString msgid = m_receivedMsgList.keys().at(i);
			QString rescode = m_receivedMsgList.value(msgid);
			if (m_msgDateList.keys().contains(msgid))
			{
				if ("20000" == rescode)
				{
					LoggingWrapper::instance()->info("update status success");
					emit signalUpdateUploadStatus(msgid, "success", rescode);
				}
				else
				{
					LoggingWrapper::instance()->error("update status error");
					emit signalUpdateUploadStatus(msgid, "failed", rescode);
					if(m_msgFileList.keys().contains(msgid))
					{
						QString filepath = m_msgFileList.value(msgid);
						// 把文件移动到NG目录
						QFile file(filepath);
						if (file.exists())
						{
							QString ngpath = filepath;
							ngpath.replace("OK", "NG");
							QFileInfo fileInfo(ngpath);
							QDir dir;
							if (!dir.exists(fileInfo.path()))
							{
								dir.mkpath(fileInfo.path());
							}
							file.rename(ngpath);
						}
					}
					m_iTotalErrorCount++;
				}
			}
		}
		m_receivedMsgList.clear();

		QMap<QString, QDateTime>::iterator it = m_msgDateList.begin();
		QDateTime current = QDateTime::currentDateTime();
		QStringList failedlist;
		MqttServerInfo info = MqttConfig::getInstance().getMqttConfig();
		while (it != m_msgDateList.end())
		{
			if (qAbs(it.value().secsTo(current)) > info.ResponseTimeOut)
			{
				LoggingWrapper::instance()->error("Data upload failed as per server response.");
				emit signalUpdateUploadStatus(it.key(), "failed", "Data upload failed as per server response.");
				failedlist.append(it.key());
				if (m_msgFileList.keys().contains(it.key()))
				{
					QString filepath = m_msgFileList.value(it.key());
					// 把文件移动到NG目录
					QFile file(filepath);
					if (file.exists())
					{
						QString ngpath = filepath;
						ngpath.replace("OK", "NG");
						QFileInfo fileInfo(ngpath);
						QDir dir;
						if (!dir.exists(fileInfo.path()))
						{
							dir.mkpath(fileInfo.path());
						}
						file.rename(ngpath);
					}
				}
				m_iTotalErrorCount++;
			}
			it++;
		}
		for (auto str : failedlist)
		{
			m_msgDateList.remove(str);
		}
		if (m_iTotalErrorCount > info.nalarmnumber)
		{
			QMetaObject::invokeMethod(qApp, [&]() {
				QMessageBox::question(
					nullptr,
					"Error",
					"The number of failed MES uploads has exceeded the set threshold. Please upload manually!",
					QMessageBox::Yes);},
				Qt::BlockingQueuedConnection);
			QProcess::execute("./UpLoadHistory.exe", QStringList());
		}
		msleep(1000);
	}
}

void ReceiveDataThread::setStopFlag(bool bStop)
{
	m_bStopThread = bStop;
}

void ReceiveDataThread::setMsgIdList(const QString& msg_id, const QString& filepath)
{
	QMutexLocker lock(&m_mutex);
	if (m_msgDateList.keys().contains(msg_id))
	{
		return;
	}
	QDateTime current = QDateTime::currentDateTime();
	m_msgDateList.insert(msg_id, current);
	m_msgFileList.insert(msg_id, filepath);
}

void ReceiveDataThread::slotReceiveMqttData(const QByteArray& message, const QMqttTopicName& topic)
{
	if (topic.name() == "data.device.aoi.reply")
	{
		QMutexLocker lock(&m_mutex);
		QJsonDocument jsonDoc = QJsonDocument::fromJson(message);
		if (!jsonDoc.isObject())
		{
			LoggingWrapper::instance()->error(QString("Invalid JSON message: %1").arg(QString(message)));
			return;
		}
		QString msgid = jsonDoc.object().value("msg_id").toString();
		LoggingWrapper::instance()->info(QString("Received msg id:%1").arg(msgid));

		QString resMsg = jsonDoc.object().value("res_code").toString();
		LoggingWrapper::instance()->info(QString("Received res_code: %1").arg(resMsg));
		m_receivedMsgList.insert(msgid, resMsg);
	}
}

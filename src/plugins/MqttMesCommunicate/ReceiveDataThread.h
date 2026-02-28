#pragma once

#include <QThread>
#include <QDateTime>
#include <QMap>
#include <QMutex>
#include "MqttControl.h"

class ReceiveDataThread : public QThread
{
	Q_OBJECT
public:
	ReceiveDataThread(QObject* parent = nullptr);
	~ReceiveDataThread();

	void run() override;
	void setStopFlag(bool bStop = false);
	void setMsgIdList(const QString& msg_id, const QString& filepath);	// save all send msg_id

signals:
	void signalUpdateUploadStatus(const QString& sn, const QString& newStatus, const QString& errorInfo);

public slots:
	void slotReceiveMqttData(const QByteArray& message, const QMqttTopicName& topic);

private:
	bool m_bStopThread{ false };
	QMutex m_mutex;
	QMap<QString, QDateTime> m_msgDateList;
	QMap<QString, QString> m_msgFileList;	// save send file path
	QMap<QString, QString> m_receivedMsgList;
	int m_iTotalErrorCount{ 0 };
};


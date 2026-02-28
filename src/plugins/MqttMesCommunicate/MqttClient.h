#pragma once

#include <QObject>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include "MqttControl.h"
#include <QtMqtt/qmqttclient.h>
#include <QtNetWork>

#include "MqttModel.h"
#include "Result.h"
#include "json.hpp"

using namespace nlohmann;

class ReceiveDataThread;

enum DeviceStatus
{
	Start = 1,
	Alarm = 4,
	Stop = 10
};

class MqttClient : public QObject
{
	Q_OBJECT
public:
	MqttClient(QObject* parent = nullptr);
	~MqttClient();

	bool isConnected();
	void setCallback(MqttModelCallback* callback);
	void subscribeTopic(QString topic);

	Result connectmqttserver();
	void disconnectFromHost();
	QStringList getSubscribeTopics() const { return m_subscribeTopics; }
	Result processSendMessage(std::string csvpath);
	Result processDeviceStatusMessage(const int& nStatus);
	Result sendHttpPostRequest();

private:
	void sleep(int msec);
	std::string aesEncrypt(const std::string& data);
	std::string aesDecrypt(const std::string& encryptedData);

signals:
	void signalConentDatabase();
	void signalAddUploadRecord(const QString& msg_id, const QString& sn, const QString& uploadTime, const QString& status, const QString& filePath, const QString& errinfo);

	void signalConnectMqtt(const QString& host, quint16 port = 1883);
	void signalDisconnectMqtt();
	void signalSubscribeTopic(const QString& topic, quint8 qos = 0);
	void signalPublishData(const QString& topic, const QByteArray& payload, quint8 qos = 0);
	
private slots:
	void onSlotReceiveMessage(const QByteArray& message, const QMqttTopicName& topic);
	bool onSlotconnectToDatabase();
	bool onSlotaddUploadRecord(const QString& msg_id, const QString& sn, const QString& uploadTime, const QString& status, const QString& filePath, const QString& errinfo);
	void updateUploadStatus(const QString& sn, const QString& newStatus, const QString& errorInfo = "");

private:
	MqttControl* m_pControl = nullptr;
	MqttModelCallback* m_callback = nullptr;
	QStringList m_subscribeTopics;	// 保存订阅的主题
	bool m_bIsReplyReceived = false;
	bool m_bIsSending = false;
	QString m_strErrorInfo;
	QMutex m_mutex;
	QSqlDatabase m_database;
	ReceiveDataThread* m_recThread{ nullptr };
};


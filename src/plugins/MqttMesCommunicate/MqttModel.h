#pragma once
#include <QObject>
#include "json.hpp"
#include "PrjCommon/service/ml_motion.h"

class MqttClient;

class MqttModelCallback
{
public:
	virtual void onMqttStateChanged(int nConnectState) = 0;
	virtual void onMqttMessageArrived(const std::string& topic, const std::string& message) = 0;
};

class MqttModel : public QObject, public MqttModelCallback
{
	Q_OBJECT
public:
	static MqttModel* getInstance();
	~MqttModel();
	Result connectMqttServer();
	void disconnectMqtt();
	bool isConnected();
	Result processSendMessage(std::string csvpath);
	QStringList getSubscribeTopics() const;
	Result processDeviceStatusMessage(const int& nStatus);
	Result sendHttpPostRequest();

	virtual void onMqttStateChanged(int nConnectState) override;
	virtual void onMqttMessageArrived(const std::string& topic, const std::string& message) override;

signals:
	void onSignalMqttStateChanged(int nConnectState);
	void onSignalMqttMessageArrived(const std::string& topic, const std::string& message);

private:
	MqttModel(QObject* parent = nullptr);
	
	static MqttModel* self;
	MqttClient* m_mqttClient = nullptr;
};


#pragma once

#include <QObject>
#include "json.hpp"

struct MqttServerInfo
{
	std::string BrokerIP;
	int BrokerPort;
	std::string ClientId;
	std::string Username;
	std::string Password;
	bool cleanSession = true;
	int ConnectTimeout = 5;
	int KeepAlive = 30;
	bool AutoReconnect = true;
	int ReconnectInterval = 5;
	int Qos = 1;
	std::string deviceno;
	std::string datatype;
	int ResponseTimeOut = 8;
	int ProtocolVersion = 4; // MQTT 3.1.1 e.g., 3 for MQTT 3.1 4 for MQTT 3.1.1, 5 for MQTT 5.0
	int nalarmnumber;
	std::map<std::string, std::string> TopicMap;
};

class MqttConfig : public QObject
{
	Q_OBJECT	
public:
	static MqttConfig& getInstance();
	MqttConfig();
	~MqttConfig();

	void loadConfig();
	MqttServerInfo getMqttConfig();

private:
	MqttServerInfo m_mqttServerInfo;
};


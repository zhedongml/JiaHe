#include "MqttConfig.h"
#include <fstream>
#include <QCoreApplication>

MqttConfig& MqttConfig::getInstance()
{
	static MqttConfig instance;
	return instance;
}

MqttConfig::MqttConfig()
{
	loadConfig();
}

MqttConfig::~MqttConfig()
{
}

void MqttConfig::loadConfig()
{
	nlohmann::ordered_json mqttConfig;

	std::ifstream ifs("./config/mesconfig.json", std::fstream::in);
	if (ifs.is_open())
	{
		ifs >> mqttConfig;
	}
	if (mqttConfig.is_null())
	{
		return;
	}
	m_mqttServerInfo.BrokerIP = mqttConfig["MQTT"]["BrokerIP"].get<std::string>();
	m_mqttServerInfo.BrokerPort = mqttConfig["MQTT"]["BrokerPort"].get<int>();
	m_mqttServerInfo.ClientId = mqttConfig["MQTT"]["ClientId"].get<std::string>();
	m_mqttServerInfo.Username = mqttConfig["MQTT"]["UserName"].get<std::string>();
	m_mqttServerInfo.Password = mqttConfig["MQTT"]["Password"].get<std::string>();
	m_mqttServerInfo.cleanSession = mqttConfig["MQTT"]["CleanSession"].get<bool>();
	m_mqttServerInfo.ConnectTimeout = mqttConfig["MQTT"]["ConnectionTimeout"].get<int>();
	m_mqttServerInfo.KeepAlive = mqttConfig["MQTT"]["KeepAliveInterval"].get<int>();
	m_mqttServerInfo.AutoReconnect = mqttConfig["MQTT"]["AutoReconnect"].get<bool>();
	m_mqttServerInfo.ReconnectInterval = mqttConfig["MQTT"]["ReconnectInterval"].get<int>();
	m_mqttServerInfo.Qos = mqttConfig["MQTT"]["Qos"].get<int>();
	m_mqttServerInfo.ResponseTimeOut = mqttConfig["MQTT"]["ResponseTimeOut"].get<int>();
	m_mqttServerInfo.ProtocolVersion = mqttConfig["MQTT"]["ProtocolVersion"].get<int>();
	m_mqttServerInfo.deviceno = mqttConfig["MQTT"]["DeviceNo"].get<std::string>();
	m_mqttServerInfo.datatype = mqttConfig["MQTT"]["DataType"].get<std::string>();
	m_mqttServerInfo.nalarmnumber = mqttConfig["MQTT"]["AlarmNumber"].get<int>();
	auto topics = mqttConfig["MQTT"]["Topic"];
	m_mqttServerInfo.TopicMap.clear();
	m_mqttServerInfo.TopicMap.insert(std::make_pair("AOI", topics["AOI"].get<std::string>()));
	m_mqttServerInfo.TopicMap.insert(std::make_pair("STATUS", topics["STATUS"].get<std::string>()));
}

MqttServerInfo MqttConfig::getMqttConfig()
{
	loadConfig();
	return m_mqttServerInfo;
}



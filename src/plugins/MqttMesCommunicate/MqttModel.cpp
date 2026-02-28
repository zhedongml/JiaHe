#include "MqttModel.h"
#include "MqttClient.h"
#include <QCoreApplication>

MqttModel* MqttModel::self = nullptr;
MqttModel* MqttModel::getInstance()
{
	if (!self)
	{
		self = new MqttModel();
	}
	return self;
}

MqttModel::MqttModel(QObject* parent)
	: QObject(parent)
{
	m_mqttClient = new MqttClient(this);
	m_mqttClient->setCallback(this);
}

MqttModel::~MqttModel()
{
	if(nullptr != m_mqttClient)
	{
		delete m_mqttClient;
		m_mqttClient = nullptr;
	}
}

Result MqttModel::connectMqttServer()
{
	if (nullptr == m_mqttClient)
	{
		m_mqttClient = new MqttClient(this);
		m_mqttClient->setCallback(this);
	}
	return m_mqttClient->connectmqttserver();
}

void MqttModel::disconnectMqtt()
{
	if (nullptr == m_mqttClient)
	{
		return;
	}
	m_mqttClient->disconnectFromHost();
}

bool MqttModel::isConnected()
{
	if (nullptr == m_mqttClient)
	{
		return false;
	}
	return m_mqttClient->isConnected();
}

Result MqttModel::processSendMessage(std::string csvpath)
{
	if (nullptr == m_mqttClient)
	{
		return Result(false, "Mqtt Not Initizial!");
	}
	return m_mqttClient->processSendMessage(csvpath);
}

QStringList MqttModel::getSubscribeTopics() const
{
	if (nullptr == m_mqttClient)
	{
		return QStringList();
	}
	return m_mqttClient->getSubscribeTopics();
}
Result MqttModel::processDeviceStatusMessage(const int& nStatus)
{
	if (nullptr == m_mqttClient)
	{
		return Result(false, "Mqtt Not Initizial!");
	}
	return m_mqttClient->processDeviceStatusMessage(nStatus);
}

Result MqttModel::sendHttpPostRequest()
{
	if (nullptr == m_mqttClient)
	{
		return Result(false, "Mqtt Not Initizial!");
	}
	return m_mqttClient->sendHttpPostRequest();
}

void MqttModel::onMqttStateChanged(int nConnectState)
{
	emit onSignalMqttStateChanged(nConnectState);
}

void MqttModel::onMqttMessageArrived(const std::string& topic, const std::string& message)
{
	emit onSignalMqttMessageArrived(topic, message);
}

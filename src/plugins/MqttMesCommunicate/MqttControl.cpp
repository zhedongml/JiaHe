#include "MqttControl.h"
#include <QDebug>
#include "globalloggermanager.h"
#include "MqttConfig.h"

MqttControl::MqttControl(QObject* parent)
{
    connect(&m_client, &QMqttClient::stateChanged, this, &MqttControl::onMqttStateChanged);
    connect(&m_client, &QMqttClient::messageReceived, this, &MqttControl::messageReceived);
}

MqttControl::~MqttControl()
{
    if (m_isConnected)
    {
        m_client.disconnectFromHost();
    }
}

void MqttControl::onSlotConnectToHost(const QString& host, quint16 port)
{
    if (m_isConnected)
        return;
    
	MqttServerInfo info = MqttConfig::getInstance().getMqttConfig();
    m_isConnected = false;
    m_client.setHostname(QString::fromStdString(info.BrokerIP));
    m_client.setPort(info.BrokerPort);
    m_client.setClientId(QString::fromStdString(info.ClientId));
    m_client.setProtocolVersion(QMqttClient::MQTT_3_1_1);
    m_client.setAutoKeepAlive(info.AutoReconnect);
	m_client.setCleanSession(info.cleanSession);
    m_client.connectToHost();
}

void MqttControl::onSlotDisconnectFromHost()
{
    if (!m_isConnected)
        return;
    m_client.disconnectFromHost();
}

void MqttControl::onSlotPublish(const QString& topic, const QByteArray& payload, quint8 qos)
{
    if (!m_isConnected)
        return;
    m_client.publish(QMqttTopicName(topic), payload, qos);
}

void MqttControl::onSlotSubscribe(const QString& topic, quint8 qos)
{
    if (!m_isConnected)
        return;

	auto sub = m_client.subscribe(QMqttTopicFilter(topic), qos);
    connect(sub, &QMqttSubscription::stateChanged, this, [=](QMqttSubscription::SubscriptionState state) {
        if (state == QMqttSubscription::Subscribed) {
			m_bIsSubscribed = true;
        } else if (state == QMqttSubscription::Unsubscribed) {
            m_bIsSubscribed = false;
        } else if (state == QMqttSubscription::Error) {
            m_bIsSubscribed = false;
        }
		});
}

void MqttControl::onSlotUnsubscribe(const QString& topic)
{
    if (!m_isConnected)
        return;
    m_client.unsubscribe(topic);
}

void MqttControl::onMqttStateChanged(QMqttClient::ClientState state)
{
    switch (state)
    {
    case QMqttClient::Connected:
        m_isConnected = true;
        STATUS_LOG("connect mqtt server success");
        break;
    case QMqttClient::Disconnected:
        m_isConnected = false;
		STATUS_LOG("disconnect mqtt server");
        break;
    default:
        break;
    }
}
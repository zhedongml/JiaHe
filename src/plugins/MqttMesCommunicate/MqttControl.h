#ifndef MQTTCONTROL_H
#define MQTTCONTROL_H

#include <QObject>
#include "QtMqtt/qmqttclient.h"

class MqttControl : public QObject
{
    Q_OBJECT

public:
    explicit MqttControl(QObject* parent = nullptr);
    ~MqttControl();
    bool isConnected() const { return m_isConnected; }
	bool isSubscribed() const { return m_bIsSubscribed; }

public slots:
    void onSlotConnectToHost(const QString& host, quint16 port = 1883);
    void onSlotDisconnectFromHost();
    void onSlotPublish(const QString& topic, const QByteArray& payload, quint8 qos = 0);
    void onSlotSubscribe(const QString& topic, quint8 qos = 0);
    void onSlotUnsubscribe(const QString& topic);

signals:
    void messageReceived(const QByteArray& message, const QMqttTopicName& topic = QMqttTopicName());

private slots:
    void onMqttStateChanged(QMqttClient::ClientState state);

private:
    QMqttClient m_client;
    bool m_isConnected = false;
	bool m_bIsSubscribed = false;
	//bool m_bIsSendSuccess = false;
};

#endif // MQTTCONTROL_H
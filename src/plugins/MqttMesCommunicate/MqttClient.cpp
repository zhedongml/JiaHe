#include "MqttClient.h"
#include "MqttModel.h"
#include "loggingwrapper.h"
#include "qaesencryption.h"
#include "MqttConfig.h"
#include "ReceiveDataThread.h"
#include <fstream>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "globalloggermanager.h"

static const QByteArray AESKey = "sjgd2023gyhlwiot";

MqttClient::MqttClient(QObject* parent)
	: QObject(parent)
{
	m_pControl = new MqttControl(this);
	qRegisterMetaType<QMqttClient::ClientState>("ClientState");
	qRegisterMetaType<ordered_json>("ordered_json");
	qRegisterMetaType<std::string>("std::string");
	
	connect(this, &MqttClient::signalConnectMqtt, m_pControl, &MqttControl::onSlotConnectToHost);
	connect(this, &MqttClient::signalDisconnectMqtt, m_pControl, &MqttControl::onSlotDisconnectFromHost);
	connect(this, &MqttClient::signalSubscribeTopic, m_pControl, &MqttControl::onSlotSubscribe);
	connect(this, &MqttClient::signalPublishData, m_pControl, &MqttControl::onSlotPublish);
	connect(m_pControl, &MqttControl::messageReceived, this, &MqttClient::onSlotReceiveMessage);

	connect(this, &MqttClient::signalConentDatabase, this, &MqttClient::onSlotconnectToDatabase);
	connect(this, &MqttClient::signalAddUploadRecord, this, &MqttClient::onSlotaddUploadRecord, Qt::QueuedConnection);

	m_recThread = new ReceiveDataThread();
	connect(m_pControl, &MqttControl::messageReceived, m_recThread, &ReceiveDataThread::slotReceiveMqttData);
	connect(m_recThread, &ReceiveDataThread::signalUpdateUploadStatus, this, &MqttClient::updateUploadStatus);
	m_recThread->start();
	
	emit signalConentDatabase();

	// init log
	GlobalLoggerManager::initialize("D:/mes/log/mqtt");
	GlobalLoggerManager::create_logger("mqtt");
	GlobalLoggerManager::create_logger("status");
}

MqttClient::~MqttClient()
{
	if (m_pControl)
	{
		delete m_pControl;
		m_pControl = nullptr;
	}
	if (m_database.isOpen())
	{
		m_database.close();
	}
	if (m_recThread)
	{
		m_recThread->setStopFlag(true);
		delete m_recThread;
		m_recThread = nullptr;
	}
	GlobalLoggerManager::shutdown();
}

bool MqttClient::isConnected()
{
	if (m_pControl)
	{
		return m_pControl->isConnected();
	}
	return false;
}

void MqttClient::setCallback(MqttModelCallback* callback)
{
	m_callback = callback;
}

bool MqttClient::onSlotconnectToDatabase()
{
	m_database = QSqlDatabase::addDatabase("QSQLITE");
	m_database.setDatabaseName("mydatabase.db");
	if (!m_database.open())
	{
		LoggingWrapper::instance()->error("Failed to connect to database:" + m_database.lastError().text());
		return false;
	}

	QSqlQuery query;
	QString createTableSQL = "CREATE TABLE IF NOT EXISTS uploads("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"msg_id TEXT, "
		"SN TEXT, "
		"uploadTime TEXT, "
		"status TEXT,"
		"filePath TEXT UNIQUE,"
		"errorinfo TEXT)";

	if (!query.exec(createTableSQL))
	{
		LoggingWrapper::instance()->error("Failed to create table:" + query.lastError().text());
		return false;
	}
	return true;
}

bool MqttClient::onSlotaddUploadRecord(const QString& msg_id, const QString& sn, const QString& uploadTime, const QString& status, const QString& filePath, const QString& errinfo)
{
	if (m_database.isOpen() == false)
	{
		if (!m_database.open())
		{
			LoggingWrapper::instance()->error("Database is not open and failed to open it:" + m_database.lastError().text());
			return false;
		}
	}
	QSqlQuery query;
	query.prepare("INSERT INTO uploads (msg_id, SN, uploadTime, status, filePath, errorinfo) VALUES (?, ?, ?, ?, ?, ?)");
	query.addBindValue(msg_id);
	query.addBindValue(sn);
	query.addBindValue(uploadTime);
	query.addBindValue(status);
	query.addBindValue(filePath);
	query.addBindValue(errinfo);

	if (!query.exec())
	{
		LoggingWrapper::instance()->error("Failed to add upload record:" + query.lastError().text());
		return false;
	}
	return true;
}

void MqttClient::updateUploadStatus(const QString& sn, const QString& newStatus, const QString& errorInfo)
{
	QString query = QString("UPDATE uploads SET status = '%1', errorinfo = '%2' WHERE msg_id = '%3'")
		.arg(newStatus, errorInfo, sn);
	QSqlQuery sqlQuery(m_database);
	sqlQuery.prepare(query);
	if (!sqlQuery.exec())
	{
		LoggingWrapper::instance()->error("Query execution failed:" + sqlQuery.lastError().text());
	}
}

void MqttClient::subscribeTopic(QString topic)
{
	if (!m_pControl || !m_pControl->isConnected() || m_subscribeTopics.contains(topic))
	{
		return;
	}

	emit signalSubscribeTopic(topic, 1);
	int nWaitCount = 0;
	while (nWaitCount < 10)
	{
		if(m_pControl->isSubscribed())
		{
			break;
		}
		sleep(100);
		nWaitCount++;
	}
	if(!m_pControl->isSubscribed())
	{
		LoggingWrapper::instance()->error("Mqtt Subscribe Topic Failed!");
		return;
	}
	m_subscribeTopics.append(topic);
}

Result MqttClient::connectmqttserver()
{
	if (!m_pControl)
	{
		return Result(false, "Mqtt Not Initizial!");
	}
	if(m_pControl->isConnected())
	{
		return Result();
	}
	MqttServerInfo info = MqttConfig::getInstance().getMqttConfig();
	emit signalConnectMqtt(QString::fromStdString(info.BrokerIP), info.BrokerPort);

	int nWaitCount = 0;
	while (nWaitCount < 10)
	{
		if(m_pControl->isConnected())
		{
			break;
		}
		sleep(100);
		nWaitCount++;
	}
	if(!m_pControl->isConnected())
	{
		LoggingWrapper::instance()->error("Mqtt Connect Failed!");
		return Result(false, "Mqtt Connect Failed!");
	}
	subscribeTopic("data.device.aoi.reply");
	if(m_callback)
	{
		m_callback->onMqttStateChanged(m_pControl->isConnected() ? 2 : 1);
	}
	return Result();
}

void MqttClient::disconnectFromHost()
{
	if (m_pControl)
	{
		emit signalDisconnectMqtt();
	}
}

Result MqttClient::processSendMessage(std::string csvpath)
{
	QMutexLocker locker(&m_mutex);
	MqttServerInfo info = MqttConfig::getInstance().getMqttConfig();
	QString strCsvPath = "D:/20250826_MetricsTest/12344_20250826T115000/metrics_135.csv.csv";
	QFile file(strCsvPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		LoggingWrapper::instance()->error("Open csv file failed!");
		return Result(false, "Open csv file failed!");
	}
	QTextStream in(&file);
	// 读取第一行和第四行数据
	QString headerLine = in.readLine();
	QStringList headers = headerLine.split(",");
	QString fourthLine;
	for (int i = 0; i < 3; ++i)
	{
		if (!in.atEnd())
			fourthLine = in.readLine();
		else
		{
			LoggingWrapper::instance()->error("CSV file has less than 4 lines!");
			return Result(false, "CSV file has less than 4 lines!");
		}
	}
	QStringList values = fourthLine.split(",");
	file.close();

	// 构建JSON数据
	QDateTime currentdata = QDateTime::currentDateTime();
	nlohmann::ordered_json jsonData;
	QString strMsgId = QString("aoi-%1-%2").arg(MqttConfig::getInstance().getMqttConfig().deviceno.c_str()).arg(currentdata.toMSecsSinceEpoch());
	jsonData["msg_id"] = strMsgId.toStdString();
	jsonData["device_no"] = MqttConfig::getInstance().getMqttConfig().deviceno;
	jsonData["datatype"] = "aoi";
	jsonData["deviceType"] = "data.device.aoi.reply";

	// 创建data对象
	nlohmann::ordered_json dataObject;
	//dataObject["FILE_ID"] = strCsvPath.toStdString();
	//dataObject["SERIVALNUMBER"] = values.size() > 0 ? values[0].toStdString() : "";
	//dataObject["LINE_ID"] = values.size() > 1 ? values[1].toStdString() : "";
	//dataObject["TESTER_NAME"] = values.size() > 2 ? values[2].toStdString() : "";
	//dataObject["APPRAISER"] = values.size() > 3 ? values[3].toStdString() : "";
	//dataObject["TEST_START_TIME"] = values.size() > 5 ? (values[4].toStdString() + "T" + values[5].toStdString()) : "";
	//dataObject["TEST_END_TIME"] = currentdata.toString("yyyy-MM-ddTHH:mm:ss").toStdString();
	//dataObject["TEST_MODE"] = values.size() > 6 ? values[6].toStdString() : "";
	//dataObject["AUDIT_MODE"] = values.size() > 7 ? values[7].toStdString() : "";
	//dataObject["TEST_SW"] = values.size() > 8 ? values[8].toStdString() : "";
	//dataObject["TEST_STATION"] = values.size() > 9 ? values[9].toStdString() : "";
	//dataObject["TEST_RESULT"] = values.size() > 10 ? values[10].toStdString() : "";
	//dataObject["TEST_FAIL_ITEM"] = values.size() > 11 ? values[11].toStdString() : "";
	//dataObject["ERROR_CODE"] = values.size() > 12 ? values[12].toStdString() : "";
	//dataObject["TEST_TOTAL_TIME"] = values.size() > 13 ? values[13].toStdString() : "";
	dataObject["FILE_ID"] = strCsvPath.toStdString();
	dataObject["FLOW_QR_CODE"] = values.size() > 0 ? values[0].toStdString() : "";				// SERIVALNUMBER
	dataObject["FEED_PALLET_SERIAL"] = values.size() > 1 ? values[1].toStdString() : "";		// Line_ID
	dataObject["O_OPERATOR"] = values.size() > 2 ? values[2].toStdString() : "";				// TESTER_NAME
	dataObject["FEED_SERIAL"] = values.size() > 3 ? values[3].toStdString() : "";				// APPRAISER
	dataObject["TEST_START_TIME"] = values.size() > 5 ? (values[4].toStdString() + "T" + values[5].toStdString()) : "";
	dataObject["TEST_END_TIME"] = currentdata.toString("yyyy-MM-ddTHH:mm:ss").toStdString();
	dataObject["QR_GRADE"] = values.size() > 6 ? values[6].toStdString() : "";					// TEST_MODE
	dataObject["LOCATION"] = values.size() > 7 ? values[7].toStdString() : "";					// AUDIT_MODE
	dataObject["O_WORKSTATION"] = values.size() > 8 ? values[8].toStdString() : "";				// TEST_SW
	dataObject["MACHINE_NO"] = values.size() > 9 ? values[9].toStdString() : "";				// TEST_STATION
	dataObject["TEST_ITEM_RESULT"] = values.size() > 10 ? values[10].toStdString() : "";		// TEST_RESULT
	dataObject["TEST_ITEM_FAULTY"] = values.size() > 11 ? values[11].toStdString() : "";		// TEST_FAIL_ITEM
	dataObject["SCAN_NO"] = values.size() > 12 ? values[12].toStdString() : "";					// ERROR_CODE
	dataObject["MOUTH_NO"] = values.size() > 13 ? values[13].toStdString() : "";				// TEST_TOTAL_TIME
	dataObject["FLOW_CARD_ID"] = "";
	dataObject["FEED_PALLET_QR_CODE"] = "";
	dataObject["COLLECT_PALLET_SERIAL"] = -1;
	dataObject["COLLECT_BOX_CODE"] = "";
	dataObject["COLLECT_SERIAL"] = 2;
	dataObject["PORRO_NO"] = 1;
	dataObject["VEHICLE_CODE"] = "";
	dataObject["DOWN_MOUTH_NO"] = "";

	// 创建ITEM_LIST数组
	nlohmann::ordered_json itemList = nlohmann::ordered_json::array();
	for (int i = 14; i < values.size() && i < headers.size(); i++)
	{
		nlohmann::ordered_json item;
		item["TEST_ITEM_NAME"] = headers[i].toStdString();
		item["TEST_ITEM_VALUE"] = values[i].toStdString();
		item["TEST_ITEM_RESULT"] = "ok";
		itemList.push_back(item);
	}
	dataObject["ITEM_LIST"] = itemList;

	// 将data对象赋值给jsonData
	jsonData["data"] = dataObject;

	// 在csv路径下保存json文件用于后面的调试
	QString strJsonPath = strCsvPath.remove(".csv") + ".json";
	std::ofstream jsonFile(strJsonPath.toStdString(), std::ios::out);
	if (jsonFile.is_open())
	{
		jsonFile << jsonData.dump(4);
		jsonFile.close();
	}
	else
	{
		LoggingWrapper::instance()->error("Failed to create JSON file for writing!");
	}
	MQTT_LOG("Prepared JSON Data: {}", QString::fromStdString(jsonData.dump(4)).toStdString());

	// 将json里面的data数据用AES加密
	jsonData["data"] = aesEncrypt(jsonData["data"].dump(4));
	QDir sendDir(QString("D:/mes/log/mqtt/%1/%2/OK/").arg(currentdata.toString("yyyyMMdd")).arg(currentdata.toString("yyyyMMddHH")));
	if(!sendDir.exists())
	{
		sendDir.mkpath(sendDir.absolutePath());
	}
	QFile sendFile(sendDir.absolutePath() + "/NULL_NULL_" + QString::number(currentdata.toMSecsSinceEpoch()) + ".txt");
	if(sendFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&sendFile);
		out << QString::fromStdString(jsonData.dump(4));
		sendFile.close();
	}
	else
	{
		LoggingWrapper::instance()->error("Failed to create mqtt file for writing!");
	}

	// 发送MQTT消息
	emit signalPublishData(QString::fromStdString(info.TopicMap.find("AOI")->second), QByteArray::fromStdString(jsonData.dump(4)), 1);

	emit signalAddUploadRecord(strMsgId,
		values.size() > 0 ? values[0] : "",
		currentdata.toString("yyyy-MM-dd HH:mm:ss"),
		"failed", strJsonPath, "first post");

	m_recThread->setMsgIdList(strMsgId, sendDir.absolutePath() + "/NULL_NULL_" + QString::number(currentdata.toMSecsSinceEpoch()) + ".txt");
	return Result();

#if 0
	Result res;
	// 循环判断是否接收到回复消息
	m_bIsReplyReceived = false;
	m_bIsSending = false;
	m_strErrorInfo = "";
	int waitCount = 0;
	bool breaked = false;
	while (waitCount < 8) // 最多等待8秒
	{
		if (m_bIsReplyReceived)
		{
			breaked = true;
			LoggingWrapper::instance()->info("Received reply message from MQTT server.");
			break;
		}
		QCoreApplication::processEvents();
		sleep(1000); // 每秒检查一次
		waitCount++;
	}
	if (breaked)
	{
		if (m_bIsSending)
		{
			LoggingWrapper::instance()->info("Data upload confirmed by server.");
			emit signalAddUploadRecord(strMsgId,
				values.size() > 0 ? values[0] : "",
				currentdata.toString("yyyy-MM-dd HH:mm:ss"),
				"success", strJsonPath, m_strErrorInfo);
			res.success = true;
		}
		else
		{
			LoggingWrapper::instance()->error("Data upload failed as per server response.");
			emit signalAddUploadRecord(strMsgId,
				values.size() > 0 ? values[0] : "",
				currentdata.toString("yyyy-MM-dd HH:mm:ss"),
				"failed", strJsonPath, m_strErrorInfo);
			res.success = false;
			res.errorMsg = m_strErrorInfo.toStdString();
		}
	}
	else
	{
		LoggingWrapper::instance()->error("Timeout waiting for reply message from MQTT server.");
		emit signalAddUploadRecord(strMsgId,
			values.size() > 0 ? values[0] : "",
			currentdata.toString("yyyy-MM-dd HH:mm:ss"),
			"failed", strJsonPath, "Server Not Responding");
		res.success = false;
		res.errorMsg = "Server Not Responding";
	}
	return res;
#endif
}

Result MqttClient::processDeviceStatusMessage(const int& nStatus)
{
	QMutexLocker locker(&m_mutex);
	MqttServerInfo info = MqttConfig::getInstance().getMqttConfig();
	QString strStatus;
	switch (nStatus)
	{
	case DeviceStatus::Start:
		strStatus = "RUN";
		break;
	case DeviceStatus::Alarm:
		strStatus = "UDT";
		break;
	case DeviceStatus::Stop:
		strStatus = "BREAK";
		break;
	default:
		strStatus = "UNKNOWN";
		break;
	}
	QDateTime currentdata = QDateTime::currentDateTime();
	nlohmann::ordered_json jsonData;
	// TODO 修改为实际的设备编号
	jsonData["msg_id"] = QString("aoi-%1-%2").arg("").arg(currentdata.toString("yyyyMMddHHmmsszzz")).toStdString();
	jsonData["device_no"] = "RQ202500001";
	jsonData["dataType"] = "RunData";
	jsonData["deviceType"] = "";

	nlohmann::ordered_json dataObject;
	dataObject["o_workStation"] = "AOI1";
	dataObject["o_machineNo"] = "RQ202500001";
	dataObject["o_machineState"] = nStatus;
	dataObject["o_stateName"] = strStatus.toStdString();
	dataObject["o_stateStartTime"] = currentdata.toString("yyyy-MM-dd HH:mm:ss").toStdString();
	dataObject["o_alertCode"] = "";
	dataObject["o_alertInfo"] = "";

	jsonData["data"] = dataObject;
	MQTT_LOG("Prepared Device Status JSON Data: {}", QString::fromStdString(jsonData.dump(4)).toStdString());

	jsonData["data"] = aesEncrypt(jsonData["data"].dump(4));
	// 发送MQTT消息
	emit signalPublishData(QString::fromStdString(info.TopicMap.find("STATUS")->second), QByteArray::fromStdString(jsonData.dump(4)), 1);
	return Result();
}

Result MqttClient::sendHttpPostRequest()
{
	QNetworkAccessManager* manager = new QNetworkAccessManager(this);
	QNetworkRequest request(QUrl("http://127.0.0.1:5000/api/response"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	QJsonObject json;
	json["msg_id"] = "test-msg-id";
	json["status"] = "success";
	QJsonDocument doc(json);
	QByteArray data = doc.toJson();
	QNetworkReply* reply = manager->post(request, data);

	QEventLoop loop;
	connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();

	QJsonObject responseJson;
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray responseData = reply->readAll();
		QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
		if (responseDoc.isObject())
		{
			responseJson = responseDoc.object();
			LoggingWrapper::instance()->info("HTTP POST request successful. Response: " + QString(responseData));
			QString message = responseJson.value("message").toString();
			LoggingWrapper::instance()->info("Response message: " + message);
		}
		else
		{
			LoggingWrapper::instance()->error("Invalid JSON response from server.");
			return Result(false, "Invalid JSON response from server.");
		}
	}
	else
	{
		LoggingWrapper::instance()->error("HTTP POST request failed: " + reply->errorString());
		return Result(false, "HTTP POST request failed: " + reply->errorString().toStdString());
	}
	reply->deleteLater();
	manager->deleteLater();

	return Result();
}

void MqttClient::sleep(int msec)
{
	QDateTime dieTime = QDateTime::currentDateTime().addMSecs(msec);
	while (QDateTime::currentDateTime() < dieTime)
	{
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}
}

std::string MqttClient::aesEncrypt(const std::string& data)
{
	QAESEncryption encryption(QAESEncryption::AES_128, QAESEncryption::ECB, QAESEncryption::PKCS7);
	QByteArray encryptedData = encryption.encode(QByteArray::fromStdString(data), AESKey);
	return encryptedData.toBase64().toStdString();
}

std::string MqttClient::aesDecrypt(const std::string& encryptedData)
{
	QAESEncryption decryption(QAESEncryption::AES_128, QAESEncryption::ECB, QAESEncryption::PKCS7);
	QByteArray decryptedData = decryption.decode(QByteArray::fromBase64(QByteArray::fromStdString(encryptedData)), AESKey);
	QByteArray removedNulls = decryption.removePadding(decryptedData);
	return removedNulls.toStdString();
}

void MqttClient::onSlotReceiveMessage(const QByteArray& message, const QMqttTopicName& topic)
{
	MQTT_LOG("Received MQTT Message on topic {}: {}", topic.name().toStdString(), QString(message).toStdString());
	return;
	// receiv json file
	// 判断是哪个主题的消息
	if (topic.name() == "data.device.aoi.reply")
	{
		QJsonDocument jsonDoc = QJsonDocument::fromJson(message);
		if (!jsonDoc.isObject())
		{
			LoggingWrapper::instance()->error(QString("Invalid JSON message: %1").arg(QString(message)));
			return;
		}

		QString resMsg = jsonDoc.object().value("res_code").toString();
		LoggingWrapper::instance()->info(QString("Received response message: %1").arg(resMsg));
		if (resMsg == "20000")
		{
			m_bIsSending = true;
			LoggingWrapper::instance()->info("Data upload successful.");
		}
		else
		{
			m_bIsSending = false;
			LoggingWrapper::instance()->error(QString("Data upload failed: %1").arg(resMsg));
		}
		m_strErrorInfo = resMsg;
		m_bIsReplyReceived = true;
	}

	if (m_callback)
		m_callback->onMqttMessageArrived(topic.name().toStdString(), message.toStdString());

}
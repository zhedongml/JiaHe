#pragma once

#include <json.hpp>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QJsonArray>
#include <QEventLoop>
#include <Result.h>
#include "mesandfileload_global.h"
#include "MesCommon.h"

class MESANDFILELOAD_EXPORT MyHTTPClient : public QObject
{
	Q_OBJECT

public:
	explicit MyHTTPClient(QObject* parent = nullptr);
	~MyHTTPClient();

	//Result setRequestHeader(const QString& headerName, const QString& headerValue);

	Result sendPostRequest(SingleLensUpResponseData& responseDatas, const SingleLensUpRequestData& data);
	Result sendPostRequest(SingleLensDownResponseData& responseDatas, const SingleLensDownRequestData& data);
	Result sendPostRequest(SingleLensDownResponseData& responseDatas, const VehicledesignRequestData& data);

private:
	Result sendTest();

signals:
	void configUpdated(const ControlData& data);

private slots:
	void onReplyFinished(QNetworkReply* reply);

private:
	QNetworkAccessManager* networkManager;
};


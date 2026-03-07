#include "MyHTTPClient.h"
#include "MesConfig.h"
#include <QTimer>
#include <QNetworkProxy>

MyHTTPClient::MyHTTPClient(QObject* parent) :QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
    //QNetworkProxy proxy(QNetworkProxy::HttpProxy, "proxy_address", 8080); 
    //networkManager->setProxy(proxy);
    //connect(networkManager, &QNetworkAccessManager::finished, this, &MyHTTPClient::onReplyFinished);
}

MyHTTPClient::~MyHTTPClient()
{
}

//Result MyHTTPClient::setRequestHeader(const QString& headerName, const QString& headerValue)
//{
//    // 设置请求头
//    request.setRawHeader(headerName.toUtf8(), headerValue.toUtf8());
//    return Result();
//}

Result MyHTTPClient::sendPostRequest(SingleLensUpResponseData& responseDatas, const SingleLensUpRequestData& data)
{
    //if (!QSslSocket::supportsSsl()) {
    //    return Result(false, "Send single lens up request data error, SSL not supported!");
    //}

    QString urlStr = MesConfig::instance().getSingleLensLoadURL();

    if (urlStr.isEmpty()) {
        return Result(false, "Send single lens up request data error,, Request URL is not set!");
    }

    QNetworkRequest request;
    request.setUrl(QUrl(urlStr));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString mesToken = MesConfig::instance().getMesToken();
    //mesToken = "fG@5t2kH%jU7pQ6w&sR3eD8oX";
    request.setRawHeader("mes-token", mesToken.toUtf8());

    QJsonDocument doc(data.toJson());
    QByteArray byteArray = doc.toJson();

    QNetworkReply* reply = networkManager->post(request, byteArray);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec(); 

    QJsonObject response;
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        if (!jsonResponse.isNull()) {
            response = jsonResponse.object();
            responseDatas = SingleLensUpResponseData::fromJson(response);
        }
    }
    else {
        QString msg = reply->errorString();
        return Result(false, msg.toStdString());
    }
    reply->deleteLater();
    return Result();
}

Result MyHTTPClient::sendPostRequest(SingleLensDownResponseData& responseDatas, const SingleLensDownRequestData& data)
{
    QString urlStr = MesConfig::instance().getSingleLensUnloadURL();
    QNetworkRequest request;
    request.setUrl(QUrl(urlStr));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString mesToken = MesConfig::instance().getMesToken();
    //mesToken = "fG@5t2kH%jU7pQ6w&sR3eD8oX";
    request.setRawHeader("mes-token", mesToken.toUtf8());

    QJsonDocument doc(data.toJson());
    QByteArray byteArray = doc.toJson();

    QNetworkReply* reply = networkManager->post(request, byteArray);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QJsonObject response;
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        if (!jsonResponse.isNull()) {
            response = jsonResponse.object();
            responseDatas = SingleLensDownResponseData::fromJson(response);
        }
    }
    else {
        QString msg = reply->errorString();
        return Result(false, msg.toStdString());
    }
    reply->deleteLater();
    return Result();
}

Result MyHTTPClient::sendPostRequest(SingleLensDownResponseData& responseDatas, const VehicledesignRequestData& data)
{
    QString urlStr = MesConfig::instance().getCuttingPreventURL();
    if (urlStr.isEmpty()) {
        return Result(false, "Send Cutting Prevent request data error,, Request URL is not set!");
    }

    QNetworkRequest request;
    request.setUrl(QUrl(urlStr));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString mesToken = MesConfig::instance().getMesToken();
    //mesToken = "fG@5t2kH%jU7pQ6w&sR3eD8oX";
    request.setRawHeader("mes-token", mesToken.toUtf8());

    QJsonDocument doc(data.toJson());
    QByteArray byteArray = doc.toJson();

    QNetworkReply* reply = networkManager->post(request, byteArray);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QJsonObject response;
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        if (!jsonResponse.isNull()) {
            response = jsonResponse.object();
            responseDatas = SingleLensDownResponseData::fromJson(response);
        }
    }
    else {
        QString msg = reply->errorString();
        return Result(false, msg.toStdString());
    }
    reply->deleteLater();
    return Result();
}

void MyHTTPClient::onReplyFinished(QNetworkReply* reply)
{
    // 读取响应数据
     if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Response:" << reply->readAll();
    }
    else {
        QString msg = reply->errorString();
        qWarning() << "Response Headers:" << reply->rawHeaderPairs();
        qWarning() << "Response Data:" << reply->readAll();
        qWarning() << "Error:" << reply->errorString();
    }
    reply->deleteLater();
}

Result MyHTTPClient::sendTest()
{
    if (!QSslSocket::supportsSsl()) {
        return Result(false, "SSL not supported!");
    }

    //QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    //sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);  // 禁用 SSL 验证

    QUrl url("https://3b8c4d43-ca45-4525-9cc7-f1203f04e6f5.mock.pstmn.io/test");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //request.setSslConfiguration(sslConfig);  // 在请求之前设置 SSL 配置

    QJsonObject json;
    json["qq"] = 33;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = networkManager->post(request, data);
    //QTimer::singleShot(10000, reply, &QNetworkReply::abort);  // 设置10秒超时
    return Result();
}


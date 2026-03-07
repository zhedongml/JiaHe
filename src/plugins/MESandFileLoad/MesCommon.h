#pragma once
#include <QMap>
#include <QList>
#include <json.hpp>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QJsonArray>

using Json = nlohmann::json;

// 封装JSON数据结构体
struct SingleLensUpRequestData {
	QString id;
	QString lenscode;
	QString ADcode;
	QString WGcode;
	QString mac;

	// 转换为JSON格式
	QJsonObject toJson() const {
		QJsonObject jsonObj;
		jsonObj["id"] = id;
		jsonObj["lenscode"] = lenscode;
		jsonObj["ADcode"] = ADcode;
		jsonObj["WGcode"] = WGcode;
		jsonObj["mac"] = mac;
		return jsonObj;
	}

	QString toStr() const{
		QJsonObject jsonObject = toJson();
		QJsonDocument doc(jsonObject);
		return doc.toJson(QJsonDocument::Compact);
	}
};

struct SingleLensUpResponseData {
	bool success;
	QString message;
	qint64 timestamp;
	int code;
	QString judgment;

	static SingleLensUpResponseData fromJson(const QJsonObject& jsonObj) {
		SingleLensUpResponseData response;
		response.success = jsonObj["success"].toBool();
		response.message = jsonObj["message"].toString();
		response.timestamp = jsonObj["timestamp"].toVariant().toLongLong();
		response.code = jsonObj["code"].toInt();
		response.judgment = jsonObj["judgment"].toString();
		return response;
	}

	QJsonObject toJson() const {
		QJsonObject jsonObj;
		jsonObj["success"] = success;
		jsonObj["message"] = message;
		jsonObj["timestamp"] = timestamp;
		jsonObj["code"] = code;
		jsonObj["judgment"] = judgment;
		return jsonObj;
	}

	QString toStr() const {
		QJsonObject jsonObject = toJson();
		QJsonDocument doc(jsonObject);
		return doc.toJson(QJsonDocument::Compact);
	}
};

struct SingleLensDownRequestData {
	QString id;
	QString dvehiclecode;
	QStringList cbpcode;
	QString cstatus;
	QString qualifiedtype;
	QString rbfxcode;
	QString rbfxvalue;
	QString lenscode;
	QString ADcode;
	QString WGcode;
	QString judgment;
	QString mac;

	// 转换为JSON格式
	QJsonObject toJson() const {
		QJsonObject jsonObj;
		jsonObj["id"] = id;
		jsonObj["dvehiclecode"] = dvehiclecode;
		jsonObj["cbpcode"] = QJsonArray::fromStringList(cbpcode);
		jsonObj["cstatus"] = cstatus;
		jsonObj["qualifiedtype"] = qualifiedtype;
		jsonObj["rbfxcode"] = rbfxcode;
		jsonObj["rbfxvalue"] = rbfxvalue;
		jsonObj["lenscode"] = lenscode;
		jsonObj["ADcode"] = ADcode;
		jsonObj["WGcode"] = WGcode;
		jsonObj["judgment"] = judgment;
		jsonObj["mac"] = mac;
		return jsonObj;
	}

	QString toStr() const {
		QJsonObject jsonObject = toJson();
		QJsonDocument doc(jsonObject);
		return doc.toJson(QJsonDocument::Compact);
	}
};

// 单镜片下料和载具防呆返回JSON数据格式
struct SingleLensDownResponseData {
	bool success;
	QString message;
	qint64 timestamp;
	int code;
	QString cgw;
	QString cgwz;

	static SingleLensDownResponseData fromJson(const QJsonObject& jsonObj) {
		SingleLensDownResponseData response;
		response.code = jsonObj["code"].toInt();
		response.message = jsonObj["message"].toString();
		if (jsonObj.contains("result")) {
			QJsonObject resultObj = jsonObj["result"].toObject();
			response.cgw = resultObj["cgw"].toString();
			response.cgwz = resultObj["cgwz"].toString();
		}
		response.success = jsonObj["success"].toBool();
		response.timestamp = jsonObj["timestamp"].toVariant().toLongLong();
		return response;
	}

	QJsonObject toJson() const {
		QJsonObject jsonObj;
		jsonObj["success"] = success;
		jsonObj["message"] = message;
		jsonObj["timestamp"] = timestamp;
		jsonObj["code"] = code;
		jsonObj["cgw"] = cgw;
		jsonObj["cgwz"] = cgwz;
		return jsonObj;
	}

	QString toStr() const {
		QJsonObject jsonObject = toJson();
		QJsonDocument doc(jsonObject);
		return doc.toJson(QJsonDocument::Compact);
	}
};

struct VehicledesignRequestData {
	QString id;
	QString dvehiclecode;
	QString mac;

	// 转换为JSON格式
	QJsonObject toJson() const {
		QJsonObject jsonObj;
		jsonObj["id"] = id;
		jsonObj["dvehiclecode"] = dvehiclecode;
		jsonObj["mac"] = mac;
		return jsonObj;
	}

	QString toStr() const {
		QJsonObject jsonObject = toJson();
		QJsonDocument doc(jsonObject);
		return doc.toJson(QJsonDocument::Compact);
	}
};

struct ControlData {
	bool qualifiedMolds;
	bool giveWayMolds;
	QString lineStatus;
	QString lineMac;
	QMap<QString, QString> tableData;
};

// 构造上传文件格式结构体
struct ExternalTOSPathbody
{
	std::string use;
	std::string csvPath;
	std::vector<std::string> zipPath;

	// 将结构体转换为JSON
	Json toJson() const {
		return Json{
			{"_use",use},
			{"_csvPath",csvPath},
			{"_zipPath",zipPath}
		};
	}

	QString toStr() const{
		Json jsonObject = toJson();
		return QString::fromStdString(jsonObject.dump());
	}
};

struct ExternalTOSPathToRDbody
{
	std::string use;
	std::string lensId;
	std::vector<std::string> filePath;

	// 将结构体转换为JSON
	Json toJson() const {
		return Json{
			{"use",use},
			{"LensID",lensId},
			{"FilePath",filePath}
		};
	}

	QString toStr() const {
		Json jsonObject = toJson();
		return QString::fromStdString(jsonObject.dump());
	}
};

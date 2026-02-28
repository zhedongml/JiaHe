#include "configItem.h"
#include <LogPlus.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMutex>
#include <QtCore/qfile.h>
#include "Motion2DModel.h"
#include "Core/loggingwrapper.h"
#include "Core/icore.h"

ConfigItem* ConfigItem::self = nullptr;
ConfigItem::ConfigItem()
{
	loadFile();
}

ConfigItem* ConfigItem::instance()
{
	if (!self)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		if (!self)
		{
			self = new ConfigItem();
		}
	}
	return self;
}

ConfigItem::~ConfigItem()
{
}

Result ConfigItem::loadFile()
{
	Result ret;
	ret = readJson(m_motionJson, FILE_MOTION);
	if (!ret.success)
	{
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, ret.errorMsg.c_str());
	}
	ret = readJson(m_keyenceJson, FILE_KEYENCE);
	if (!ret.success)
	{
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, ret.errorMsg.c_str());
	}

	ret = readJson(m_mvCameraJson,FILE_MVCAMERA);
	if (!ret.success)
	{
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, ret.errorMsg.c_str());
	}

	return ret;
}

QString ConfigItem::getMotion3DIpAndOther()
{
	if (m_motionJson.is_null())
	{
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger,
			"Get Motion3D ip and other error, error in analyzing device config json file.");
		return QString();
	}
	QString buffer;
	QList<int> list;
	if (m_motionJson.contains("Motion3D"))
	{
		nlohmann::json motion = m_motionJson["Motion3D"];
		return getMotionType(motion, "DUTControl");
	}

	//LoggingWrapper::instance()->error("Get Motion3D ip and other error, please check device config json file.");
	return QString();
}

QString ConfigItem::getMotion1DIpAndOther()
{
	if (m_motionJson.is_null())
	{
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger,
			"Get Motion1D ip and other error, error in analyzing device config json file.");
		return QString();
	}
	QString buffer;
	QList<int> list;
	if (m_motionJson.contains("Motion1D"))
	{
		nlohmann::json motion = m_motionJson["Motion1D"];
		return getMotionType(motion, "ProjectorThorlabs");
	}

	//LoggingWrapper::instance()->error("Get Motion1D ip and other error, please check device config json file.");
	return QString();
}

QString ConfigItem::getMotion2DIpAndOther(int type)
{
	if (m_motionJson.is_null())
	{
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger,
			"Get Motion2D ip and other error, error in analyzing device config json file.");
		return QString();
	}
	QString buffer = "";
	QList<int> list = { 0 };
	if (m_motionJson.contains("Motion2D"))
	{
		nlohmann::json motion = m_motionJson["Motion2D"];
		if (type == ACS2DDUT)
		{
			return getMotionType(motion, "DUTTilt");
		}
		else if (type == ACS2DPro)
		{
			return getMotionType(motion, "ProjectorTilt");
		}
		else if (type == ACS2DDUTPlane)
		{
			return getMotionType(motion, "DUT");
		}
		else if (type == ACS2DCameraTilt)
		{
			return getMotionType(motion, "CameraModuleTilt");
		}
	}
	// LoggingWrapper::instance()->error("Get Motion3D ip and other error, please check device config json file.");
	return QString();
}

QString ConfigItem::getMotionType(nlohmann::json motion, QString type)
{
	QString buffer = "";
	QList<int> list;
	nlohmann::json motionObj = motion[type.toStdString()];

	if (motionObj.contains("IP"))
	{
		if (motionObj.contains("InitBuffers"))
		{
			nlohmann::json jsonArray = motionObj["InitBuffers"];
			for (const auto& elem : jsonArray)
			{
				list.append(elem.get<int>());
			}
		}
		//if (list.size() == 0)
		//{
		//    if (type == "DUT")
		//    {
		//        LoggingWrapper::instance()->error(
		//            "Get Motion3D init buffers error, please check device config json file.");
		//        return QString();
		//    }
		//}
	}

	QString ipStr = QString::fromStdString(motionObj["IP"].get<std::string>());
	for (int j = 0; j < list.size(); j++)
	{
		buffer += "\\" + QString::number(list[j]);
	}

	QString motionStr = ipStr + buffer;
	return motionStr;
}

MotionLimit ConfigItem::getMotion2DLimits(int type)
{
	MotionLimit m_limits;
	if (m_motionJson.is_null())
	{
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger,
			"Get Limits error, error in analyzing device config json file.");
		return m_limits;
	}
	bool v = m_motionJson.contains("Motion2D");
	if (m_motionJson.contains("Motion2D"))
	{
		nlohmann::json motion = m_motionJson["Motion2D"];
		nlohmann::json motionType;
		if (type == ACS2DDUTPlane)
		{
			if (motion.contains("DUT"))
			{
				motionType = motion["DUT"];
			}
		}

		else if (type == ACS2DPro)
		{
			motionType = motion["ProjectorTilt"];
		}
		else if (type == ACS2DCameraTilt)
		{
			motionType = motion["CameraModuleTilt"];
		}
		if (motionType.contains("Limits"))
		{
			nlohmann::json limits = motionType["Limits"];
			m_limits.x_pos = limits["Max"]["X"].get<float>();
			m_limits.x_neg = limits["Min"]["X"].get<float>();
			m_limits.y_pos = limits["Max"]["Y"].get<float>();
			m_limits.y_neg = limits["Min"]["Y"].get<float>();
		}
	}
	else
	{
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "ConfigItem:get motion2D limits error, please check device config json file.");
	}
	return m_limits;
}

QString ConfigItem::getMVCameraSn()
{
	if (m_mvCameraJson.is_null())
	{
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger,
			"Get camera SN error, error in analyzing device config json file.");
		return QString();
	}

	if (m_mvCameraJson.contains("MVCamera"))
	{
		return QString::fromStdString(m_mvCameraJson["MVCamera"]["withModule"]["SerialNumber"].get<string>());
	}

	LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Get camera SN error, please check device config json file.");
	return QString();
}

Result ConfigItem::getCameraCoefficient(float& pixelSize, float& magnification)
{
	if (m_mvCameraJson.is_null())
	{
		QString msg = "Get camera pixelSize and magnification error, error in analyzing device config json file.";
		return Result(false, msg.toStdString());
	}

	QString msg = "Get camera pixelSize and magnification error, please check device config json file.";
	pixelSize =m_mvCameraJson["MVCamera"]["withModule"]["PixelSize"].get<double>();
	magnification = m_mvCameraJson["MVCamera"]["withModule"]["magnification"].get<double>();

	return Result();
}

Result ConfigItem::getCameraResolution(int& width, int& height)
{
	QString msg;
	if (m_mvCameraJson.is_null())
	{
		msg = "Get camera resolution error, error in analyzing device config json file.";
		return Result(false, msg.toStdString());
	}

	if (m_mvCameraJson.contains("MVCamera"))
	{
		width = m_mvCameraJson["MVCamera"]["withModule"]["resolution"][0];
		height = m_mvCameraJson["MVCamera"]["withModule"]["resolution"][1];
		return Result();
	}
	msg = "Get camera resolution error, please check device config json file.";
	return Result(false, msg.toStdString());
}

QString ConfigItem::getMotion3DIpAndOther(int type)
{
	if (m_motionJson.is_null())
	{
		LoggingWrapper::instance()->error(
			"Get Motion3D ip and other error, error in analyzing device config json file.");
		return QString();
	}
	QString buffer;
	QList<int> list;
	if (m_motionJson.contains("Motion3D"))
	{
		nlohmann::json motion = m_motionJson["Motion3D"];
		nlohmann::json motionobj;
		if (!type)
			motionobj = motion["MeasureCameraControl"];
		else if (type == 1)
			motionobj = motion["DUTControl"];
		else if (type == 2)
			motionobj = motion["DUTTilt"];

		if (motionobj.contains("IP"))
		{
			//if (motionobj.contains("InitBuffers") && motionobj.value("InitBuffers").isArray())
			//{
			//    QJsonArray jsonArray = motionobj.value("InitBuffers").toArray();
			//    for (int i = 0; i < jsonArray.size(); i++)
			//    {
			//        list.append(jsonArray.at(i).toInt());
			//    }
			//}
			//if (list.size() == 0)
			//{
			//    LoggingWrapper::instance()->error(
			//        "Get Motion3D init buffers error, please check device config json file.");
			//    return QString();
			//}
		}

		QString ipStr = QString::fromStdString(motionobj["IP"].get<std::string>());
		for (int j = 0; j < list.size(); j++)
		{
			buffer += "\\" + QString::number(list[j]);
		}

		QString motionStr = ipStr + buffer;
		return motionStr;
	}

	LoggingWrapper::instance()->error("Get Motion3D ip and other error, please check device config json file.");
	return QString();
}

cv::Point2f ConfigItem::getMotion2DZeroPositionOrigin(int type, DUT_EYE_TYPE eyeType)
{
	cv::Point2f point;
	if (type == ACS2DDUT)
	{
		if (DUT_EYE_LEFT == eyeType)
		{
			point.x = m_motionJson["Motion2D"]["DUT"]["ZeroPosition"]["LeftEye"]["X"].get<float>();
			point.y = m_motionJson["Motion2D"]["DUT"]["ZeroPosition"]["LeftEye"]["Y"].get<float>();
		}
		else
		{
			point.x = m_motionJson["Motion2D"]["DUT"]["ZeroPosition"]["RightEye"]["X"].get<float>();
			point.y = m_motionJson["Motion2D"]["DUT"]["ZeroPosition"]["RightEye"]["Y"].get<float>();
		}

	}
	else
	{
		point.x = m_motionJson["Motion2D"]["ProjectorTilt"]["ZeroPosition"]["X"].get<float>();
		point.y = m_motionJson["Motion2D"]["ProjectorTilt"]["ZeroPosition"]["Y"].get<float>();
	}
	return point;
}

cv::Point2f ConfigItem::getMotion2DZeroPositionRaw(int type, DUT_EYE_TYPE eyeType)
{
	return getMotion2DZeroPositionOrigin(type, eyeType);
}

cv::Point2f ConfigItem::getMotion2DZeroPosition(int type, DUT_EYE_TYPE eyeType)
{
	if (!m_realTimeUpdate)
	{
		return getMotion2DZeroPositionOrigin(type, eyeType);
	}
	else
	{
		return m_2DZeroPos[type][eyeType];
	}
}

void ConfigItem::updateMotion2DZeroPosition(int type, DUT_EYE_TYPE eyeType, cv::Point2f tilt, bool forced)
{
	m_2DZeroPos[type][eyeType] = tilt;
	if (forced || m_realTimeUpdate)
	{
		Motion2DModel::getInstance(type)->updateMotion2DZero(eyeType, tilt);
	}
}

void ConfigItem::setRealTimeUpdate(bool realTime)
{
	m_realTimeUpdate = realTime;
}

bool ConfigItem::getRealTimeUpdate()
{
	return m_realTimeUpdate;
}

Result ConfigItem::writeDutZeroPosition(DUT_EYE_TYPE eyeType, cv::Point2f tilt)
{
	if (DUT_EYE_LEFT == eyeType)
	{
		m_motionJson["Motion2D"]["DUT"]["ZeroPosition"]["LeftEye"]["X"] = (int)round((tilt.x * 1000)) / 1000.0;
		m_motionJson["Motion2D"]["DUT"]["ZeroPosition"]["LeftEye"]["Y"] = (int)round((tilt.y * 1000)) / 1000.0;
	}
	else
	{
		m_motionJson["Motion2D"]["DUT"]["ZeroPosition"]["RightEye"]["X"] = (int)round((tilt.x * 1000)) / 1000.0;
		m_motionJson["Motion2D"]["DUT"]["ZeroPosition"]["RightEye"]["Y"] = (int)round((tilt.y * 1000)) / 1000.0;
	}

	std::ofstream ofs(FILE_MOTION.toStdString());
	if (!ofs.is_open())
	{
		return Result(false, QString("Failed to open file %1 for writing dut zero position.").arg(FILE_MOTION).toStdString());
	}
	ofs << m_motionJson.dump(4);
	ofs.close();
	return Result();
}

Limit2D ConfigItem::getMotion2DTiltLimit()
{
	return getMotion2DLimit(ACS2DPro);
}

Limit2D ConfigItem::getMotion2DLimit()
{
	return getMotion2DLimit(ACS2DDUTPlane);
}

cv::Point2f ConfigItem::getMotion1DLimit()
{
	cv::Point2f limit;
	nlohmann::json json = m_motionJson["Motion1D"];
	if (json.contains("ProjectorThorlabs")) {
		limit.x = json["ProjectorThorlabs"]["Limits"]["Min"].get<float>();
		limit.y = json["ProjectorThorlabs"]["Limits"]["Max"].get<float>();
	}
	return limit;
}

Limit2D ConfigItem::getMotion2DLimit(int type)
{
	Limit2D limit;
	nlohmann::json json = m_motionJson["Motion2D"];
	if (ACS2DDUTPlane == type) {
		limit.min.x = json["DUT"]["Limits"]["Min"]["X"].get<float>();
		limit.min.y = json["DUT"]["Limits"]["Min"]["Y"].get<float>();
		limit.max.x = json["DUT"]["Limits"]["Max"]["X"].get<float>();
		limit.max.y = json["DUT"]["Limits"]["Max"]["Y"].get<float>();
	}
	else if (ACS2DPro == type) {
		limit.min.x = json["ProjectorTilt"]["Limits"]["Min"]["X"].get<float>();
		limit.min.y = json["ProjectorTilt"]["Limits"]["Min"]["Y"].get<float>();
		limit.max.x = json["ProjectorTilt"]["Limits"]["Max"]["X"].get<float>();
		limit.max.y = json["ProjectorTilt"]["Limits"]["Max"]["Y"].get<float>();
	}
	return limit;
}

CoordinateLimit ConfigItem::getMotion3DLimit(int type)
{
	CoordinateLimit limit;
	nlohmann::json json = m_motionJson["Motion3D"];
	if (withCamera == type) {
		limit.min.x = json["MeasureCameraControl"]["Limits"]["Min"]["X"].get<float>();
		limit.min.y = json["MeasureCameraControl"]["Limits"]["Min"]["Y"].get<float>();
		limit.min.z = json["MeasureCameraControl"]["Limits"]["Min"]["Z"].get<float>();
		limit.max.x = json["MeasureCameraControl"]["Limits"]["Max"]["X"].get<float>();
		limit.max.y = json["MeasureCameraControl"]["Limits"]["Max"]["Y"].get<float>();
		limit.max.z = json["MeasureCameraControl"]["Limits"]["Max"]["Z"].get<float>();
	}
	return limit;
}

CoordinateLimit ConfigItem::getOrientalLimit()
{
	CoordinateLimit limit;
	nlohmann::json json = m_motionJson["Motion3D"];
	limit.min.x = json["DUTModuleTilt"]["Limits"]["Min"]["X"].get<float>();
	limit.min.y = json["DUTModuleTilt"]["Limits"]["Min"]["Y"].get<float>();
	limit.min.z = json["DUTModuleTilt"]["Limits"]["Min"]["Z"].get<float>();
	limit.max.x = json["DUTModuleTilt"]["Limits"]["Max"]["X"].get<float>();
	limit.max.y = json["DUTModuleTilt"]["Limits"]["Max"]["Y"].get<float>();
	limit.max.z = json["DUTModuleTilt"]["Limits"]["Max"]["Z"].get<float>();
	return limit;
}

KeyenceDviceInfo ConfigItem::getKeyenceType()
{
	if (m_keyenceJson.is_null())
	{
		LoggingWrapper::instance()->error("Get keyence type error, error in analyzing device config json file.");
		return KeyenceDviceInfo();
	}

	KeyenceDviceInfo info;
	info.type = QString::fromStdString(m_keyenceJson["Keyence"]["Type"].get<string>());
	info.id = m_keyenceJson["Keyence"]["id"].get<int>();
	return info;
}

KeyenceInfo ConfigItem::getKeyenceInfo()
{
	if (m_keyenceJson.is_null())
	{
		LoggingWrapper::instance()->error("Get keyence range error, error in analyzing device config json file.");
		return KeyenceInfo();
	}

	KeyenceInfo info;
	info.keyenceRangeMax = m_keyenceJson["Ranging"]["Max"].get<int>();
	info.keyenceRangeMin = m_keyenceJson["Ranging"]["Min"].get<int>();
	info.keyenceZeroPos = m_keyenceJson["Ranging"]["ZeroPos"].get<int>();
	return info;
}

double ConfigItem::getThorlabsAngle()
{
	nlohmann::json json = m_motionJson["Motion1D"];
	if (!json.contains("ProjectorThorlabs")|| !json["ProjectorThorlabs"].contains("Angle")) {
		return DBL_MAX;
	}
	return json["ProjectorThorlabs"]["Angle"].get<double>();
}

Result ConfigItem::readJson(QString fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return Result(false, QString("Open %1 failed.").arg(fileName).toStdString());
	}
	QByteArray array = file.readAll();

	QJsonParseError jsonError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(array, &jsonError);

	if (jsonError.error != QJsonParseError::NoError)
	{
		return Result(
			false,
			QString("Error in analyzing device config json file., file name is %1. ").arg(fileName).toStdString());
	}

	m_rootObj = jsonDoc.object();
	return Result();
}

Result ConfigItem::readJson(nlohmann::json& json, QString fileName)
{
	std::ifstream jsonFile(fileName.toStdString());
	if (jsonFile.is_open())
	{
		std::string contents =
			std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
		jsonFile.close();
		json = nlohmann::json::parse(contents);
	}
	else
	{
		qWarning() << QString("Load file error, %1").arg(fileName);
		return Result(false, "Load file error, " + fileName.toStdString());
	}
	return Result();
}

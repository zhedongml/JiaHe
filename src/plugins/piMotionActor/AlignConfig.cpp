#include "AlignConfig.h"
#include <LogPlus.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMutex>
#include <QtCore/qfile.h>
#include <QDebug>

AlignConfig *AlignConfig::self = nullptr;
AlignConfig::AlignConfig()
{
    loadFile();
}

AlignConfig *AlignConfig::instance()
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new AlignConfig();
        }
    }
    return self;
}

AlignConfig::~AlignConfig()
{
}

Result AlignConfig::loadFile()
{
    std::ifstream jsonFile(FILE_MOTION.toStdString());
    if (jsonFile.is_open())
    {
        std::string contents =
            std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();
        m_motionJson = Json::parse(contents);

        {
            m_2DZeroPos[ACS2DDUT][DUT_EYE_LEFT] = getMotion2DZeroPosition(ACS2DDUT, DUT_EYE_LEFT);
            m_2DZeroPos[ACS2DDUT][DUT_EYE_RIGHT] = getMotion2DZeroPosition(ACS2DDUT, DUT_EYE_RIGHT);
            m_2DZeroPos[ACS2DPro][DUT_EYE_LEFT] = getMotion2DZeroPosition(ACS2DPro, DUT_EYE_LEFT);
            m_2DZeroPos[ACS2DPro][DUT_EYE_RIGHT] = getMotion2DZeroPosition(ACS2DPro, DUT_EYE_RIGHT);
        }
    }
    else
    {
        qCritical() << QString("Load file error, %1").arg(FILE_MOTION);
    }

    Result ret = readJson(FILE_CAMERA);
    m_cameraObj = m_rootObj;
    if (!ret.success)
    {
        qCritical() << QString::fromStdString(ret.errorMsg);
    }
    return ret;
}

QString AlignConfig::getMotion2DIpAndOther(int type)
{
    if (m_motionJson.is_null())
    {
        qCritical() << "Get Motion2D ip and other error, error in analyzing device config json file.";
        return QString();
    }

    QString buffer = "";
    QList<int> list = {0}; 
    if (m_motionJson.contains("Motion2D") )
    {
        Json motion = m_motionJson["Motion2D"];
        if (type == ACS2DDUT)
        {
            return getMotionType(motion, "DUT");
        }
         else if (type == ACS2DPro)
         {
             return getMotionType(motion, "Projector");
         }    
    }
    // LoggingWrapper::instance()->error("Get Motion3D ip and other error, please check device config json file.");
     return QString();
}

QString AlignConfig::getMotion3DIpAndOther(int type)
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
        Json motion = m_motionJson["Motion3D"];
        Json motionobj;
        if (withCamera == type)
            motionobj = motion["MeasureCameraControl"];
        else if (type == 1)
            motionobj = motion["DUTControl"];

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

Limit2D AlignConfig::getMotion2DLimits(int type)
{
    Limit2D limit;
    if(ACS2DDUTPlane == type){
        limit.min.x = m_motionJson["Motion2D"]["DUT"]["Limits"]["Min"]["X"].get<float>();
        limit.min.y = m_motionJson["Motion2D"]["DUT"]["Limits"]["Min"]["Y"].get<float>();
        limit.max.x = m_motionJson["Motion2D"]["DUT"]["Limits"]["Max"]["X"].get<float>();
        limit.max.y = m_motionJson["Motion2D"]["DUT"]["Limits"]["Max"]["Y"].get<float>();
    }else if(ACS2DPro == type)
    {
        limit.min.x = m_motionJson["Motion2D"]["Projector"]["Limits"]["Min"]["X"].get<float>();
        limit.min.y = m_motionJson["Motion2D"]["Projector"]["Limits"]["Min"]["Y"].get<float>();
        limit.max.x = m_motionJson["Motion2D"]["Projector"]["Limits"]["Max"]["X"].get<float>();
        limit.max.y = m_motionJson["Motion2D"]["Projector"]["Limits"]["Max"]["Y"].get<float>();
    }
   
    return limit;
}

CoordinateLimit AlignConfig::getMotion3DLimits(int type)
{
    CoordinateLimit limit;
    if (0 == type) {
        limit.min.x = m_motionJson["Motion3D"]["MeasureCameraControl"]["Limits"]["Min"]["X"].get<float>();
        limit.min.y = m_motionJson["Motion3D"]["MeasureCameraControl"]["Limits"]["Min"]["Y"].get<float>();
        limit.min.z = m_motionJson["Motion3D"]["MeasureCameraControl"]["Limits"]["Min"]["Z"].get<float>();
        limit.max.x = m_motionJson["Motion3D"]["MeasureCameraControl"]["Limits"]["Max"]["X"].get<float>();
        limit.max.y = m_motionJson["Motion3D"]["MeasureCameraControl"]["Limits"]["Max"]["Y"].get<float>();
        limit.max.z = m_motionJson["Motion3D"]["MeasureCameraControl"]["Limits"]["Max"]["Z"].get<float>();
    }
    return limit;
}

QString AlignConfig::getCameraSn()
{
    if (m_cameraJson.empty())
    {
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger,
                        "Get camera SN error, error in analyzing device config json file.");
        return QString();
    }

    m_cameraJson["MVCamera"][""]

    if (m_cameraJson.contains("MVCamera") && m_cameraObj.value("MVCamera").isObject())
    {
        QJsonObject MVCamera = m_cameraObj.value("MVCamera").toObject();
        QJsonObject MVDutObj = MVCamera.value("withDUT").toObject();
        if (MVDutObj.contains("SerialNumber"))
        {
            return MVDutObj.value("SerialNumber").toString();
        }
    }

    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Get camera SN error, please check device config json file.");
    return QString();
}

cv::Point2f AlignConfig::getMotion2DZeroPositionOrigin(int type, DUT_EYE_TYPE eyeType)
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
        point.x = m_motionJson["Motion2D"]["Projector"]["ZeroPosition"]["X"].get<float>();
        point.y = m_motionJson["Motion2D"]["Projector"]["ZeroPosition"]["Y"].get<float>();     
    }
    return point;
}

cv::Point2f AlignConfig::getMotion2DZeroPositionRaw(int type, DUT_EYE_TYPE eyeType)
{
    return getMotion2DZeroPositionOrigin(type, eyeType);
}

cv::Point2f AlignConfig::getMotion2DZeroPosition(int type, DUT_EYE_TYPE eyeType)
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

void AlignConfig::updateMotion2DZeroPosition(int type, DUT_EYE_TYPE eyeType, cv::Point2f tilt, bool forced)
{
    m_2DZeroPos[type][eyeType] = tilt;
    if (forced || m_realTimeUpdate)
    {
        Motion2DModel::getInstance(type)->updateMotion2DZero(eyeType, tilt);
    }
}

void AlignConfig::setRealTimeUpdate(bool realTime)
{
    m_realTimeUpdate = realTime;
}

bool AlignConfig::getRealTimeUpdate()
{
    return m_realTimeUpdate;
}

Result AlignConfig::writeDutZeroPosition(DUT_EYE_TYPE eyeType, cv::Point2f tilt)
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

QString AlignConfig::getMotionType(Json motion, QString type)
{
    QString buffer = "";
    QList<int> list;
    Json motionObj = motion[type.toStdString()];

    if (motionObj.contains("IP"))
    {
        if (motionObj.contains("InitBuffers"))
        {
            Json jsonArray = motionObj["InitBuffers"];
            for (const auto& elem : jsonArray)
            {
                list.append(elem.get<int>());
            }
        }
        if (list.size() == 0)
        {
            if (type == "DUT")
            {
                LoggingWrapper::instance()->error(
                    "Get Motion3D init buffers error, please check device config json file.");
                return QString();
            }
        }
    }

    QString ipStr = QString::fromStdString(motionObj["IP"].get<std::string>());
    for (int j = 0; j < list.size(); j++)
    {
        buffer += "\\" + QString::number(list[j]);
    }

    QString motionStr = ipStr + buffer;
    return motionStr;
}



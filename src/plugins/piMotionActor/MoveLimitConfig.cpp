#include "MoveLimitConfig.h"
#include <QMutex>
#include <QThread>

MoveLimitConfig *MoveLimitConfig::m_instance = nullptr;

MoveLimitConfig *MoveLimitConfig::instance()
{
    if (!m_instance)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance)
        {
            m_instance = new MoveLimitConfig();
        }
    }
    return m_instance;
}

MoveLimitConfig::MoveLimitConfig()
{
    load();
}

MoveLimitConfig::~MoveLimitConfig()
{
}

Result MoveLimitConfig::load()
{
    {
        std::ifstream jsonFile(FILE_NAME.toStdString());
        if (!jsonFile.is_open())
        {
            return Result(false, QString("Open config file failed, %1.").arg(FILE_NAME).toStdString());
        }

        std::string contents =
            std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();

        Json jsonObj = Json::parse(contents);
        m_json = jsonObj;
    }

    {
        std::ifstream jsonFile(FILE_NAME_EYEBOX_LIMIT.toStdString());
        if (!jsonFile.is_open())
        {
            return Result(false, QString("Open config file failed, %1.").arg(FILE_NAME_EYEBOX_LIMIT).toStdString());
        }

        std::string contents =
            std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();

        Json jsonObj = Json::parse(contents);
        m_jsonEyeboxLimit = jsonObj;
    }
    return Result();
}

CoordinateLimit MoveLimitConfig::getDutLimit()
{
    CoordinateLimit limit;
    limit.min.x = m_json["DutMove"]["Min"]["X"].get<float>();
    limit.min.y = m_json["DutMove"]["Min"]["Y"].get<float>();
    limit.min.z = m_json["DutMove"]["Min"]["Z"].get<float>();

    limit.max.x = m_json["DutMove"]["Max"]["X"].get<float>();
    limit.max.y = m_json["DutMove"]["Max"]["Y"].get<float>();
    limit.max.z = m_json["DutMove"]["Max"]["Z"].get<float>();
    return limit;
}

CoordinateLimit MoveLimitConfig::getMeasureCameraLimit()
{
    CoordinateLimit limit;
    limit.min.x = m_json["MeasureCameraMove"]["Min"]["X"].get<float>();
    limit.min.y = m_json["MeasureCameraMove"]["Min"]["Y"].get<float>();
    limit.min.z = m_json["MeasureCameraMove"]["Min"]["Z"].get<float>();
                          
    limit.max.x = m_json["MeasureCameraMove"]["Max"]["X"].get<float>();
    limit.max.y = m_json["MeasureCameraMove"]["Max"]["Y"].get<float>();
    limit.max.z = m_json["MeasureCameraMove"]["Max"]["Z"].get<float>();
    return limit;
}

DutMcCollision MoveLimitConfig::getDutMcCollision()
{
    DutMcCollision data;
    data.centerRelative.x = m_json["DutMcCollision"]["CenterRelative"]["X"].get<float>();
    data.centerRelative.y = m_json["DutMcCollision"]["CenterRelative"]["Y"].get<float>();
    data.centerRelative.z = m_json["DutMcCollision"]["CenterRelative"]["Z"].get<float>();

    data.rectangle.x = m_json["DutMcCollision"]["Rectangle"]["LengthX"].get<float>();
    data.rectangle.y = m_json["DutMcCollision"]["Rectangle"]["WidthY"].get<float>();

    data.minDistanceZ = m_json["DutMcCollision"]["MinDistanceZ"].get<float>();
    return data;
}

DutProjectorCollision MoveLimitConfig::getDutProjectorCollision()
{
    DutProjectorCollision data;
    data.centerRelative.x = m_json["DutProjectorCollision"]["CenterRelative"]["X"].get<float>();
    data.centerRelative.y = m_json["DutProjectorCollision"]["CenterRelative"]["Y"].get<float>();
    data.centerRelative.z = m_json["DutProjectorCollision"]["CenterRelative"]["Z"].get<float>();

    data.rectangle.x = m_json["DutProjectorCollision"]["Rectangle"]["LengthX"].get<float>();
    data.rectangle.y = m_json["DutProjectorCollision"]["Rectangle"]["WidthY"].get<float>();

    data.minDistanceZ = m_json["DutProjectorCollision"]["MinDistanceZ"].get<float>();
    return data;
}

cv::Point3f MoveLimitConfig::getLoadDutPosition()
{
    cv::Point3f data;
    data.x = m_json["LoadDutPosition"]["X"].get<float>();
    data.y = m_json["LoadDutPosition"]["Y"].get<float>();
    data.z = m_json["LoadDutPosition"]["Z"].get<float>();
    return data;
}

cv::Point3f MoveLimitConfig::getLoadMeasureCameraPosition()
{
    cv::Point3f data;
    data.x = m_json["LoadMeasureCameraPosition"]["X"].get<float>();
    data.y = m_json["LoadMeasureCameraPosition"]["Y"].get<float>();
    data.z = m_json["LoadMeasureCameraPosition"]["Z"].get<float>();
    return data;
}

cv::Point3f MoveLimitConfig::getMotion3DMoveInitSpeed()
{
    cv::Point3f data;
    data.x = m_json["Motion3DMoveSpeed"]["InitSpeed"]["X"].get<float>();
    data.y = m_json["Motion3DMoveSpeed"]["InitSpeed"]["Y"].get<float>();
    data.z = m_json["Motion3DMoveSpeed"]["InitSpeed"]["Z"].get<float>();
    return data;
}

cv::Point3f MoveLimitConfig::getMotion3DMoveAlignSpeed()
{
    cv::Point3f data;
    data.x = m_json["Motion3DMoveSpeed"]["AlignSpeed"]["X"].get<float>();
    data.y = m_json["Motion3DMoveSpeed"]["AlignSpeed"]["Y"].get<float>();
    data.z = m_json["Motion3DMoveSpeed"]["AlignSpeed"]["Z"].get<float>();
    return data;
}

QMap<int, QList<EyeboxLimit>> MoveLimitConfig::getEyeboxLimit()
{
    QMap<int, QList<EyeboxLimit>> map;
    map[0] = getEyeboxLimit(m_jsonEyeboxLimit["LeftEye"]);
    map[1] = getEyeboxLimit(m_jsonEyeboxLimit["RightEye"]);
    return map;
}

void MoveLimitConfig::test()
{
    getDutLimit();
    getMeasureCameraLimit();
    getDutMcCollision();

    getLoadDutPosition();
    getLoadMeasureCameraPosition();
}

QList<EyeboxLimit> MoveLimitConfig::getEyeboxLimit(Json json)
{
    QList<EyeboxLimit> list;
    Json vals = json;
    for (auto it = vals.begin(); it != vals.end(); ++it)
    {
        Json val = it.value();

        EyeboxLimit limit;
        limit.dutAngle.x = val["DUTAngle"]["X"].get<float>();
        limit.dutAngle.y = val["DUTAngle"]["Y"].get<float>();

        limit.projectorAngle.x = val["ProAngle"]["X"].get<float>();
        limit.projectorAngle.y = val["ProAngle"]["Y"].get<float>();

        limit.min.x = val["EyeboxLimit"]["Min"]["X"].get<float>();
        limit.min.y = val["EyeboxLimit"]["Min"]["Y"].get<float>();
        limit.min.z = val["EyeboxLimit"]["Min"]["Z"].get<float>();

        limit.max.x = val["EyeboxLimit"]["Max"]["X"].get<float>();
        limit.max.y = val["EyeboxLimit"]["Max"]["Y"].get<float>();
        limit.max.z = val["EyeboxLimit"]["Max"]["Z"].get<float>();
        list.push_back(limit);
    }
    return list;
}

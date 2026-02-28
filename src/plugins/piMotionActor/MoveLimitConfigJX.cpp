#include "MoveLimitConfigJX.h"
#include <QMutex>
#include <QThread>
#include "configItem.h"

MoveLimitConfigJX *MoveLimitConfigJX::m_instance = nullptr;

MoveLimitConfigJX *MoveLimitConfigJX::instance()
{
    if (!m_instance)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance)
        {
            m_instance = new MoveLimitConfigJX();
        }
    }
    return m_instance;
}

MoveLimitConfigJX::MoveLimitConfigJX()
{
    load();
}

MoveLimitConfigJX::~MoveLimitConfigJX()
{
}

Result MoveLimitConfigJX::load()
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

Limit2D MoveLimitConfigJX::getDutLimit()
{
    return ConfigItem::instance()->getMotion2DLimit();
}

CoordinateLimit MoveLimitConfigJX::getMeasureCameraLimit()
{
    CoordinateLimit limit = ConfigItem::instance()->getMotion3DLimit();
    return limit;
}

Result MoveLimitConfigJX::getDutMcCollision(DutMcCollisionJX &collision)
{
    cv::Point2f mcAngle;
    cv::Point2f proAngle;
    getDefaultAngle(mcAngle, proAngle);

    QList<DutMcCollisionJX> list = getDutMcCollisionList();
    for(DutMcCollisionJX data: list){
        if(abs(data.mcAngle.x - mcAngle.x) < MAX_ERROR_VALUE && abs(data.mcAngle.y - mcAngle.y) < MAX_ERROR_VALUE){
            collision = data;
            return Result();
        }
    }

    return Result(false, QString("Get DUT Mc collision information failed, MC default angle(%1,%2).")
        .arg(QString::number(mcAngle.x, 'f', 3))
        .arg(QString::number(mcAngle.y, 'f', 3))
        .toStdString());
}

Result MoveLimitConfigJX::getDutProjectorCollision(DutProjectorCollisionJX& collision, cv::Point2f proAngle)
{
    if(abs(proAngle.x) > 50 || abs(proAngle.y) > 50){
        cv::Point2f mcAngle;
        getDefaultAngle(mcAngle, proAngle);
    }

    QList<DutProjectorCollisionJX> list = getDutProjectorCollisionList();
    for (DutProjectorCollisionJX data : list) {
        if (abs(data.proAngle.x - proAngle.x) < MAX_ERROR_VALUE && abs(data.proAngle.y - proAngle.y) < MAX_ERROR_VALUE) {
            collision = data;
            return Result();
        }
    }

    return Result(false, QString("Get DUT Projector collision information failed, Projector angle(%1,%2).")
        .arg(QString::number(proAngle.x, 'f', 3))
        .arg(QString::number(proAngle.y, 'f', 3))
        .toStdString());
}

Result MoveLimitConfigJX::getMcProjectorCollision(McProjectorCollisionJX& collision, cv::Point2f proAngle)
{
    cv::Point2f proAngleDef;
    cv::Point2f mcAngle;
    getDefaultAngle(mcAngle, proAngleDef);
    if (abs(proAngle.x) > 50 || abs(proAngle.y) > 50) {
        proAngle = proAngleDef;
    }

    QList<McProjectorCollisionJX> list = getMcProjectorCollisionList();
    for (McProjectorCollisionJX data : list) {
        if (abs(data.proAngle.x - proAngle.x) < MAX_ERROR_VALUE &&
            abs(data.proAngle.y - proAngle.y) < MAX_ERROR_VALUE &&
            abs(data.mcAngle.x - mcAngle.x) < MAX_ERROR_VALUE &&
            abs(data.mcAngle.y - mcAngle.y) < MAX_ERROR_VALUE)
        {
            collision = data;
            return Result();
        }
    }

    return Result(false, QString("Get DUT Projector collision information failed, MC default angle(%1,%2), Projector angle(%3,%4).")
        .arg(QString::number(mcAngle.x, 'f', 3))
        .arg(QString::number(mcAngle.y, 'f', 3))
        .arg(QString::number(proAngle.x, 'f', 3))
        .arg(QString::number(proAngle.y, 'f', 3))
        .toStdString());

    return Result();
}

cv::Point2f MoveLimitConfigJX::getLoadDutPosition()
{
    cv::Point2f data;
    data.x = m_json["LoadDutPosition"]["X"].get<float>();
    data.y = m_json["LoadDutPosition"]["Y"].get<float>();
    return data;
}

cv::Point3f MoveLimitConfigJX::getLoadMeasureCameraPosition()
{
    cv::Point3f data;
    data.x = m_json["LoadMeasureCameraPosition"]["X"].get<float>();
    data.y = m_json["LoadMeasureCameraPosition"]["Y"].get<float>();
    data.z = m_json["LoadMeasureCameraPosition"]["Z"].get<float>();
    return data;
}

cv::Point2f MoveLimitConfigJX::getInitTestDutPosition()
{
    cv::Point2f data;
    data.x = m_json["InitTestDutPosition"]["X"].get<float>();
    data.y = m_json["InitTestDutPosition"]["Y"].get<float>();
    return data;
}

cv::Point3f MoveLimitConfigJX::getInitTestMcPosition()
{
    cv::Point3f data;
    data.x = m_json["InitTestMcPosition"]["X"].get<float>();
    data.y = m_json["InitTestMcPosition"]["Y"].get<float>();
    data.z = m_json["InitTestMcPosition"]["Z"].get<float>();
    return data;
}

cv::Point3f MoveLimitConfigJX::getMotion3DMoveInitSpeed()
{
    cv::Point3f data;
    data.x = m_json["Motion3DMoveSpeed"]["InitSpeed"]["X"].get<float>();
    data.y = m_json["Motion3DMoveSpeed"]["InitSpeed"]["Y"].get<float>();
    data.z = m_json["Motion3DMoveSpeed"]["InitSpeed"]["Z"].get<float>();
    return data;
}

cv::Point3f MoveLimitConfigJX::getMotion3DMoveAlignSpeed()
{
    cv::Point3f data;
    data.x = m_json["Motion3DMoveSpeed"]["AlignSpeed"]["X"].get<float>();
    data.y = m_json["Motion3DMoveSpeed"]["AlignSpeed"]["Y"].get<float>();
    data.z = m_json["Motion3DMoveSpeed"]["AlignSpeed"]["Z"].get<float>();
    return data;
}

QMap<int, QList<EyeboxLimitJX>> MoveLimitConfigJX::getEyeboxLimit()
{
    QMap<int, QList<EyeboxLimitJX>> map;
    map[0] = getEyeboxLimit(m_jsonEyeboxLimit["LeftEye"]);
    map[1] = getEyeboxLimit(m_jsonEyeboxLimit["RightEye"]);
    return map;
}

void MoveLimitConfigJX::test()
{
    getDutLimit();
    getMeasureCameraLimit();

    getLoadDutPosition();
    getLoadMeasureCameraPosition();
}

QList<EyeboxLimitJX> MoveLimitConfigJX::getEyeboxLimit(Json json)
{
    QList<EyeboxLimitJX> list;
    Json vals = json;
    for (auto it = vals.begin(); it != vals.end(); ++it)
    {
        Json val = it.value();

        EyeboxLimitJX limit;
        limit.measureAngle.x = val["McAngle"]["X"].get<float>();
        limit.measureAngle.y = val["McAngle"]["Y"].get<float>();

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

void MoveLimitConfigJX::getDefaultAngle(cv::Point2f& mcAngle, cv::Point2f& proAngle)
{
    mcAngle.x = m_json["DefaultAngle"]["McAngle"]["X"].get<float>();
    mcAngle.y = m_json["DefaultAngle"]["McAngle"]["Y"].get<float>();

    proAngle.x = m_json["DefaultAngle"]["ProAngle"]["X"].get<float>();
    proAngle.y = m_json["DefaultAngle"]["ProAngle"]["Y"].get<float>();
}

QList<DutMcCollisionJX> MoveLimitConfigJX::getDutMcCollisionList()
{
    QList<DutMcCollisionJX> list;
    Json vals = m_json["DutMcCollision"];
    for (auto it = vals.begin(); it != vals.end(); ++it)
    {
        DutMcCollisionJX data;
        Json val = it.value();

        data.mcAngle.x = val["McAngle"]["X"].get<float>();
        data.mcAngle.y = val["McAngle"]["Y"].get<float>();

        data.centerRelative.x = val["CenterRelative"]["X"].get<float>();
        data.centerRelative.y = val["CenterRelative"]["Y"].get<float>();

        data.rectangle.x = val["Rectangle"]["LengthX"].get<float>();
        data.rectangle.y = val["Rectangle"]["WidthY"].get<float>();

        data.mcMaxZ = val["McMaxZ"].get<float>();
        list.push_back(data);
    }
    return list;
}

QList<DutProjectorCollisionJX> MoveLimitConfigJX::getDutProjectorCollisionList()
{
    QList<DutProjectorCollisionJX> list;
    Json vals = m_json["DutProjectorCollision"];
    for (auto it = vals.begin(); it != vals.end(); ++it)
    {
        DutProjectorCollisionJX data;
        Json val = it.value();

        data.proAngle.x = val["ProAngle"]["X"].get<float>();
        data.proAngle.y = val["ProAngle"]["Y"].get<float>();

        data.centerRelative.x = val["CenterRelative"]["X"].get<float>();
        data.centerRelative.y = val["CenterRelative"]["Y"].get<float>();

        data.rectangle.x = val["Rectangle"]["LengthX"].get<float>();
        data.rectangle.y = val["Rectangle"]["WidthY"].get<float>();

        list.push_back(data);
    }
    return list;
}

QList<McProjectorCollisionJX> MoveLimitConfigJX::getMcProjectorCollisionList()
{
    QList<McProjectorCollisionJX> list;
    Json vals = m_json["McProjectorCollision"];
    for (auto it = vals.begin(); it != vals.end(); ++it)
    {
        McProjectorCollisionJX data;
        Json val = it.value();

        data.mcAngle.x = val["McAngle"]["X"].get<float>();
        data.mcAngle.y = val["McAngle"]["Y"].get<float>();

        data.proAngle.x = val["ProAngle"]["X"].get<float>();
        data.proAngle.y = val["ProAngle"]["Y"].get<float>();

        data.centerRelative.x = val["CenterRelative"]["X"].get<float>();
        data.centerRelative.y = val["CenterRelative"]["Y"].get<float>();
        data.centerRelative.z = val["CenterRelative"]["Z"].get<float>();

        data.rectangle.y = val["Rectangle"]["LengthY"].get<float>();
        data.rectangle.z = val["Rectangle"]["WidthZ"].get<float>();

        data.mcMinX = val["MinDistanceX"].get<float>();
        list.push_back(data);
    }
    return list;
}

#include "DistanceCheck.h"
#include "Core/loggingwrapper.h"
#include "DistanceManage.h"

using namespace Align;

DistanceCheck& DistanceCheck::instance()
{
    static DistanceCheck self;
    return self;
}

DistanceCheck::DistanceCheck()
{
    Result ret = loadFile();
    if(!ret.success){
        LoggingWrapper::instance()->error(QString::fromStdString(ret.errorMsg));
    }
}

DistanceCheck::~DistanceCheck()
{
}

DistanceCheckInfo DistanceCheck::getDistanceCheckInfo()
{
    DistanceCheckInfo info;
    info.targetDistance = m_json["TargetDistance"].get<float>();
    info.offset = m_json["Offset"].get<float>();
    return info;
}

Result DistanceCheck::check()
{
    double distance;
    Result ret = DistanceManage().getKeyenceToDutDistance(distance, KEYENCE_TYPE_UP);
    if(!ret.success){
        return Result(false, "Distance check failed, " + ret.errorMsg);
    }

    DistanceCheckInfo info = getDistanceCheckInfo();
    float offsetCurrent = distance - info.targetDistance;
    if(abs(offsetCurrent) > abs(info.offset)){
        return Result(false, QString("Distance offset is too large, current distance %1, offset range +/-%2")
            .arg(QString::number(distance, 'f', 3))
            .arg(QString::number(info.offset, 'f', 3))
            .toStdString());
    }

    return Result();
}

Result DistanceCheck::updateTargetDistance(float diatance)
{
    Json json = m_json;
    json["TargetDistance"] = (int)round((diatance * 1000)) / 1000.0;;

    std::ofstream ofs(FILE_NAME.toStdString());
    if (!ofs.is_open())
    {
        return Result(false, QString("Update DUT target distance error, open %1 failed.").arg(FILE_NAME).toStdString());
    }

    ofs << json.dump(4);
    ofs.close();

    m_json = json;
    return Result();
}

Result DistanceCheck::loadFile()
{
    std::ifstream jsonFile(FILE_NAME.toStdString());
    if (!jsonFile.is_open())
    {
        return Result(false, QString("Load file error, %1").arg(FILE_NAME).toStdString());
    }

    std::string contents =
        std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
    jsonFile.close();
    m_json = Json::parse(contents);
    return Result();
}

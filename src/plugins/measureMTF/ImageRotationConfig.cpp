#include "ImageRotationConfig.h"
#include "MLColorimeterHelp.h"
#include <QMutex>
#include <QThread>
#include <QDebug>
#include <QMessageBox>

ImageRotationConfig *ImageRotationConfig::m_instance = nullptr;

ImageRotationConfig *ImageRotationConfig::instance()
{
    if (!m_instance)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance)
        {
            m_instance = new ImageRotationConfig();
        }
    }
    return m_instance;
}

ImageRotationConfig::~ImageRotationConfig()
{
}

Result ImageRotationConfig::GetFilpRotationConfig_DutLeftEye(Flipping& flip_x, Flipping& flip_y, Rotation& rotate)
{
    if (!m_json.contains("DUT_LeftEye")) {
        return Result(false, "Get rotation and mirrorFlip error, config file is error.");
    }

    flip_x = m_json["DUT_LeftEye"]["FlippingX"].get<bool>() ? Flipping::ReverseX : Flipping::NonReverse;
    flip_y = m_json["DUT_LeftEye"]["FlippingY"].get<bool>() ? Flipping::ReverseY : Flipping::NonReverse;
    rotate = MLColorimeterHelp::instance()->TransStrToRotation(m_json["DUT_LeftEye"]["Rotation"].get<std::string>());

    return Result();
}

Result ImageRotationConfig::GetFilpRotationConfig_DutRightEye(Flipping& flip_x, Flipping& flip_y, Rotation& rotate)
{
    if (!m_json.contains("DUT_RightEye")) {
        return Result(false, "Get rotation and mirrorFlip error, config file is error.");
    }

    flip_x = m_json["DUT_RightEye"]["FlippingX"].get<bool>() ? Flipping::ReverseX : Flipping::NonReverse;
    flip_y = m_json["DUT_RightEye"]["FlippingY"].get<bool>() ? Flipping::ReverseY : Flipping::NonReverse;
    rotate = MLColorimeterHelp::instance()->TransStrToRotation(m_json["DUT_RightEye"]["Rotation"].get<std::string>());

    return Result();
}

Result ImageRotationConfig::GetFilpRotationConfig_SLB(Flipping& flip_x, Flipping& flip_y, Rotation& rotate)
{
    if (!m_json.contains("SLB")) {
        return Result(false, "Get rotation and mirrorFlip error, config file is error.");
    }

    flip_x = m_json["SLB"]["FlippingX"].get<bool>() ? Flipping::ReverseX : Flipping::NonReverse;
    flip_y = m_json["SLB"]["FlippingY"].get<bool>() ? Flipping::ReverseY : Flipping::NonReverse;
    rotate = MLColorimeterHelp::instance()->TransStrToRotation(m_json["SLB"]["Rotation"].get<std::string>());

    return Result();
}

bool ImageRotationConfig::isRotation()
{
    return false;
    bool ret = m_json["IsRotation"].get<bool>();
    return ret;
}

ImageRotationConfig::ImageRotationConfig()
{
    Result ret = load();
    if(!ret.success){
        QMessageBox::warning(NULL, QString("Read %1 config file error, ").arg(FILE_NAME), QString::fromStdString(ret.errorMsg), QMessageBox::Yes);
        qCritical() << QString("Read %1 config file error, ").arg(FILE_NAME) << QString::fromStdString(ret.errorMsg);
    }
}

Result ImageRotationConfig::load()
{
    // function is cancle
    return Result();
    std::ifstream jsonFile(FILE_NAME.toStdString());
    if (!jsonFile.is_open())
    {
        return Result(false, QString("Open config file failed, %1.").arg(FILE_NAME).toStdString());
    }

    std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
    jsonFile.close();

    Json jsonObj = Json::parse(contents);
    m_json = jsonObj;
    return Result();
}

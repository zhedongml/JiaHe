#include "ConfigMAInfo.h"
#include <fstream>
#include <iomanip>
#include <QDebug>

bool ConfigMAInfo::Load()
{
    std::string instrumentFile = FILE_NAME.toStdString();
    std::ifstream ifs(instrumentFile.c_str(), std::fstream::in);
    if (ifs.fail())
    {
        qCritical() << "Load config file error, " << FILE_NAME;
        return false;
    }
    ifs >> m_JsonData;

    {
        std::ifstream ifsSLB(SLB_FILE_NAME.toStdString().c_str(), std::fstream::in);
        if (ifsSLB.fail())
        {
            qCritical() << "Load config file error, " << SLB_FILE_NAME;
            return false;
        }
        ifsSLB >> m_jsonSLB;
    }

    return true;
}

MAInfo ConfigMAInfo::ReadMAInfo()
{
    std::lock_guard<std::mutex> locker(m_mutex);

    MAInfo info;
    info.stepZ = m_JsonData["DutProjectorDistance"]["step"].get<float>();

    m_MAInfo = info;
    return info;
}

MAInfo ConfigMAInfo::GetMAInfo()
{
    return m_MAInfo;
}

Result ConfigMAInfo::getMoveOutInputgratingOffset(cv::Point2f &offset)
{
    if (!m_JsonData.contains("MoveOutInputgratingOffset")){
        return Result(false, "inputAlign.json is not exist 'MoveOutInputgratingOffset' field information.");
    }

    json obj = m_JsonData["MoveOutInputgratingOffset"];
    if (!obj.contains("X"))
    {
        return Result(false, "inputAlign.json is not exist 'MoveOutInputgratingOffset' info error.");
    }
    offset.x = obj["X"].get<float>();

    if (!obj.contains("Y"))
    {
        return Result(false, "inputAlign.json is not exist 'MoveOutInputgratingOffset' info error.");
    }
    offset.y = obj["Y"].get<float>();
    return Result();
}

QString ConfigMAInfo::getReticle()
{
    return QString::fromStdString(m_JsonData["Reticle"].get<std::string>());
}

void ConfigMAInfo::getIntergratingSphere(QString &sulution, float &scale)
{
    sulution = QString::fromStdString(m_JsonData["IntergratingSphere"]["Solution"].get<std::string>());
    scale = m_JsonData["IntergratingSphere"]["Scal"].get<float>();
}

Result ConfigMAInfo::getSafePositonSLB(cv::Point3f &pos)
{
    if (!m_jsonSLB.contains("SafePositon")){
        return Result(false, SLB_FILE_NAME.toStdString() + " is not exist 'SafePositon' field information.");
    }

    pos.x = m_jsonSLB["SafePositon"]["X"].get<float>();
    pos.y = m_jsonSLB["SafePositon"]["Y"].get<float>();
    pos.z = m_jsonSLB["SafePositon"]["Z"].get<float>();
    return Result();
}

Result ConfigMAInfo::getAlignPositonSLB(cv::Point3f &pos)
{
    if (!m_jsonSLB.contains("AlignPositon"))
    {
        return Result(false, SLB_FILE_NAME.toStdString() + " is not exist 'AlignPositon' field information.");
    }
    pos.x = m_jsonSLB["AlignPositon"]["X"].get<float>();
    pos.y = m_jsonSLB["AlignPositon"]["Y"].get<float>();
    pos.z = m_jsonSLB["AlignPositon"]["Z"].get<float>();
    return Result();
}

Result ConfigMAInfo::getProjectorTiltSLB(cv::Point2f &pos)
{
    if (!m_jsonSLB.contains("ProjectorTilt"))
    {
        return Result(false, SLB_FILE_NAME.toStdString() + " is not exist 'ProjectorTilt' field information.");
    }
    pos.x = m_jsonSLB["ProjectorTilt"]["X"].get<float>();
    pos.y = m_jsonSLB["ProjectorTilt"]["Y"].get<float>();
    return Result();
}

Result ConfigMAInfo::getSafeTiltSLB(cv::Point2f& pos)
{
    if (!m_jsonSLB.contains("SafeTilt")) {
        return Result(false, SLB_FILE_NAME.toStdString() + " is not exist 'SafePositon' field information.");
    }

    pos.x = m_jsonSLB["SafeTilt"]["X"].get<float>();
    pos.y = m_jsonSLB["SafeTilt"]["Y"].get<float>();
    return Result();
}

Result ConfigMAInfo::getReticleSLB(QString &reticle)
{
    if (!m_jsonSLB.contains("Reticle"))
    {
        return Result(false, SLB_FILE_NAME.toStdString() + " is not exist 'Reticle' field information.");
    }
    reticle = QString::fromStdString(m_jsonSLB["Reticle"].get<std::string>());
    return Result();
}

Result ConfigMAInfo::getIntergratingSphereSLB(QString &sulution, float &scale)
{
    if (!m_jsonSLB.contains("IntergratingSphere"))
    {
        return Result(false, SLB_FILE_NAME.toStdString() + " is not exist 'IntergratingSphere' field information.");
    }

    sulution = QString::fromStdString(m_jsonSLB["IntergratingSphere"]["Solution"].get<std::string>());
    scale = m_jsonSLB["IntergratingSphere"]["Scal"].get<float>();
    return Result();
}

Result ConfigMAInfo::updateSLB(const cv::Point3f &mcPosition, const cv::Point2f &projectorTilt)
{
    json json = m_jsonSLB;
    json["AlignPositon"]["X"] = round(mcPosition.x *1000)/1000.0;
    json["AlignPositon"]["Y"] = round(mcPosition.y * 1000)/1000.0;
    json["AlignPositon"]["Z"] = round(mcPosition.z *1000)/1000.0;

    json["ProjectorTilt"]["X"] = round(projectorTilt.x *1000)/1000.0;
    json["ProjectorTilt"]["Y"] = round(projectorTilt.y *1000)/1000.0;

    std::ofstream ofs(SLB_FILE_NAME.toStdString());
    if (!ofs.is_open())
    {
        return Result(false, "Open config file failed.");
    }
    ofs << json.dump(4);
    ofs.close();

    Load();
    return Result();
}

bool ConfigMAInfo::isDisplayUI()
{
    return m_JsonData["IsDisplay"].get<bool>();
}

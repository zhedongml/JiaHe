#include "FocusConfig.h"
#include "loggingwrapper.h"

FocusConfig::FocusConfig()
{
    if (!Load())
    {
        LoggingWrapper::instance()->error(QString("Focus config load error, %1 load failed.").arg(QString::fromStdString(FILE_NAME)));
    }
}

FocusConfig::~FocusConfig()
{
}

bool FocusConfig::Load()
{
    std::ifstream ifs(FILE_NAME, std::fstream::in);
    if (ifs.fail())
    {
        return false;
    }
    ifs >> m_JsonLightFocus;

    return true;
}

float FocusConfig::GetLightFocus(std::string color)
{
    if (m_JsonLightFocus.contains(color))
        return m_JsonLightFocus[color].get<float>();
    return 0.0;
}

//bool FocusConfig::IsFocusByLight()
//{
//    return m_JsonLightFocus["Auto"].get<bool>();
//}
#include "mtfmodelconfig.h"

MTFModelConfig *MTFModelConfig::self = nullptr;
MTFModelConfig *MTFModelConfig::instance()
{
    if (self == nullptr)
    {
        self = new MTFModelConfig();
    }
    return self;
}

MTFModelConfig::MTFModelConfig()
{
}

MTFModelConfig::~MTFModelConfig()
{
    if (self != nullptr)
        delete self;
}

void MTFModelConfig::load(const char *path)
{
    filepath = path;
    std::ifstream jsonFile(filepath);
    if (jsonFile.is_open())
    {
        std::string contents =
            std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();
        settingJsonObj = Json::parse(contents);
    }
}

std::vector<std::string> MTFModelConfig::getSpecifedFreq()
{
    Json& specifedFreqs = settingJsonObj["SpecifedFreq"];
    if (specifedFreqs.is_array())
    {
        return specifedFreqs.get<std::vector<std::string>>();
    }
    return std::vector<std::string>();
}

std::vector<std::string> MTFModelConfig::getMeasureMTFColor()
{
    Json& specifedFreqs = settingJsonObj["color"];
    if (specifedFreqs.is_array())
    {
        return specifedFreqs.get<std::vector<std::string>>();
    }
    return std::vector<std::string>();
}

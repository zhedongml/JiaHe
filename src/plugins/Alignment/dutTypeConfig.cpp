#include "dutTypeConfig.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>

bool DutTypeConfig::Load(const char* path)
{
    m_path = path;
    std::ifstream ifs(path, std::fstream::in);

    if (ifs.fail())
    {
        return false;
    }

    ifs >> m_JsonControl;
    return true;
}

std::map<std::string, std::string> DutTypeConfig::GetDutTypeConfig()
{
    std::map<std::string, std::string> mapping;

    for (auto& item : m_JsonControl.items()) {
        mapping[item.key()] = item.value();
    }

    return mapping;
}

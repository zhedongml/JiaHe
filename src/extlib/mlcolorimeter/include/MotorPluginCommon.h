// Copyright 2023 MLOptic
#pragma once
#include <string>
#include <vector>
#include <map>

namespace ML {
namespace Motion {
enum class ControllerType{
    PI_,
    ACS
};

struct MotorConfigInfo {
    ControllerType type;
    std::string sn;
    std::vector<std::map<std::string, std::string>> axisInfo;
};
}  // namespace Motion
}  // namespace ML

#pragma once
//#include "json4moderncpp/json.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include "json4moderncpp/json.hpp"

using Json = nlohmann::json;

class MTFModelConfig
{
  public:
    static MTFModelConfig *instance();
    void load(const char *path);
    ~MTFModelConfig();
    std::vector<std::string> getSpecifedFreq();
    std::vector<std::string> getMeasureMTFColor();

  private:
    MTFModelConfig();
    Json settingJsonObj;
    std::string filepath;
    static MTFModelConfig *self;
};

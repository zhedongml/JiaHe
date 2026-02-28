#pragma once
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
#include "visionAlgorithm.h"
#include "./json/json.hpp"

using Json = nlohmann::json;

struct ROIPosConfig
{
    int centerOffset = 40;
    int roiWidth = 60;
    int roiHeight = 60;
};
struct CrossAreaConfig
{
    int FromX;
    int FromY;
    int aoiWidth;
    int aoiHeight;
    int id = 0;
};
struct MTFLimitConfig
{
    int id = 0;
    std::string freqVal = "0";
    int freqId = 0;
    double upperLimit = 23.0;
    double lowerLimit = 20.0;
};
class MTFModelConfig
{
  public:
    static MTFModelConfig *instance();
    void load(const char *path);
    ROIPosConfig getROIPosCfg();
    std::vector<CrossAreaConfig> getAOIConfig();
    std::vector<CrossAreaConfig> getChessboardAOIConfig();
    std::vector<CrossAreaConfig> getRectangleConfig();

    std::vector<CrossAreaConfig> getAutoROIConfig();
    std::vector<CrossAreaConfig *> getCrossAreaConfig();
    void setROIPosCfg(ROIPosConfig cfg);
    std::vector<std::string> getSpecifedFreq();
    std::vector<MTFLimitConfig> getMTFLimit(int crossImgIdx);
    double getFocusLength();
    void setFocusLength(double val);
    double getVid();
    void setVid(double val);
    std::string getDisplayResultFreq();
    ~MTFModelConfig();
    bool updateCrossAreaConfig(std::vector<CrossAreaConfig> crossAreas);
    bool updateRectangleAreaConfig(std::vector<CrossAreaConfig> rectangleAreas);

    std::vector<double> crossesPosition;
    std::string getNyHeaderFromFre(std::string freq);
    std::vector<std::string> getMeasureMTFColor();
  private:
    bool writeFileJson(const char *path, Json json);

  private:
    MTFModelConfig();
    Json settingJsonObj;
    std::string filepath;
    ROIPosConfig roiPosInfo;
    std::vector<CrossAreaConfig *> crossAreaInfo;
    static MTFModelConfig *self;

    double focusLength = 10.0;
    double vid = 1000000;
    std::vector<s_SingleTargetMatch> resultAutoROI;
};
void from_json(const nlohmann::json &, MTFLimitConfig &p);

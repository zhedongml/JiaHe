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

        Json &roiPos = settingJsonObj["ROIPostion"];
        if (!roiPos.is_null())
        {
            roiPosInfo.centerOffset = roiPos["centeroffset"].get<int>();
            roiPosInfo.roiWidth = roiPos["width"].get<int>();
            roiPosInfo.roiHeight = roiPos["height"].get<int>();
        }
        Json &focLen = settingJsonObj["FocusLength"];
        if (!focLen.is_null())
        {
            focusLength = focLen.get<double>();
        }
        Json &vidVal = settingJsonObj["Vid"];
        if (!vidVal.is_null())
        {
            vid = vidVal.get<double>();
        }
    }
}

ROIPosConfig MTFModelConfig::getROIPosCfg()
{
    return roiPosInfo;
}

std::vector<CrossAreaConfig> MTFModelConfig::getAOIConfig()
{
    Json &crossAreas = settingJsonObj["CrossAreas"];
    std::vector<CrossAreaConfig> aoiInfo;
    if (crossAreas.is_array())
    {
        for (int i = 0; i < crossAreas.size(); i++)
        {
            Json item = crossAreas[i];
            CrossAreaConfig cross;
            cross.FromX = item[0];
            cross.FromY = item[1];
            cross.aoiWidth = item[2];
            cross.aoiHeight = item[3];
            cross.id = i;
            aoiInfo.push_back(cross);
        }
    }
    return aoiInfo;
}

std::vector<CrossAreaConfig> MTFModelConfig::getChessboardAOIConfig()
{

    Json &crossAreas = settingJsonObj["ChessBoardAreas"];
    std::vector<CrossAreaConfig> aoiInfo;
    if (crossAreas.is_array())
    {
        for (int i = 0; i < crossAreas.size(); i++)
        {
            Json item = crossAreas[i];
            CrossAreaConfig cross;
            cross.FromX = item[0];
            cross.FromY = item[1];
            cross.aoiWidth = item[2];
            cross.aoiHeight = item[3];
            cross.id = i;
            aoiInfo.push_back(cross);
        }
    }
    return aoiInfo;
}

std::vector<CrossAreaConfig> MTFModelConfig::getRectangleConfig()
{

    Json &crossAreas = settingJsonObj["RectangleAreas"];
    std::vector<CrossAreaConfig> aoiInfo;
    if (crossAreas.is_array())
    {
        for (int i = 0; i < crossAreas.size(); i++)
        {
            Json item = crossAreas[i];
            CrossAreaConfig cross;
            cross.FromX = item[0];
            cross.FromY = item[1];
            cross.aoiWidth = item[2];
            cross.aoiHeight = item[3];
            cross.id = i;
            aoiInfo.push_back(cross);
        }
    }
    return aoiInfo;
}

bool sortWithPosition(const s_SingleTargetMatch lhs, const s_SingleTargetMatch rhs)
{
    int distanceRHS =
        int(std::sqrt(std::pow(rhs.sourceImageCenterPoint.y, 2) + std::pow(rhs.sourceImageCenterPoint.x, 2)));
    int distanceLHS =
        int(std::sqrt(std::pow(lhs.sourceImageCenterPoint.y, 2) + std::pow(lhs.sourceImageCenterPoint.x, 2)));
    return (2 * rhs.sourceImageCenterPoint.y) + distanceRHS > (2 * lhs.sourceImageCenterPoint.y) + distanceLHS;
}

std::vector<CrossAreaConfig> MTFModelConfig::getAutoROIConfig()
{
    visionAlgorithm::GetInstance()->Mode = NONE;
    visionAlgorithm::GetInstance()->Match();
    resultAutoROI = visionAlgorithm::GetInstance()->ResultData;
    std::vector<CrossAreaConfig> aoiInfo;
    int width = visionAlgorithm::GetInstance()->DestinationImage.cols;
    int height = visionAlgorithm::GetInstance()->DestinationImage.rows;
    std::sort(resultAutoROI.begin(), resultAutoROI.end(), sortWithPosition);

    for (int i = 0; i < resultAutoROI.size(); i++)
    {
        CrossAreaConfig cross;
        cross.FromX = resultAutoROI[i].sourceImageCenterPoint.x - int(width / 2);
        cross.FromY = resultAutoROI[i].sourceImageCenterPoint.y - int(height / 2);
        cross.aoiWidth = width;
        cross.aoiHeight = height;
        cross.id = i;
        aoiInfo.push_back(cross);
        crossesPosition.push_back(resultAutoROI[i].dMatchScore);
        crossesPosition.push_back(resultAutoROI[i].dMatchedAngle);
        crossesPosition.push_back(resultAutoROI[i].sourceImageCenterPoint.x);
        crossesPosition.push_back(resultAutoROI[i].sourceImageCenterPoint.y);
    }

    return aoiInfo;
}

std::vector<CrossAreaConfig *> MTFModelConfig::getCrossAreaConfig()
{
    Json &crossAreas = settingJsonObj["CrossAreas"];
    crossAreaInfo.clear();
    if (crossAreas.is_array())
    {
        for (size_t i = 0; i < crossAreas.size(); i++)
        {
            CrossAreaConfig *cross = new CrossAreaConfig();
            cross->FromX = crossAreas[i][0];
            cross->FromY = crossAreas[i][1];
            cross->aoiWidth = crossAreas[i][2];
            cross->aoiHeight = crossAreas[i][3];

            crossAreaInfo.push_back(cross);
        }
    }
    return crossAreaInfo;
}
void MTFModelConfig::setROIPosCfg(ROIPosConfig cfg)
{
    roiPosInfo = cfg;
}

std::vector<std::string> MTFModelConfig::getSpecifedFreq()
{
    Json &specifedFreqs = settingJsonObj["SpecifedFreq"];
    if (specifedFreqs.is_array())
    {
        return specifedFreqs.get<std::vector<std::string>>();
    }
    return std::vector<std::string>();
}

void from_json(const nlohmann::json &j, MTFLimitConfig &p)
{
    j.at(0).get_to(p.freqId);
    j.at(1).get_to(p.lowerLimit);
    j.at(2).get_to(p.upperLimit);
}
std::vector<MTFLimitConfig> MTFModelConfig::getMTFLimit(int crossImgIdx)
{
    std::vector<MTFLimitConfig> result;
    Json &thresholds = settingJsonObj["threshold"];
    if (!thresholds.is_null())
    {
        std::string key = std::to_string(crossImgIdx);
        if (key != "")
        {
            Json val = thresholds.at(key);
            result = val.get<std::vector<MTFLimitConfig>>();
        }
    }
    std::vector<std::string> specFreqs = getSpecifedFreq();
    for (int i = 0; i < result.size(); i++)
    {
        if (i < specFreqs.size())
        {
            result.at(i).freqVal = specFreqs.at(i);
        }
    }

    return result;
}

double MTFModelConfig::getFocusLength()
{
    return focusLength;
}

void MTFModelConfig::setFocusLength(double val)
{
    focusLength = val;
}

double MTFModelConfig::getVid()
{
    return vid;
}

void MTFModelConfig::setVid(double val)
{
    vid = val;
}

std::string MTFModelConfig::getDisplayResultFreq()
{
    Json &displayFreq = settingJsonObj["DisplayFreq"];
    if (!displayFreq.is_null())
    {
        return displayFreq.get<std::string>();
    }
    return "";
}

MTFModelConfig::MTFModelConfig()
{
}

MTFModelConfig::~MTFModelConfig()
{
    if (self != nullptr)
        delete self;
}

bool MTFModelConfig::updateCrossAreaConfig(std::vector<CrossAreaConfig> crossAreas)
{
    Json &crossAreasJson = settingJsonObj["CrossAreas"];
    crossAreasJson.clear();
    if (crossAreasJson.is_array())
    {
        for (size_t i = 0; i < crossAreas.size(); i++)
        {
            CrossAreaConfig cross = crossAreas.at(i);
            crossAreasJson[i][0] = cross.FromX;
            crossAreasJson[i][1] = cross.FromY;
            crossAreasJson[i][2] = cross.aoiWidth;
            crossAreasJson[i][3] = cross.aoiHeight;
        }
        return writeFileJson(filepath.c_str(), settingJsonObj);
    }
    else
    {
        return false;
    }
}

bool MTFModelConfig::updateRectangleAreaConfig(std::vector<CrossAreaConfig> rectangleAreas)
{
    Json &rectangleAreasJson = settingJsonObj["RectangleAreas"];
    rectangleAreasJson.clear();
    if (rectangleAreasJson.is_array())
    {
        for (size_t i = 0; i < rectangleAreas.size(); i++)
        {
            CrossAreaConfig cross = rectangleAreas.at(i);
            rectangleAreasJson[i][0] = cross.FromX;
            rectangleAreasJson[i][1] = cross.FromY;
            rectangleAreasJson[i][2] = cross.aoiWidth;
            rectangleAreasJson[i][3] = cross.aoiHeight;
        }
        return writeFileJson(filepath.c_str(), settingJsonObj);
    }
    else
    {
        return false;
    }
}

std::string MTFModelConfig::getNyHeaderFromFre(std::string freq)
{
    Json &freqMap = settingJsonObj["nyfreqmap"];
    if (!freqMap.is_null())
    {
        return freqMap[freq].get<std::string>();
    }
    return "";
}

bool MTFModelConfig::writeFileJson(const char *path, Json json)
{
    std::ofstream ofs(path, std::ios_base::out);
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << json.dump(4);
    ofs.close();
    return true;
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

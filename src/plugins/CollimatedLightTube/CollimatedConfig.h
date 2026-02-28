#pragma once
#include "json.hpp"
#include "ml.h"
#include <QObject>
#include "collimatedlighttube_global.h"
using Json = nlohmann::ordered_json;
using namespace std;

struct MLCollimatorConfig {
	double focalLength;
	double pixelSize;
	double AngleRange;
	int AdjustTimes;
	cv::Point2f BaseCenter;
};

class COLLIMATEDLIGHTTUBE_EXPORT CollimatedConfig
{
public:
	CollimatedConfig();
	~CollimatedConfig();
	static CollimatedConfig* instance();

public:
	bool ReadCollimatorConfigInfo();
	MLCollimatorConfig GetLevelingConfig();
	std::string GetCollimatorSn();

private:
    static CollimatedConfig* m_instance;
	Json m_JsonControl;
    std::string m_ConfigPath = "./config/device/MindVersionCamera.json";
    MLCollimatorConfig m_CollimatorConfig;
};
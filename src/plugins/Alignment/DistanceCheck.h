#pragma once
#include <json.hpp>
#include "PrjCommon/service/ml_motion.h"
#include <fstream>
#include <iostream>

using Json = nlohmann::json;

struct DistanceCheckInfo{
	float targetDistance;
	float offset;
};

class DistanceCheck
{
public:
	static DistanceCheck& instance();
	~DistanceCheck();

	DistanceCheckInfo getDistanceCheckInfo();
	Result check();
	Result updateTargetDistance(float diatance);

private:
	DistanceCheck();
	Result loadFile();

private:
	const QString FILE_NAME = "./config/AlignConfig/distanceCheck.json";
	Json m_json;
};


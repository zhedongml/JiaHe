#include "limitconfig.h"
#include <fstream>
#include <iostream>
LimitConfig* LimitConfig::self = nullptr;
LimitConfig::LimitConfig(QObject *parent)
	: QObject(parent)
{
}

LimitConfig* LimitConfig::instance(QObject* parent)
{
	if (self == nullptr) {
		self = new LimitConfig(parent);
	}
	self->setParent(parent);
	return self;
}

void LimitConfig::load(const char* path)
{
	std::ifstream jsonFile(path);
	if (jsonFile.is_open()) {
		std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)),
			(std::istreambuf_iterator<char>()));
		jsonFile.close();
		settingJsonObj = Json::parse(contents);
	}
}

LimitConfig::~LimitConfig()
{
}

LimitsInfos LimitConfig::getLimitInfos(std::string metricName)
{
	LimitsInfos result;
	Json& metricJson = settingJsonObj[metricName];
	if (!metricJson.is_null()) {
		result.uplimit = metricJson["uplimit"].get<float>();
		result.lowlimit = metricJson["lowlimit"].get<float>();
	}
	return result;
}

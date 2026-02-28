#pragma once

#include <QObject>
#include "json.hpp"
using Json = nlohmann::json;
struct LimitsInfos {
	double uplimit = -1;
	double lowlimit = -1;
};
class LimitConfig  : public QObject
{
	Q_OBJECT

public:
	static LimitConfig* instance(QObject* parent = nullptr);
	void load(const char* path);
	~LimitConfig();
	LimitsInfos getLimitInfos(std::string metricName);
private:
	LimitConfig(QObject* parent = nullptr);
	Json settingJsonObj;
	std::string filepath;
	static LimitConfig* self;
};

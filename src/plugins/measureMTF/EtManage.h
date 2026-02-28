#pragma once
#include "json/json.hpp"
#include "Result.h"
#include <iostream>
#include <QString>
#include "PrjCommon/metricsdata.h"

using Json = nlohmann::json;

class EtManage
{
public:
	static EtManage& instance();

	Result setting(bool customize, bool save, const QString& dutType = "");
	Result writeData();
	Result getExposureTime(bool& isAuto, float&et);
	bool getCustomize();

private:
	EtManage();
	~EtManage();

	Result loadFile(Json& json, const QString &fileName);
	QString fileByDutType(const QString &dutType);
	QString getKey();

private:
	const QString FILE_PATH = "./config/exposureTime/";
	QString m_fileName;
	Json m_json;

	bool m_save = false;
	bool m_customize = true;
};


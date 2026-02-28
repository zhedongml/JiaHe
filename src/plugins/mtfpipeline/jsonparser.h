#pragma once
#include "processorconfig.h"
#include "json.h"
using namespace std;
class JsonParser
{
private:
	JsonParser();
public:
	bool loadConfig(string path);
	bool writeConfigToFile();
	static JsonParser* getInstance();
private:
	static JsonParser* self;
	void setPreProcConfig(const Json::Value const & jsonObject);
	void setFitProcConfig(const Json::Value const& jsonObject);
	void setLSFProcConfig(const Json::Value const& jsonObject);
	void setESFProcConfig(const Json::Value const& jsonObject);
	void setDFTProcConfig(const Json::Value const& jsonObject);
	void setMTFPostProcConfig(const Json::Value const& jsonObject);
	void setOverSampleProcConfig(const Json::Value const& jsonObject);

	void setPreProcJsonVal(Json::Value&jsonObject);
	void setFitProcJsonVal(Json::Value& jsonObject);
	void setLSFProcJsonVal(Json::Value& jsonObject);
	void setESFProcJsonVal(Json::Value& jsonObject);
	void setDFTProcJsonVal(Json::Value& jsonObject);
	void setOverSampleProcJsonVal(Json::Value& jsonObject);
	void setMTFPostProcJsonVal(Json::Value& jsonObject);
	string configpath = "";
};


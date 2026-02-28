#pragma once
#include <json.hpp>

// 使用 nlohmann/json 的命名空间
//using json = nlohmann::json;

const std::string SENSOR_DUT_TYPE = "./config/dutType.json";

class DutTypeConfig {

public:
	static DutTypeConfig& GetInstance() {
		static DutTypeConfig config;
		return config;
	}

public:
	bool Load(const char* path);
	std::map<std::string, std::string> GetDutTypeConfig();

private:
	nlohmann::ordered_json m_JsonControl;
	std::string m_path;
private:
	DutTypeConfig() {};
	~DutTypeConfig() {};
	DutTypeConfig(DutTypeConfig const&) = delete;
	DutTypeConfig& operator=(DutTypeConfig const&) = delete;

};
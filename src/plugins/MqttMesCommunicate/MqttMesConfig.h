#pragma once
#include "json.hpp"

class MqttMesConfig
{
public:
	static MqttMesConfig& instance()
	{
		static MqttMesConfig instance;
		return instance;
	}

	void loadConfig();
	void saveConfig();

	nlohmann::ordered_json getConfig();
	void setConfig(const nlohmann::ordered_json& config);

private:
	MqttMesConfig() = default;
	~MqttMesConfig() = default;

	MqttMesConfig(const MqttMesConfig&) = delete;
	MqttMesConfig& operator=(const MqttMesConfig&) = delete;

	nlohmann::ordered_json m_config;
	std::string m_configFilePath = "./config/mesconfig.json";
};

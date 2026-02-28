#include "MqttMesConfig.h"
#include <fstream>

void MqttMesConfig::loadConfig()
{
	std::ifstream file(m_configFilePath);
	if (file.is_open())
	{
		file >> m_config;
		file.close();
	}
}

void MqttMesConfig::saveConfig()
{
	//m_config.clear();
	std::ofstream file(m_configFilePath);
	if (file.is_open())
	{
		file << m_config.dump(4);
		file.close();
	}
}

nlohmann::ordered_json MqttMesConfig::getConfig()
{
	loadConfig();
	return m_config;
}

void MqttMesConfig::setConfig(const nlohmann::ordered_json& config)
{
	m_config = config;
	saveConfig();
}

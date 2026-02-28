#include "iqmetricconfig.h"
#include <fstream>
#include <iostream>
#include "limitconfig.h"
IQMetricConfig* IQMetricConfig::self = nullptr;
std::string trim(const std::string &str)
{
    std::string::size_type pos = str.find_first_not_of(' ');
    if (pos == std::string::npos)
    { 
        return str;
    }
    std::string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 != std::string::npos)
        return str.substr(pos, pos2 - pos + 1);
                                               
    return str.substr(pos);
}
IQMetricConfig::IQMetricConfig()
{
}

IQMetricConfig* IQMetricConfig::instance()
{
	if (self == nullptr) {
		self = new IQMetricConfig();
	}
	return self;
}

void IQMetricConfig::load(const char* path)
{
	filepath = path;
	std::ifstream jsonFile(filepath);
	if (jsonFile.is_open()) {
		std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)),
			(std::istreambuf_iterator<char>()));
		jsonFile.close();
		settingJsonObj = Json::parse(contents);
		Json& limitFilePath = settingJsonObj["limitfile"];
		if (!limitFilePath.is_null()) {
		 std::string lmFilePath = limitFilePath.get<std::string>();
		 loadLimitInfos(lmFilePath.c_str());
		}
		else {
			loadLimitInfos("./config/limits.json");
		}
	}

}

void IQMetricConfig::loadLimitInfos(const char* path)
{
	LimitConfig::instance()->load(path);
}

IQMetricConfig::~IQMetricConfig()
{
	if (pMetrics != nullptr) {

		for each (MetricDescription* var in *pMetrics)
		{
			delete var;
		}
		pMetrics->clear();
		delete pMetrics;
	}
}

std::vector<MetricDescription*>* IQMetricConfig::getMetricDescripts()
{
	if (pMetrics == nullptr) {
		pMetrics = new std::vector<MetricDescription*>;
		Json& metricsJson = settingJsonObj["Metrics"];
		if (metricsJson.is_array()) {
			for each (auto var in metricsJson)
			{
				for (auto elem : var.items()) {
					MetricDescription* itemDes = new MetricDescription();
					Json& val = elem.value();
					itemDes->name = trim(elem.key());
                    Json &unit = val["unit"];
                    if (!unit.is_null())
                    {
                        itemDes->unit = val["unit"].get<std::string>();
                    }
                    Json &color = val["color"];
                    if (!color.is_null())
                    {
                        itemDes->colors = val["color"].get<std::vector<std::string>>();
                    }
                    Json &img = val["images"];
                    if (!img.is_null())
                    {
                        itemDes->images = val["images"].get<std::vector<std::string>>();
                    }
                    Json &eyebox = val["eyeboxId"];
                    if (!eyebox.is_null())
                    {
                        itemDes->eyeboxId = val["eyeboxId"].get<std::vector<std::string>>();
                    }
					Json& sufJson = val["suffix"];
					if (!sufJson.is_null()) {
						itemDes->suffix = val["suffix"].get<std::vector<std::string>>();
					}
					Json& csvHeader = val["logheader"];
					if(!csvHeader.is_null())
					  itemDes->logheader = val["logheader"].get<std::string>();

                    Json &csvHeader1 = val["logheader1"];
                    if (!csvHeader1.is_null())
                        itemDes->logheader1 = val["logheader1"].get<std::string>();

					Json imgType = val["imageType"];
					if (!imgType.is_null()) {
						itemDes->imageType = imgType.get<int>();
					}
					Json& relatedJson = val["related"];
					if (!relatedJson.is_null()) {
						itemDes->releate = val["related"].get<std::vector<std::string>>();
					}
                    Json &dispalyFlag = val["dispaly"];
                    if (!dispalyFlag.is_null())
                    {
                        itemDes->dispaly = val["dispaly"].get<bool>();
                    }


					pMetrics->push_back(itemDes);
				}
			}

		}
	}
	return pMetrics;
}

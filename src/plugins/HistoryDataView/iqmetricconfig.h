#pragma once
#include "json4moderncpp/json.hpp"
#include <vector>
#include <String>
using Json = nlohmann::json;
struct MetricDescription {
	std::string name = "";
	std::string unit = "";
	std::vector<std::string> colors;
	std::vector<std::string> images;
	std::vector<std::string> suffix;
    std::vector<std::string> eyeboxId;
    bool dispaly;
	std::string logheader = "";
	std::vector<std::string> releate;
	int imageType = 1; //0 xyY chroma,1 2*2 binning gray Y;2 mtf;3  XYZ image,
};




class IQMetricConfig 
{
public:
	static IQMetricConfig* instance();
	void load(const char* path);
	~IQMetricConfig();
	std::vector<MetricDescription*>* getMetricDescripts();
private:
	IQMetricConfig();
	Json settingJsonObj;
	std::string filepath;
	static IQMetricConfig* self;
	std::vector<MetricDescription*>* pMetrics = nullptr;
};

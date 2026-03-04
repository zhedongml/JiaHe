#pragma once

#include <json.hpp>
#include <fstream>
#include <vector>

#ifdef ALIGNMENT_LIB
#define CONFIG_API __declspec(dllexport)
#else
#define CONFIG_API __declspec(dllimport)
#endif

const std::string WAFER_FILE_NAME = "./config/waferConfig.json";

struct DutScanPos
{
	double x;
	double y;
};

struct DutFiducialViewPos
{
	double x;
	double y;
};

struct DutRangingPos
{
	double x;
	double y;
	double z;
};

struct DutParallelAdjustmentPos
{
	double x;
	double y;
	double z;
};

struct waferConfigInfo
{
	std::string waferName;
	int dutNum;
	double scanZPos;

	std::map<int, DutScanPos>  dutScanPos_map;
	std::map<int, DutFiducialViewPos>  dutFiducialViewPos_map;
	std::map<int, DutRangingPos>  dutRangingPos_map;
	std::map<int, DutParallelAdjustmentPos>  dutParallelAdjustmentPos_map;
};

class CONFIG_API waferConfig
{
public:
	static waferConfig& GetInstance() {
		static waferConfig config;
		return config;
	}

public:
	bool Load(const char* path);
	std::map<std::string, waferConfigInfo> GetWaferConfigInfo();

private:
	nlohmann::ordered_json m_JsonControl;
	std::string m_path;
	std::map<std::string, waferConfigInfo> m_waferConfigInfoMap;
private:
	waferConfig() {};
	~waferConfig() {};
	waferConfig(waferConfig const&) = delete;
	waferConfig& operator=(waferConfig const&) = delete;
};


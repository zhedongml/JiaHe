#pragma once

#include <json.hpp>
#include <fstream>
#include <vector>
//using json = nlohmann::ordered_json;//using json = nlohmann::json;

#ifdef ALIGNMENT_LIB
#define CONFIG_API __declspec(dllexport)
#else
#define CONFIG_API __declspec(dllimport)
#endif

const std::string PROCESS_FILE_NAME = "./config/processConfig.json";

struct pos3D
{
	double x;
	double y;
	double z;
};

struct pos2D
{
	double x;
	double y;
};

struct LoadPos
{
	pos3D dutModule;
};

struct ParallelAdjustmentPos
{
	pos3D adjustPos;
};

struct RangingPos
{
	pos3D rangingPos;
};

struct Anticollision
{
	double dutMotorAngleMax;
	double dutMotorAngleMin;
	double inputAlignmentMotorPosXMax;
	double inputAlignmentMotorPosXMin;
	double inputAlignmentMotorPosYMax;
	double inputAlignmentMotorPosYMin;
	double rangingMax;
	double rangingMin;
};

struct OffsetRoatate
{
	std::map<std::string, pos2D> projectionTiptilt;
	std::map<std::string, pos2D> projectionOffsetRelativeToMV;

	//std::map<std::string, pos3D> initImagingMeasurePos;
	std::map<std::string, pos2D> imagingTiptilt;
	std::map<std::string, pos2D> imagingOffsetRelativeToMV;
	std::map<std::string, double> eyeBoxCenterKeyenceValue;
	std::map<std::string, pos3D> imagingFixedPos;
	std::map<std::string, Anticollision> anticollision;
	std::map<std::string, LoadPos> loadPos;
	std::map<std::string, RangingPos> rangingPos;
	std::map<std::string, ParallelAdjustmentPos> parallelAdjustmentPos;
};



struct processConfigInfo
{
	//LoadPos loadPos;
	//ParallelAdjustmentPos parallelAdjustmentPos;
	//RangingPos rangingPos;
	OffsetRoatate offsetRoatate;
};

class CONFIG_API processConfig
{
public:
	static processConfig& GetInstance() {
		static processConfig config;
		return config;
	}

public:
	bool Load(const char* path);
	processConfigInfo GetProcessConfigInfo();

private:
	nlohmann::ordered_json m_JsonControl;
	std::string m_path;
	processConfigInfo m_processConfigInfo;
private:
	processConfig() {};
	~processConfig() {};
	processConfig(processConfig const&) = delete;
	processConfig& operator=(processConfig const&) = delete;
};


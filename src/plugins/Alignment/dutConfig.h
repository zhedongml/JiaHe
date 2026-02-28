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

const std::string DUT_FILE_NAME = "./config/dutconfig.json";

struct originPointMotorPositionInMV
{
	double x;
	double y;
};

struct offsetPos
{
	double x;
	double y;
};

struct offsetPos3D
{
	double x;
	double y;
	double z;
};

struct dutConfigInfo
{
	std::string dutName;
	std::string eyeType;
	//double eyeRelief;
	double walkoffDistance;
	double eyeBoxPupil;
	double fiducialZMotorPos;
	originPointMotorPositionInMV originPointMotorPositionInMV_;
	std::map<int, offsetPos> fiducialOffset_;
	offsetPos inputgratingOffset_;
	offsetPos QROffset_;
	std::map<int, offsetPos3D> outputgratingOffset_;
};

class CONFIG_API dutConfig
{
public:
	static dutConfig& GetInstance() {
		static dutConfig config;
		return config;
	}

public:
	bool Load(const char* path);
	std::map<std::string, dutConfigInfo> GetDutConfigInfo();

private:
	nlohmann::ordered_json m_JsonControl;
	std::string m_path;
	std::map<std::string, dutConfigInfo> m_dutConfigInfoMap;
private:
	dutConfig() {};
	~dutConfig() {};
	dutConfig(dutConfig const&) = delete;
	dutConfig& operator=(dutConfig const&) = delete;
};


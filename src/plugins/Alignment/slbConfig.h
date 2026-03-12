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

const std::string SLB_FILE_NAME = "./config/slbconfig.json";

struct acsPos3D
{
	double x;
	double y;
	double z;
};

struct tiptilt2D
{
	double dx;
	double dy;
};

struct slbConfigInfo
{
	acsPos3D slb_LoadImageXYZPosition;
	acsPos3D slb_ImagingXYZPosition;
	acsPos3D slb_LoadDutXYZPosition;
	tiptilt2D slb_projectionTiptilt;
	tiptilt2D slb_imagingTiptilt;
};

class CONFIG_API slbConfig
{
public:
	static slbConfig& GetInstance() {
		static slbConfig config;
		return config;
	}

public:
	bool Load(const char* path);
	slbConfigInfo GetSlbConfigInfo();
private:
	nlohmann::ordered_json m_JsonControl;
	std::string m_path;
private:
	slbConfig() {};
	~slbConfig() {};
	slbConfig(slbConfig const&) = delete;
	slbConfig& operator=(slbConfig const&) = delete;
};


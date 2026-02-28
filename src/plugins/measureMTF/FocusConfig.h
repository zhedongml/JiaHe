#ifndef AUTOFOCUSCONFIG_H
#define AUTOFOCUSCONFIG_H

#include <iostream>
#include <json.hpp>
#include <fstream>
#include <QString>
//#include "PrjCommon/PrjCommon.h"

using namespace std;
using Json = nlohmann::json;

class __declspec(dllexport) FocusConfig
{
public:
	static FocusConfig& GetInstance() {
		static FocusConfig config;
		return config;
	}

public:
	//bool IsFocusByLight();
	float GetLightFocus(std::string color);


private:
	FocusConfig();
	~FocusConfig();
	FocusConfig(FocusConfig const&) = delete;
	FocusConfig& operator=(FocusConfig const&) = delete;
	bool Load();

private:
	const string FILE_NAME = "./config/mlcolorimeter/EYE1/LightFocusConfig.json";
	Json m_JsonLightFocus;
};

#endif
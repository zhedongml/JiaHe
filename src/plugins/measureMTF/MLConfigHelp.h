#pragma once
#include <iostream>
#include <json.hpp>
#include <fstream>
#include "MLTaskCommon.h"
#include "Result.h"
#include <QString>

using namespace ML_Task;

using namespace std;
using Json = nlohmann::json;

class MLConfigHelp
{
public:
	static MLConfigHelp& GetInstance() {
		static MLConfigHelp self;
		return self;
	}

public:
	Result GetExposureCheckParam(ExposureCheckParam& param);

private:
	MLConfigHelp();
	~MLConfigHelp();
	MLConfigHelp(MLConfigHelp const&) = delete;
	MLConfigHelp& operator=(MLConfigHelp const&) = delete;

private:
	const string EXPOSURECHECK_FILE_NAME = "./config/mlcolorimeter/EYE1/ExposureCheckConfig.json";
};

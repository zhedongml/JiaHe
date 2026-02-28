#pragma once
#include "plccontrol_global.h"
#include <iostream>
#include <json.hpp>
#include <fstream>
#include <QMap>
using namespace std;
//using json = nlohmann::json;

struct PLCCONTROL_EXPORT PLCCell
{
	int index;

	int size;

	QString name;

	QString offset;

	QString on;

	QString off;

	int isDdefault;

	int readoniy;

	int visible;
};

struct PLCCONTROL_EXPORT ConnectInfo
{
	string ip;

	int rack;

	int slot;
};

class PLCCONTROL_EXPORT PLCConfig
{
public:
	static PLCConfig& GetInstance()
	{
		static PLCConfig config;
		return config;
	}

public:
	bool Load();
	QMap<int, PLCCell> ReadInfo();

	ConnectInfo GetConnectInfo();

	bool GetInterLockDefaultValue();

	void getItemOffset(QString& name, int& off_byte, int& off_bit);

private:
	PLCConfig() {};
	~PLCConfig() {};
	PLCConfig(PLCConfig const&) = delete;
	PLCConfig& operator=(PLCConfig const&) = delete;

private:
	nlohmann::json m_JsonControl;

	std::string m_path;
	const string configPath = "./config/device/PLCConfig.json";

};

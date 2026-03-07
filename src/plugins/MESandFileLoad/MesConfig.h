#pragma once
#include <QList>
#include <QMap>
#include <QString>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <vector>
#include "mesandfileload_global.h"

using Json = nlohmann::json;
using namespace std;

class MESANDFILELOAD_EXPORT MesConfig
{
public:
	static MesConfig& instance();
	~MesConfig();

	void getTcpInfo(QString &ip, int &port);
	bool getSWDebug();

	QString getSingleLensLoadURL();
	QString getSingleLensUnloadURL();
	QString getCuttingPreventURL();

	bool getMesOnline();

	QString getMac();
	QString getADcode();
	QString getMesToken();
	bool writeTrayInfo(QString type, QString code);
	bool writeTray(bool isOK, QString code);

	QString getCbpcodeNG();
	QString getTrayCodeOK();
	QString getTrayCodeNG();

private:
	MesConfig();
	Json load(QString fileName);
	bool mesHttpTest();

private:
	const QString FILE_NAME = "./config/QdpMes/Mes.json";
	const QString FILE_NAME_PARAM = "./config/QdpMes/MesParam.json";
	Json m_json;
	Json m_jsonParam;
};


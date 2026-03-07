#pragma once
#include "Result.h"
#include "mesandfileload_global.h"
#include "PrjCommon/metricsdata.h"
#include <QList>
#include "TcpModel.h"
#include "MesCommon.h"

using namespace std;

class TcpModel;

class MESANDFILELOAD_EXPORT MesMode
{
public:
	static MesMode& instance();
	MesMode();
	~MesMode();

	Result singleLensLoad();
	Result singleLensUnload();
	Result singleLensUnload(bool success);
	Result setCuttingPrevent(QString trayCode);

	Result sendTcp(ExternalTOSPathbody data);
	Result sendTcpRD(ExternalTOSPathToRDbody data);

	Result zipDataUpload();
	Result fileDataUpload();

	Result zipUpload(const ExternalTOSPathbody& data, const std::vector<std::string>& dirPaths, const std::vector<std::string>& zipPaths);
	Result zipUploadAsync(const ExternalTOSPathbody& data, const std::vector<std::string>& dirPaths, const std::vector<std::string>& zipPaths);

	Result loadAndRunAdpTask();
	Result runAdpTask(DutMeasureInfo info);

	void notifyStop(bool isstop);

	void setMesBaseInfo(const ControlData &data);

	int getTestCount();

	void DetectIncompleteAdpTask();

private:
	string getUse(bool isZip);
	Result compressedFiles();
	Result getZipPaths(const std::vector<std::string>& imgPathsRelative, std::vector<std::string>&dirPaths, std::vector<std::string>&zipPaths);

private:
	TcpModel *m_dataUpload = nullptr;
	std::atomic_bool m_zipStop;

	ControlData m_mesBaseInfo;
	QString m_judgment = "1";
};


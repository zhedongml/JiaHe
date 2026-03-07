#pragma once
#include "PrjCommon/taskManage/TaskManager.h"
#include "Result.h"
#include "MesCommon.h"
#include "AutoDP/QDFData.h"
#include "AutoDP/ImageProcessor.h"

using namespace PrjCommon;

class MesTaskAsync : public QObject, public TaskCallback
{
	Q_OBJECT
public:
	static MesTaskAsync& instance(QObject* parent = nullptr);
	~MesTaskAsync();

	Result runQdp(const std::string& sourcePath, const std::string& resultPath);

	Result uploadZip(const ExternalTOSPathbody& data, const std::vector<std::string>& dirPaths, const std::vector<std::string>& zipPaths);

	Result stopTask();
	void stopThread();
	Result waitAllTask();
	Result judgeEndMetricsTask();

	void notifyResult(Result ret) override;

private:
	MesTaskAsync(QObject* parent = nullptr);

private:
	//TaskManager m_managerZip;
	TaskManager *m_managerQdp = nullptr;
};


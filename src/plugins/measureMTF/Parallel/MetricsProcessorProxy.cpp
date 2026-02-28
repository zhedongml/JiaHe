#include <Windows.h>
#include "MetricsProcessorProxy.h"
#include "ImageDataManager.h"
#include <QThreadPool>
#include "json.hpp"
#include <fstream>
#include <iostream>
#include "iqmetricconfig.h"
#include "Core/loggingwrapper.h"
#include "iqtmodel.h"

using namespace std;
using Json = nlohmann::json;

using namespace IQ_Parallel_NS;

MetricsProcessorProxy* MetricsProcessorProxy::GetInstance()
{
	static MetricsProcessorProxy self;
	return &self;
}

MetricsProcessorProxy::MetricsProcessorProxy(QObject* parent)
{
	IQTModel::instance();

	//Init();
}

Result MetricsProcessorProxy::StartParallelCalculate(QStringList metricslist, QStringList eyeboxlist)
{
	try
	{
		ClearCache();

		std::vector<MetricDescription*>* metricVec = IQMetricConfig::instance()->getMetricDescripts();

		for each (MetricDescription * var in *metricVec)
		{
			QString taskName = QString::fromStdString(var->name);

			if (!metricslist.contains(taskName, Qt::CaseSensitivity::CaseInsensitive))
				continue;

			pMetrics.insert(std::pair<std::string, MetricDescription*>(var->name, var));
			_qHashThreadTask.insert(QString::fromStdString(var->name), new IQ_ParallelTask(var, eyeboxlist, getVirtualCameraMode()));
		}

		//std::cout << _qHashThreadTask.size() << endl;

		QHash<QString, IQ_ParallelTask*>::const_iterator it = _qHashThreadTask.constBegin();

		while (it != _qHashThreadTask.constEnd())
		{
			it.value()->Start();
			++it;
		}

		QString message = QString("MetricsProcessorProxy: start parallel calculate tasks successfully.");
		LoggingWrapper::instance()->info(message);

	}
	catch (const std::exception& e)
	{
		qInfo() << "错误: " << e.what() << '\n';
		QString message = QString("MetricsProcessorProxy: start parallel calculate failed. %1.").arg(QString::fromStdString(e.what()));
		LoggingWrapper::instance()->error(message);
		return Result(false, e.what());
	}

	return Result();
}

Result MetricsProcessorProxy::StopParallelCalculate()
{
	setVirtualCameraMode(false);

	QHash<QString, IQ_ParallelTask*>::const_iterator it = _qHashThreadTask.constBegin();

	while (it != _qHashThreadTask.constEnd())
	{
		it.value()->Stop();
		++it;
	}

	emit Signal_Abort_DUT();

	ImageDataManager::GetInstance()->Clear();
	QString message = QString("MetricsProcessorProxy: stop parallel calculate tasks successfully.");
	LoggingWrapper::instance()->info(message);

	return Result();
}

Result MetricsProcessorProxy::RestartParallelCalculate()
{
	if (IQ_TaskState::Idle == getRunningStatus())
		return Result();

	QHash<QString, IQ_ParallelTask*>::const_iterator it = _qHashThreadTask.constBegin();

	while (it != _qHashThreadTask.constEnd())
	{
		it.value()->Restart(m_VirtualCameraMode);
		++it;
	}

	return Result();// StartParallelCalculate();
}

void MetricsProcessorProxy::ClearCache()
{
	ImageDataManager::GetInstance()->Clear();
}

//bool MetricsProcessorProxy::ReadAllFile(const std::string& strFileName, std::string& strFileData)
//{
//	std::ifstream in(strFileName, std::ios::in | std::ios::binary);
//	if (!in.is_open())
//	{
//		return false;
//	}
//	std::istreambuf_iterator<char> beg(in), end;
//	strFileData = std::string(beg, end);
//	in.seekg(0, std::ios::end);//移动的文件尾部
//	int strFileSize = in.tellg();//获取文件的整体大小
//	in.close();
//	return true;
//}

//std::string trim(const std::string& str)
//{
//	std::string::size_type pos = str.find_first_not_of(' ');
//	if (pos == std::string::npos)
//	{
//		return str;
//	}
//	std::string::size_type pos2 = str.find_last_not_of(' ');
//	if (pos2 != std::string::npos)
//		return str.substr(pos, pos2 - pos + 1);
//
//	return str.substr(pos);
//}

//int MetricsProcessorProxy::LoadJsonFile()
//{
//	//filepath = path;
//	std::ifstream jsonFile("IQMetricConfig.json");
//	if (jsonFile.is_open()) {
//		std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)),
//			(std::istreambuf_iterator<char>()));
//		jsonFile.close();
//		Json settingJsonObj = Json::parse(contents);
//
//		Json& metricsJson = settingJsonObj["Metrics"];
//
//		if (metricsJson.is_array())
//		{
//			for (auto var : metricsJson)
//			{
//				for (auto elem : var.items())
//				{
//					MetricDescription* itemDes = new MetricDescription();
//					Json& val = elem.value();
//					itemDes->name = trim(elem.key());
//					Json& unit = val["unit"];
//					if (!unit.is_null())
//					{
//						itemDes->unit = val["unit"].get<std::string>();
//					}
//					Json& color = val["color"];
//					if (!color.is_null())
//					{
//						itemDes->colors = val["color"].get<std::vector<std::string>>();
//					}
//					Json& img = val["images"];
//					if (!img.is_null())
//					{
//						itemDes->images = val["images"].get<std::vector<std::string>>();
//					}
//					Json& eyebox = val["eyeboxId"];
//					if (!eyebox.is_null())
//					{
//						itemDes->eyeboxId = val["eyeboxId"].get<std::vector<std::string>>();
//					}
//					Json& sufJson = val["suffix"];
//					if (!sufJson.is_null()) {
//						itemDes->suffix = val["suffix"].get<std::vector<std::string>>();
//					}
//					Json& csvHeader = val["logheader"];
//					if (!csvHeader.is_null())
//						itemDes->logheader = val["logheader"].get<std::string>();
//
//					Json imgType = val["imageType"];
//					if (!imgType.is_null()) {
//						itemDes->imageType = imgType.get<int>();
//					}
//					Json& relatedJson = val["related"];
//					if (!relatedJson.is_null()) {
//						itemDes->releate = val["related"].get<std::vector<std::string>>();
//					}
//					Json& dispalyFlag = val["dispaly"];
//					if (!dispalyFlag.is_null())
//					{
//						itemDes->dispaly = val["dispaly"].get<bool>();
//					}
//
//					pMetrics.insert(std::pair<std::string, MetricDescription*>(itemDes->name, itemDes));
//				}
//			}
//		}
//	}
//	return 0;
//}

MetricsProcessorProxy::~MetricsProcessorProxy()
{
	//StopParallelCalculate();

	qDeleteAll(_qHashThreadTask);

	_qHashThreadTask.clear();
}

int MetricsProcessorProxy::GetImageRefCount(QString _Name)
{
	int refCount = 0;

	QHash<QString, IQ_ParallelTask*>::const_iterator iter = _qHashThreadTask.constBegin();

	for (auto iter = _qHashThreadTask.begin(); iter != _qHashThreadTask.end(); iter++)
	{
		refCount += iter.value()->GetImageRefCount(_Name);

		if (refCount > 0)
			break;
	}

	return refCount;
}

IQ_TaskState IQ_Parallel_NS::MetricsProcessorProxy::getRunningStatus()
{
	QHash<QString, IQ_ParallelTask*>::const_iterator it = _qHashThreadTask.constBegin();

	while (it != _qHashThreadTask.constEnd())
	{
		if (IQ_TaskState::Busy == it.value()->getRunningStatus())
		{
			return IQ_TaskState::Busy;
		}
		++it;
	}

	setVirtualCameraMode(false);
	emit Signal_ParallelCalc_Finished();
	return IQ_TaskState::Idle;
}

void IQ_Parallel_NS::MetricsProcessorProxy::setVirtualCameraMode(bool value)
{
	m_VirtualCameraMode = value;
}

bool IQ_Parallel_NS::MetricsProcessorProxy::getVirtualCameraMode()
{
	return m_VirtualCameraMode;
}

#include "CollimatedConfig.h"
#include "loggingwrapper.h"
#include <fstream>

CollimatedConfig* CollimatedConfig::m_instance = nullptr;

CollimatedConfig::CollimatedConfig()
{
	ReadCollimatorConfigInfo();
}

CollimatedConfig::~CollimatedConfig()
{
}

CollimatedConfig* CollimatedConfig::instance()
{
	if (!m_instance)
	{
		static std::mutex mutex;
		std::unique_lock<std::mutex> locker(mutex);
		if (!m_instance)
		{
			m_instance = new CollimatedConfig();
		}
	}
	return m_instance;
}

bool CollimatedConfig::ReadCollimatorConfigInfo()
{
	string message;
	try
	{

		std::ifstream ifs(m_ConfigPath.c_str(), std::ifstream::in);
		if (ifs.fail())
		{
			LoggingWrapper::instance()->error(
				QString::fromStdString("CollimatedConfig: load " + m_ConfigPath + " failed"));
			return false;
		}
		ifs >> m_JsonControl;

		if (m_JsonControl.contains("CollimatorParallelAdjustment"))
		{
			m_CollimatorConfig.AdjustTimes = m_JsonControl["CollimatorParallelAdjustment"]["AdjustTimes"].get<int>();
			m_CollimatorConfig.AngleRange = m_JsonControl["CollimatorParallelAdjustment"]["AngleRange"].get<int>();
			m_CollimatorConfig.focalLength = m_JsonControl["CollimatorParallelAdjustment"]["focalLength"].get<double>();
			m_CollimatorConfig.pixelSize = m_JsonControl["CollimatorParallelAdjustment"]["pixelSize"].get<double>();
			float baseCenter_x = m_JsonControl["CollimatorParallelAdjustment"]["BaseCenter"]["X"].get<float>();
			float baseCenter_y = m_JsonControl["CollimatorParallelAdjustment"]["BaseCenter"]["Y"].get<float>();
			m_CollimatorConfig.BaseCenter = cv::Point2f(baseCenter_x, baseCenter_y);
		}
		return true;
	}
	catch (const std::exception e)
	{
		std::string ex = e.what();
		message = "Read config exception, " + ex;
		LoggingWrapper::instance()->error(QString::fromStdString(message));
		return false;
	}
}

MLCollimatorConfig CollimatedConfig::GetLevelingConfig()
{
	return m_CollimatorConfig;
}

std::string CollimatedConfig::GetCollimatorSn()
{
	std::string sn;
	if (m_JsonControl.contains("MindVisionConnectionInfo"))
	{
		sn = m_JsonControl["MindVisionConnectionInfo"]["SN"].get<std::string>();
		return sn;
	}
	return "";
}

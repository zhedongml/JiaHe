#include "IQ_ParallelTask.h"
#include <QEventLoop>
#include <regex>
#include <QTime>
#include <QTimer>
#include <QtConcurrent>
#include <algorithm>
#include "ImageDataManager.h"
#include "CameraSimulator.h"
#include "IQ_BasicDataType.h"
#include "Core/loggingwrapper.h"
#include "iqtmodel.h"
#include "PrjCommon/metricsdata.h"

//#include "EtManage.h"

using namespace IQ_Parallel_NS;

IQ_Parallel_NS::IQ_ParallelTask::IQ_ParallelTask(MetricDescription* _pConfog, QStringList _eyeboxlist, bool _vcm)
{
	if (nullptr != _pConfog && _eyeboxlist.size() > 0)
	{
		m_pConfig = _pConfog;
		m_VirtualCameraMode = _vcm;
		m_eyeboxlist = _eyeboxlist;

		//bool ok = connect(ImageDataManager::GetInstance(), &ImageDataManager::Signal_RecvSpecificImage,
		//	this, &IQ_ParallelTask::Slot_RecvSpecificImage, Qt::DirectConnection);// , Qt::DirectConnection); //, Qt::UniqueConnection Qt::DirectConnection
		Init();
	}
}

IQ_Parallel_NS::IQ_ParallelTask::~IQ_ParallelTask()
{
	Stop();
}

void IQ_Parallel_NS::IQ_ParallelTask::Init()
{
	if ("PupilSwim" == m_pConfig->name)
	{
		int i = 0;
	}

	m_SummaryTable.clear();

	for (auto eyeboxId : m_pConfig->eyeboxId)
	{
		if (m_eyeboxlist.size() > 0 && false == m_eyeboxlist.contains(QString::fromStdString(eyeboxId), Qt::CaseInsensitive))
			continue;

		std::vector<std::string> exps = ReplaceSpecialString(m_pConfig->images, "$eyeboxId$", eyeboxId);

		if (SearchSubstringCount(exps, "$color$") > 0)
		{
			for (auto color : m_pConfig->colors)
			{
				std::vector<std::string> color_exps = ReplaceSpecialString(exps, "$color$", color);

				m_SummaryTable.push_back(convertVectorToQStringList(color_exps));
			}
		}
		else
		{
			m_SummaryTable.push_back(convertVectorToQStringList(exps));
		}
	}

	if (m_VirtualCameraMode)
		getValidSummary4VirtualCamera();

	PrintSummaryTable();
}

void IQ_Parallel_NS::IQ_ParallelTask::PrintSummaryTable()
{
	if (m_SummaryTable.size() < 1)
		return;

	QString message = QString("IQ_ParallelTask: [%1] now the summary table : ").arg(QString::fromStdString(m_pConfig->name));

	std::vector<QStringList>::const_iterator iter;

	for (iter = m_SummaryTable.begin(); iter != m_SummaryTable.end(); iter++)
	{
		message += "{" + (*iter).join(",") + "},";
	}
	message.chop(1);
	message += " Working:";

	for (iter = m_WorkingTable.begin(); iter != m_WorkingTable.end(); iter++)
	{
		message += "{" + (*iter).join(",") + "},";
	}

	message.chop(1);
	LoggingWrapper::instance()->debug(message);
}

void IQ_Parallel_NS::IQ_ParallelTask::Start()
{
	m_UserAbort = false;
	m_Thread = std::thread(&IQ_ParallelTask::Run, this);
	m_Pause = false;
	/*while (IQ_TaskState::Idle == getRunningStatus())
	{
	}*/
}

void IQ_Parallel_NS::IQ_ParallelTask::Stop()
{
	m_UserAbort = true;
	emit Signal_MandatoryStop();

	m_Pause = false;
	m_Condition.notify_all();


	if (m_Thread.joinable())
	{
		m_Thread.join();
	}

}

void IQ_Parallel_NS::IQ_ParallelTask::Restart(bool _vcm)
{
	m_VirtualCameraMode = _vcm;
	pause();
	Init();
	resume();
}


std::string IQ_Parallel_NS::IQ_ParallelTask::GetName()
{
	return m_pConfig->name;
}

std::vector<QStringList> IQ_Parallel_NS::IQ_ParallelTask::GetSummaryTable()
{
	return m_SummaryTable;
}

int IQ_Parallel_NS::IQ_ParallelTask::GetImageRefCount(QString _Name)
{
	if (false == m_Running)
		return 0;

	int refCount = 0;

	std::vector<QStringList>::const_iterator iter;

	for (iter = m_WorkingTable.begin(); iter != m_WorkingTable.end(); iter++)
	{
		refCount += (*iter).count(_Name);
		if (refCount > 0)
			return refCount;
	}

	for (iter = m_SummaryTable.begin(); iter != m_SummaryTable.end(); iter++)
	{
		refCount += (*iter).count(_Name);
		if (refCount > 0)
			return refCount;
	}

	return refCount;
}

IQ_TaskState IQ_Parallel_NS::IQ_ParallelTask::getRunningStatus()
{
	if (m_Running)
	{
		return IQ_TaskState::Busy;
	}
	else
	{
		return IQ_TaskState::Idle;
	}
}

void IQ_Parallel_NS::IQ_ParallelTask::Delay_MSec(unsigned int msec)
{
	QEventLoop loop;
	QTimer::singleShot(msec, &loop, SLOT(quit()));
	loop.exec();
}

void IQ_Parallel_NS::IQ_ParallelTask::Slot_RecvSpecificImage(QString Name)
{
	//UpdateCalcQueue();
}

IQ_Parallel_NS::IQ_WaitResult IQ_Parallel_NS::IQ_ParallelTask::WaitRecvRequiredImages(int sec)
{
	QEventLoop loop;
	connect(this, &IQ_ParallelTask::Signal_MandatoryStop, &loop, &QEventLoop::quit);
	connect(this, &IQ_ParallelTask::Signal_WaitEventStop, &loop, &QEventLoop::quit);

	QTimer timer;
	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	timer.start(sec * 1000);

	loop.exec();

	if (false == timer.isActive())
	{
		return IQ_WaitResult::TimeOut;
	}

	if (m_UserAbort)
	{
		return IQ_WaitResult::Abort;
	}

	return IQ_WaitResult::Success;
}

QStringList IQ_Parallel_NS::IQ_ParallelTask::UpdateCalcQueue()
{
	std::shared_lock<std::shared_mutex> lock(rw_mtx);

	QStringList result;

	if (CheckCalcCondition(result))
	{
		m_WorkingTable.push_back(result);
		m_SummaryTable.erase(std::remove(m_SummaryTable.begin(), m_SummaryTable.end(), result), m_SummaryTable.end());
		emit Signal_WaitEventStop();
	}
	return result;
}

void IQ_Parallel_NS::IQ_ParallelTask::RemoveFromSummary(QStringList freelist)
{
	std::unique_lock<std::shared_mutex> lock(rw_mtx);

	if (0 == freelist.size())
		return;

	m_SummaryTable.erase(std::remove(m_SummaryTable.begin(), m_SummaryTable.end(), freelist), m_SummaryTable.end());
	m_WorkingTable.erase(std::remove(m_WorkingTable.begin(), m_WorkingTable.end(), freelist), m_WorkingTable.end());
}

bool IQ_Parallel_NS::IQ_ParallelTask::CheckCalcCondition(QStringList& result)
{
	std::vector<QStringList>::const_iterator iter;
	int cnt = 0;
	for (iter = m_SummaryTable.begin(); iter != m_SummaryTable.end(); iter++)
	{
		QStringList list = (*iter);
		cnt = 0;
		for (auto name : list) {
			cnt += ImageDataManager::GetInstance()->IsImageExist(name);
		}

		if (0 == cnt) {
			result = list;
			return true;
		}
		else
			continue;
	}

	return false;
}

void IQ_Parallel_NS::IQ_ParallelTask::pause()
{
	m_Pause = true;
	emit Signal_WaitEventStop();
}

void IQ_Parallel_NS::IQ_ParallelTask::resume()
{
	m_Pause = false;
	m_Condition.notify_all();
}

void IQ_Parallel_NS::IQ_ParallelTask::Run()
{
	m_Running = true;

	std::cout << m_pConfig->name << " ÈÎÎñ¿ªÊ¼" << std::endl;

	IQ_WaitResult result;

	while (!m_UserAbort)
	{
		if (m_Pause)
		{
			unique_lock<mutex> locker(_mutex);
			while (m_Pause)
			{
				m_Condition.wait(locker); // Unlock _mutex and wait to be notified
			}
			locker.unlock();
		}

		if ((0 == m_SummaryTable.size()) && (0 == m_WorkingTable.size()))
			break;

		QStringList m_NowCalculationQueue = UpdateCalcQueue();

		if (m_NowCalculationQueue.size() > 0)
		{
			QString message = QString("IQ_ParallelTask: [%1] CalculationQueue {%2}.")
				.arg(QString::fromStdString(m_pConfig->name))
				.arg(m_NowCalculationQueue.join(","));
			LoggingWrapper::instance()->debug(message);
			result = IQ_WaitResult::Success;
		}
		else
		{
			if (1)//false)//m_VirtualCameraMode)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				result = IQ_WaitResult::TimeOut;
			}
			else
			{
				result = WaitRecvRequiredImages(100);
			}
		}

		if (IQ_WaitResult::Success == result)
		{
			//QString message = QString("IQ_ParallelTask: [%1] WaitResult::Success.")
			//	.arg(QString::fromStdString(m_pConfig->name));
			//LoggingWrapper::instance()->info(message);

			if (0 == m_NowCalculationQueue.size())
				continue;

			//QStringList deleteList(m_NowCalculationQueue);

			QtConcurrent::run([=]() {
				MetricCalculationMethod(m_NowCalculationQueue);
				RemoveFromSummary(m_NowCalculationQueue);
				ImageDataManager::GetInstance()->
					FreeImagesByNameList(QString::fromStdString(m_pConfig->name), m_NowCalculationQueue);
				});	

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else if (IQ_WaitResult::Abort == result)
		{
			//QString message = QString("IQ_ParallelTask: [%1] WaitResult::Abort.")
			//	.arg(QString::fromStdString(m_pConfig->name));
			//LoggingWrapper::instance()->info(message);
			break;
		}
		else
		{
			continue;
		}

		//Delay_MSec(50);

		PrintSummaryTable();
	}

	while ((m_WorkingTable.size() + m_WorkingTable.size()) > 0)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	m_Running = false;

	QString message = QString("IQ_ParallelTask: [%1] calculate task finished.")
		.arg(QString::fromStdString(m_pConfig->name));
	LoggingWrapper::instance()->debug(message);

	//std::cout << m_pConfig->name << "task finished" << std::endl;
}

std::vector<std::string> IQ_Parallel_NS::IQ_ParallelTask::ReplaceSpecialString(std::vector<std::string> src, const std::string& oldstr, const std::string& newstr)
{
	std::regex old(oldstr);
	std::vector<std::string>::iterator iter;

	for (iter = src.begin(); iter != src.end(); iter++)
	{
		QString expression = QString::fromStdString((*iter));
		expression = expression.replace(QString::fromStdString(oldstr), QString::fromStdString(newstr));
		(*iter) = expression.toStdString();
	}

	std::vector<std::string> ret;
	ret = src;

	return ret;
}

QStringList IQ_Parallel_NS::IQ_ParallelTask::convertVectorToQStringList(const std::vector<std::string>& vec)
{
	QStringList qStringList;
	for (const auto& str : vec) {
		qStringList.append(QString::fromStdString(str).toLower());
	}
	return qStringList;
}

int IQ_Parallel_NS::IQ_ParallelTask::SearchSubstringCount(const std::vector<std::string>& vec, const std::string& sub)
{
	int count = 0;

	for (const auto& str : vec)
	{
		if (QString::fromStdString(str).contains(QString::fromStdString(sub), Qt::CaseInsensitive))
			count++;
	}

	return count;
}

int IQ_Parallel_NS::IQ_ParallelTask::getValidSummary4VirtualCamera()
{
	std::vector<QStringList>::const_iterator iter;

	for (iter = m_SummaryTable.begin(); iter != m_SummaryTable.end();)
	{
		QStringList list = (*iter);
		int cnt = 0;
		for (auto name : list) {
			if (CameraSimulator::getInstance()->isImageExist(name))
				cnt++;
		}

		if (list.size() != cnt) {
			iter = m_SummaryTable.erase(iter);
		}
		else
			iter++;
	}
	return 0;
}

void IQ_Parallel_NS::IQ_ParallelTask::MetricCalculationMethod(QStringList m_NowCalculationQueue)
{
	IQTModel::MetricCalculator metricFunc = IQTModel::metricCalculators.value(QString::fromStdString(m_pConfig->name));
	if (metricFunc == nullptr)
	{
		LoggingWrapper::instance()->error("can not find metric function.");
		return;
	}

	std::vector<cv::Mat> params;
	QString Color("");
	QString CurrentId("");

	for (int i = 0; i < m_NowCalculationQueue.size(); i++)
	{
		ImageAlgoMetaData _imgData;
		int ret = ImageDataManager::GetInstance()->ReadImageByName(m_NowCalculationQueue[i], _imgData);
		if (0 == ret)
		{
			params.push_back(_imgData.image); //_imgData.image.clone()
		}

		if (i == 0)
		{
			//use first color-MetricName-eyeboxId info to call metricFunc;
			Color = QString::fromStdString(_imgData.lightSource);
			CurrentId = QString::fromStdString(_imgData.eyeboxId);
		}
	}

	if (params.size() == m_NowCalculationQueue.size())
	{
		LoggingWrapper::instance()->info(QString("calculating [%1] metric begin. image paras {%2}.")
			.arg(QString::fromStdString(m_pConfig->name))
			.arg(m_NowCalculationQueue.join(",")));

		try
		{
			QString result = (IQTModel::instance()->*metricFunc)(Color, QString::fromStdString(m_pConfig->name), params, CurrentId);
			if (m_pConfig->releate.size() > 0)
			{
				for each (std::string var in m_pConfig->releate)
				{
					IQTModel::MetricCalculator metricFunc2 = IQTModel::metricCalculators.value(QString::fromStdString(var));

					if (metricFunc2 == nullptr)
					{
						LoggingWrapper::instance()->error("can not find metric function");
					}
					(IQTModel::instance()->*metricFunc2)("", QString::fromStdString(var), params, CurrentId);
				}
			}
		}
		catch (const std::exception& e)
		{
			LoggingWrapper::instance()->error(QString("calculating [%1] metric error. %2.")
				.arg(QString::fromStdString(m_pConfig->name))
				.arg(QString::fromStdString(e.what())));
		}

		//LoggingWrapper::instance()->info(QString("calculating [%1] metric finished.").arg(QString::fromStdString(m_pConfig->name)));
	}
}
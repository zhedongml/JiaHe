#pragma once
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <QObject>
#include "IQ_BasicDataType.h"
#include "iqmetricconfig.h"

namespace IQ_Parallel_NS
{
	class IQ_ParallelTask :public QObject
	{
		Q_OBJECT
	public:
		IQ_ParallelTask(MetricDescription*, QStringList, bool);
		~IQ_ParallelTask();

		void Init();
		void Start();
		void Stop();
		void Restart(bool);

		std::string GetName();
		std::vector<QStringList> GetSummaryTable();
		int GetImageRefCount(QString);
		IQ_TaskState getRunningStatus();

	signals:
		void Signal_WaitEventStop();
		void Signal_MandatoryStop();//终止信号 线程停止

	protected slots:
		void Slot_RecvSpecificImage(QString Name);

	protected:
		bool m_UserAbort = false;
		MetricDescription* m_pConfig = nullptr;
		std::vector<QStringList> m_SummaryTable;
		std::atomic<bool> m_Running;
		std::atomic<bool> m_Pause;   ///<暂停标识
		std::mutex _mutex;
		mutable std::shared_mutex rw_mtx;
		std::condition_variable m_Condition;
		std::vector<QStringList> m_WorkingTable = {};
		QStringList m_eyeboxlist = {};
		std::thread m_Thread;
		bool m_VirtualCameraMode = false;

		void Run();

		std::vector<std::string> ReplaceSpecialString(std::vector<std::string>,
			const std::string&, const std::string&);
		void MetricCalculationMethod(QStringList m_NowCalculationQueue);
		void Delay_MSec(unsigned int msec);
		void PrintSummaryTable();
		IQ_WaitResult WaitRecvRequiredImages(int sec);
		QStringList UpdateCalcQueue();
		bool CheckCalcCondition(QStringList&);
		void RemoveFromSummary(QStringList);
		QStringList convertVectorToQStringList(const std::vector<std::string>&);
		int SearchSubstringCount(const std::vector<std::string>& vec, const std::string& sub);
		int getValidSummary4VirtualCamera();
		void pause();
		void resume();




	};
}

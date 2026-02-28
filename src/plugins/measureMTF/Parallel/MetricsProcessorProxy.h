#pragma once
#include <QCoreApplication>
#include <QDebug>
#include <QHash>
#include <QObject>
#include <QStringList>
#include <vector>
#include <map>
#include "IQ_BasicDataType.h"
#include "IQ_ParallelTask.h"
#include "Result.h"


struct MetricDescription;

class IQ_ParallelTask;

namespace IQ_Parallel_NS
{
	class MetricsProcessorProxy:public QObject
	{
		Q_OBJECT
	public:
		static MetricsProcessorProxy* GetInstance(void);
		~MetricsProcessorProxy();
		Result StartParallelCalculate(QStringList, QStringList);
		Result StopParallelCalculate();
		Result RestartParallelCalculate();
		void ClearCache();
		int GetImageRefCount(QString imageName);
		IQ_TaskState getRunningStatus();
		void setVirtualCameraMode(bool);
		bool getVirtualCameraMode();

	signals:
		void Signal_Abort_DUT();
		void Signal_ParallelCalc_Finished();

	private:
		explicit MetricsProcessorProxy(QObject* parent = nullptr);
		QHash<QString, IQ_ParallelTask*> _qHashThreadTask;
		std::map<std::string, MetricDescription*> pMetrics;
		bool m_VirtualCameraMode = false;
	};
}


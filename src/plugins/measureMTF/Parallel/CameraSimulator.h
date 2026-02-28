#pragma once
#include <Windows.h>
#include <QCoreApplication>
#include <QDebug>
#include <QHash>
#include <QImage>
#include <QObject>
#include <opencv2/opencv.hpp>
#include "IQ_BasicDataType.h"
#include "MLColorimeterMode.h"
#include "MLColorimeterHelp.h"
#include "MLColorimeterCommon.h"
#include "MLTaskManager.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include <QTimer>

using namespace ML_Task;

namespace IQ_Parallel_NS
{
	struct SimulationMetaData {
		std::string eyeboxId = "";
		std::string lightSource = "";  //"R/G/B/W"
		std::string imageType = "";    //"Solid/Checkerboard/XHI/Flare/Ghost/Grid"
		std::string imageName = "";
	};

	class CameraSimulator : public QObject
	{
		Q_OBJECT
	public:
		static CameraSimulator* getInstance(void);
		~CameraSimulator();
		void initialize(QString dir, int interval);
		bool isDirExist(QString fullPath);
		bool isImageExist(QString fileName);
		bool startWork(BT::TreeNode& node);

	signals:
		void startSignal();
		void stopSignal();

	private slots:
		void Slot_ParallelCalc_Finished();

	private:
		CameraSimulator(QObject* parent = nullptr);
		void Init();
		void loadImagesToCache(const QString& path, QStringList& imageList, QStringList&);
		QString getRealFileName(QString path);
		bool batchUpdateImages();
		void Delay_MSec(unsigned int msec);

		QStringList m_fileList;
		QStringList m_nameList;
		QString m_imgPoolDir;
		int m_timerInterval = 5;
		QHash<QString, SimulationMetaData> m_MapInfo;
		bool m_RunStop = false;
	};
}

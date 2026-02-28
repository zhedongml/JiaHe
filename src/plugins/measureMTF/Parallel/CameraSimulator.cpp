#include "CameraSimulator.h"
#include "LoggingWrapper.h"
#include <Windows.h>
#include "MetricsProcessorProxy.h"
#include "ImageDataManager.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include "iqmetricconfig.h"
#include "Core/loggingwrapper.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QThread>
#include <QDirIterator>
#include "IQ_BasicDataType.h"
#include "MLColorimeterMode.h"
#include "MLColorimeterHelp.h"
#include "MLColorimeterCommon.h"
#include "MLTaskManager.h"

class QSharedMemory;

using namespace ML_Task;

namespace fs = std::filesystem;
using namespace IQ_Parallel_NS;

CameraSimulator* CameraSimulator::getInstance()
{
	static CameraSimulator self;
	return &self;
}

CameraSimulator::CameraSimulator(QObject* parent)
{
	// 心跳定时器
	//m_heartbeatTimer = new QTimer(this);
	//connect(m_heartbeatTimer, &QTimer::timeout,
	//	this, &CameraSimulator::onImageCaptured);

	//m_thread = nullptr;
	//m_worker = nullptr;
}

CameraSimulator::~CameraSimulator()
{
}

void CameraSimulator::Slot_ParallelCalc_Finished()
{
	m_RunStop = true;
}

void CameraSimulator::initialize(QString dir, int interval)
{
	m_imgPoolDir = dir;
	m_timerInterval = interval;
	connect(MetricsProcessorProxy::GetInstance(), &MetricsProcessorProxy::Signal_ParallelCalc_Finished,
		this, &CameraSimulator::Slot_ParallelCalc_Finished);

	MetricsProcessorProxy::GetInstance()->setVirtualCameraMode(true);
	MetricsProcessorProxy::GetInstance()->ClearCache();
	loadImagesToCache(m_imgPoolDir, m_fileList, m_nameList);

}

bool CameraSimulator::isDirExist(QString fullPath)
{
	if (fullPath.isEmpty())
	{
		return true;
	}
	QDir dir(fullPath);
	if (dir.exists())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CameraSimulator::isImageExist(QString fileName)
{
	return m_nameList.contains(fileName, Qt::CaseSensitivity::CaseInsensitive);
}

bool CameraSimulator::startWork(BT::TreeNode& node)
{
	m_RunStop = false;

	if (m_timerInterval <= 0)
	{
		MetricsProcessorProxy::GetInstance()->RestartParallelCalculate();
		batchUpdateImages();
		return true;
	}
	else
	{
		MetricsProcessorProxy::GetInstance()->RestartParallelCalculate();
		while (!m_RunStop && (m_fileList.size() > 0))
		{
			int randomInt = QRandomGenerator::global()->bounded(0, m_fileList.size());
			qDebug() << __FUNCTION__ << randomInt << m_fileList.size();

			QString imgPath = m_fileList.at(randomInt);

			QString imgName = getRealFileName(imgPath);

			qDebug() << "**********" << imgName;

			cv::Mat img = cv::imread(imgPath.toStdString(), cv::IMREAD_UNCHANGED);
			if (!img.empty()) {

				if (m_MapInfo.keys().contains(imgName))
				{
					SimulationMetaData info = m_MapInfo.value(imgName);
					auto metadata = std::make_shared<ML_Task::ImageAlgoMetaData>();
					metadata->eyeboxId = info.eyeboxId;
					metadata->lightSource = info.lightSource;
					metadata->image = img;
					metadata->imageName = imgName.toStdString();
					emit MLColorimeterMode::Instance()->GetCaptureTaskManager()->dispatch_image_to_compute(metadata);
				}
			}

			if (node.IsStopChildNodes() || (IQ_TaskState::Idle == MetricsProcessorProxy::GetInstance()->getRunningStatus()))
				break;

			m_fileList.removeAt(randomInt);
			std::this_thread::sleep_for(std::chrono::seconds(m_timerInterval));
		}

		MetricsProcessorProxy::GetInstance()->setVirtualCameraMode(false);
		MetricsProcessorProxy::GetInstance()->ClearCache();
		return true;
	}

}

bool CameraSimulator::batchUpdateImages()
{
	ImageDataManager::GetInstance()->Clear();

	for (int i = 0; i < m_fileList.size(); i++)
	{
		QString imgPath = m_fileList.at(i);

		QString imgName = getRealFileName(imgPath);

		qDebug() << "**********" << imgName;

		cv::Mat img = cv::imread(imgPath.toStdString(), cv::IMREAD_UNCHANGED);
		if (!img.empty()) {

			if (m_MapInfo.keys().contains(imgName))
			{
				SimulationMetaData info = m_MapInfo.value(imgName);
				auto metadata = std::make_shared<ML_Task::ImageAlgoMetaData>();
				metadata->eyeboxId = info.eyeboxId;
				metadata->lightSource = info.lightSource;
				metadata->image = img;
				metadata->imageName = imgName.toStdString();

				ImageDataManager::GetInstance()->SaveImageByName(imgName, *metadata, false);
			}
		}
	}

	return true;
}


QString CameraSimulator::getRealFileName(QString path)
{
	QFileInfo fileinfo = QFileInfo(path);
	QString suffix = fileinfo.suffix();
	QString fileName = fileinfo.fileName();
	return fileName.left(fileName.length() - suffix.length() - 1);
}

void CameraSimulator::loadImagesToCache(const QString& path, QStringList& fileList, QStringList& nameList)
{
	fileList.clear();
	nameList.clear();
	//QDirIterator it(path,
	//	{ "*.tif" }, // 支持的图片格式
	//	QDir::Files | QDir::NoDotAndDotDot,
	//	true ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);

	//while (it.hasNext()) {
	//	fileList.append(it.next());
	//	nameList.append(getRealFileName(it.next()));
	//}

	QDirIterator dir_iterator(path, { "*.tif" },
		QDir::Files | QDir::NoSymLinks,
		QDirIterator::NoIteratorFlags);

	while (dir_iterator.hasNext())
	{
		dir_iterator.next();
		QFileInfo file_info = dir_iterator.fileInfo();
		//QString absolute_file_path =file_info.absoluteFilePath();
		//m_filelist << file_info.absoluteFilePath();

		QString imgName = getRealFileName(file_info.absoluteFilePath().toLower());

		int refCount = MetricsProcessorProxy::GetInstance()->GetImageRefCount(imgName);

		if (refCount > 0)
		{
			fileList.append(file_info.absoluteFilePath().toLower());
			nameList.append(imgName);
		}
	}

	Init();

	int size = fileList.size();
}


void CameraSimulator::Init()
{
	m_MapInfo.clear();

	std::vector<MetricDescription*>* desVec = IQMetricConfig::instance()->getMetricDescripts();

	for (int i = 0; i < desVec->size(); i++)
	{
		MetricDescription* item = desVec->at(i);
		std::string name = item->name;
		const std::vector<std::string>& images = item->images;
		const std::vector<std::string>& colors = item->colors;
		std::vector<std::string>& eyeboxs = item->eyeboxId;

		for each (std::string eyebox in eyeboxs)
		{
			for each (std::string color in colors)
			{
				for each (std::string header in images)
				{
					QString qImgHeader = QString::fromStdString(header);// .split("_");

					if (qImgHeader.contains("$eyeboxId$", Qt::CaseSensitivity::CaseInsensitive))
					{
						qImgHeader = qImgHeader.replace("$eyeboxId$", QString::fromStdString(eyebox));
					}

					if (qImgHeader.contains("$color$", Qt::CaseSensitivity::CaseInsensitive))
					{
						qImgHeader = qImgHeader.replace("$color$", QString::fromStdString(color));
					}

					qImgHeader = qImgHeader.toLower();

					if (isImageExist(qImgHeader))
					{
						if (false == m_MapInfo.keys().contains(qImgHeader))
						{
							SimulationMetaData info;
							info.eyeboxId = eyebox;
							info.lightSource = color;  //"R/G/B/W"
							info.imageName = qImgHeader.toLower().toStdString();
							m_MapInfo.insert(qImgHeader.toLower(), info);
						}
					}
				}
			}
		}

	}
}

void CameraSimulator::Delay_MSec(unsigned int msec)
{
	QEventLoop loop;
	QTimer::singleShot(msec, &loop, SLOT(quit()));
	loop.exec();
}

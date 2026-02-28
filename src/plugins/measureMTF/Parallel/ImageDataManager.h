#pragma once
#include <Windows.h>
#include <QCoreApplication>
#include <QDebug>
#include <QHash>
#include <QImage>
#include <QObject>
#include <shared_mutex>
#include <opencv2/opencv.hpp>
#include "IQ_BasicDataType.h"
#include "MLColorimeterMode.h"
#include "MLColorimeterHelp.h"
#include "MLColorimeterCommon.h"
#include "MLTaskManager.h"

class QSharedMemory;

using namespace ML_Task;

namespace IQ_Parallel_NS
{
	class ImageDataManager : public QObject
	{
		Q_OBJECT
	public:
		static ImageDataManager* GetInstance(void);
		~ImageDataManager();
		int SaveImageByName(QString, ImageAlgoMetaData, bool notify = true);
		int IsImageExist(QString);
		int ReadImageByName(QString, ImageAlgoMetaData&);
		void Clear();
		int FreeImageByName(QString, QString);
		void FreeImagesByNameList(QString,QStringList);
		QStringList GetExistingImagesList();
		QStringList FilterExistingImages(QStringList);

	signals:
		void Signal_RecvSpecificImage(QString);

	public slots:
		void Slot_NewImgMetaData(std::shared_ptr<ML_Task::ImageAlgoMetaData>);
		//void Slot_NewImgMetaData(ML_Task::ImageAlgoMetaData imgData);
		//void Slot_test(QString imgData);

	private:
		ImageDataManager(QObject* parent = nullptr);
		int GetPixelByteSize(int);
		void Init();
		void ThreadSafeEmit(const QString& _SN);

		QHash<QString, ImageAlgoMetaData> _qHashRAM;
		//std::mutex mtx;
		mutable std::shared_mutex rw_mutex;
	};
}
#include "ImageDataManager.h"
#include <QSharedMemory>
#include "LoggingWrapper.h"
#include "MetricsProcessorProxy.h"

using namespace IQ_Parallel_NS;

std::unique_ptr<ImageDataManager> ImageDataManager::instance_ = nullptr;
std::mutex ImageDataManager::mutex_;

//ImageDataManager* ImageDataManager::GetInstance()
//{
//    static ImageDataManager self;
//    return &self;
//}

void ImageDataManager::Initialize(SharedData& initConfig) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!instance_) {
        instance_ = std::unique_ptr<ImageDataManager>(new ImageDataManager(initConfig));
    }
}

ImageDataManager& ImageDataManager::GetInstance() {
    return *instance_;
}

ImageDataManager::ImageDataManager(SharedData& data, QObject* parent)
    : shared(data)
{
    Init();
}

void ImageDataManager::Init()
{
    //qRegisterMetaType<ML_Task::ImageAlgoMetaData>("ML_Task::ImageAlgoMetaData");
    qRegisterMetaType<std::shared_ptr<ML_Task::ImageAlgoMetaData>>(
        "std::shared_ptr<ML_Task::ImageAlgoMetaData>");
    //bool a = connect(MLColorimeterMode::Instance()->GetCaptureTaskManager(), SIGNAL(dispatch_image_to_compute(ML_Task::ImageAlgoMetaData)),
    //    this, SIGNAL(Slot_NewImaMetaData(ML_Task::ImageAlgoMetaData)));
    bool b = connect(MLColorimeterMode::Instance()->GetCaptureTaskManager(), &ML_Task::MLTaskManager::dispatch_image_to_compute,
        this, &ImageDataManager::Slot_NewImgMetaData,Qt::DirectConnection);
    //bool a = connect(MLColorimeterMode::Instance()->GetCaptureTaskManager(), &ML_Task::MLTaskManager::sig_test,
    //    this, &ImageDataManager::Slot_test, Qt::DirectConnection);
}

void ImageDataManager::Slot_NewImgMetaData(std::shared_ptr<ML_Task::ImageAlgoMetaData> imgData)
{
    SaveImageByName(QString::fromStdString(imgData->imageName).toLower(), *imgData);
}

//void ImageDataManager::Slot_test(QString imgData)
//{
//    qInfo() << imgData;
//}

int ImageDataManager::SaveImageByName(QString _SN, ImageAlgoMetaData imgData, bool notify)
{
    std::unique_lock<std::shared_mutex> lock(rw_mutex);

    if (_qHashRAM.keys().contains(_SN))
    {
        QString message = QString("ImageDataManager: the new image [%1] already exists, now images pool number is [%2].")
            .arg(_SN).arg(_qHashRAM.size());
        //LoggingWrapper::instance()->error(message);

        return -1;
    }

    _qHashRAM.insert(_SN, imgData);

    QString message = QString("ImageDataManager: get new image [%1] , now images pool number is [%2].")
        .arg(_SN).arg(_qHashRAM.size());
    LoggingWrapper::instance()->info(message);

    MetricsProcessorProxy::GetInstance()->NotifyAll(_SN, _qHashRAM.keys());

    return 0;
	{
		std::unique_lock<std::mutex> lock(shared.mtx);
		shared.ready = true;
		shared.imageName = _SN;
		std::cout << "[Producer] Data ready, notifying...\n";

		shared.cv.notify_all();
		//std::this_thread::sleep_for(std::chrono::milliseconds(200));

		// 等待所有消费者完成上一轮
		//shared.consumer_done_cv.wait(lock, [this] {
		//	return shared.consumers_done;
		//	});

		//shared.reset();
	}
}

//int IQ_Parallel_NS::ImageDataManager::IsImageExist(QString _SN)
//{
//    //std::unique_lock<std::mutex> lock(mtx);
//    //std::shared_lock<std::shared_mutex> lock(rw_mutex);
//
//    if (_qHashRAM.keys().contains(_SN))
//    {
//        return 0;
//    }
//    return -1;
//}

int ImageDataManager::ReadImageByName(QString _SN, ImageAlgoMetaData& imgData)
{
    //std::unique_lock<std::mutex> lock(mtx);
    std::shared_lock<std::shared_mutex> lock(rw_mutex);

    if (_qHashRAM.keys().contains(_SN))
    {
        imgData = _qHashRAM.value(_SN);
        return 0;
    }

    QString message = QString("ImageDataManager: the image [%1] does not exist.").arg(_SN);
    //LoggingWrapper::instance()->error(message);

    return -1;
}

ImageDataManager::~ImageDataManager()
{
}

int ImageDataManager::FreeImageByName(QString _Name, QString _taskName)
{
    //std::unique_lock<std::mutex> lock(mtx);

    QStringList hh = _qHashRAM.keys();

    if (false == _qHashRAM.keys().contains(_Name))
    {
        QString message = QString("ImageDataManager: the image [%1] does not exist or has been removed. Operator:[%2]").arg(_Name).arg(_taskName);
        //LoggingWrapper::instance()->debug(message);

        return -1;
    }

    //ImageAlgoMetaData imgData = _qHashRAM.value(_Name);

    //imgData.image.release();

    if (_qHashRAM.remove(_Name))
    {
        QString message = QString("ImageDataManager: the image [%1] has been successfully deleted from image pool. Operator:[%3], now images pool number is [%2]. ")
            .arg(_Name).arg(_qHashRAM.size()).arg(_taskName);
        LoggingWrapper::instance()->info(message);

        return 0;
    }
    else
    {
        QString message = QString("ImageDataManager: the image [%1] does not exist. Operator:[%2]")
            .arg(_Name).arg(_taskName);
        LoggingWrapper::instance()->error(message);

        return -1;
    }
}

void ImageDataManager::Clear()
{
    try
    {
        std::unique_lock<std::shared_mutex> lock(rw_mutex);

        QString message = QString("ImageDataManager: the pool left images:");

        QHashIterator<QString, ImageAlgoMetaData> i(_qHashRAM);

        while (i.hasNext()) {
            i.next();
            qDebug() << i.key();
            message += QString("[%1] ").arg(i.key());
        }

        _qHashRAM.clear();

        LoggingWrapper::instance()->info(message);
    }
    catch (const std::exception& e)
    {
        LoggingWrapper::instance()->error(QString("ImageDataManager::Clear error. %1.")
            .arg(QString::fromStdString(e.what())));
    }
}

void ImageDataManager::FreeImagesByNameList(QString taskName, QStringList deleList)
{
    std::unique_lock<std::shared_mutex> lock(rw_mutex);

    QString message = QString("FreeImagesByMetricName: name [%1] deleList [%2].")
        .arg(taskName)
        .arg("{" + deleList.join(",") + "}");
    LoggingWrapper::instance()->debug(message);

    QStringList::iterator it;

    for (it = deleList.begin(); it != deleList.end(); ++it)
    {
        int refCount = MetricsProcessorProxy::GetInstance()->GetImageRefCount(*it);

        //QString message = QString("FreeImagesByMetricName: [%1] GetImageRefCount [%2].")
        //    .arg(*it).arg(refCount);
        //LoggingWrapper::instance()->debug(message);

        if (0 == refCount)
        {
            FreeImageByName(*it, taskName);
        }
    }
}

QStringList ImageDataManager::GetExistingImagesList()
{
    return _qHashRAM.keys();
}

QStringList ImageDataManager::FilterExistingImages(QStringList subs)
{
    QStringList result;

    QStringList::iterator it;

    for (it = subs.begin(); it != subs.end(); ++it)
    {
        qDebug() << *it;

        if (false == _qHashRAM.keys().contains(*it))
            result.append(*it);
    }

    return result;
}
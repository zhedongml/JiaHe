#include "ImageDataManager.h"
#include <QSharedMemory>
#include "LoggingWrapper.h"
#include "MetricsProcessorProxy.h"

using namespace IQ_Parallel_NS;

ImageDataManager* ImageDataManager::GetInstance()
{
    static ImageDataManager self;
    return &self;
}

ImageDataManager::ImageDataManager(QObject* parent)
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
    //std::unique_lock<std::mutex> lock(mtx);
    std::shared_lock<std::shared_mutex> lock(rw_mutex);

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

    if(notify)
		emit Signal_RecvSpecificImage(_SN);
}

int IQ_Parallel_NS::ImageDataManager::IsImageExist(QString _SN)
{
    //std::unique_lock<std::mutex> lock(mtx);
    //std::shared_lock<std::shared_mutex> lock(rw_mutex);

    if (_qHashRAM.keys().contains(_SN))
    {
        return 0;
    }
    return -1;
}

int ImageDataManager::ReadImageByName(QString _SN, ImageAlgoMetaData& imgData)
{
    //std::unique_lock<std::mutex> lock(mtx);
    //std::shared_lock<std::shared_mutex> lock(rw_mutex);
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

int ImageDataManager::GetPixelByteSize(int depth)
{
    int ByteSize = 1;

    switch (depth)
    {
    case CV_8U:
        ByteSize = 1;
        break;

    case CV_8S:
        ByteSize = 1;
        break;

    case CV_16U:
        ByteSize = 2;
        break;

    case CV_16S:
        ByteSize = 2;
        break;

    case CV_32S:
        ByteSize = 4;
        break;

    case CV_32F:
        ByteSize = 4;
        break;

    case CV_64F:
        ByteSize = 8;
        break;

    case CV_16F:
        ByteSize = 2;
        break;

    default:
        break;
    }

    return ByteSize;
}

int ImageDataManager::FreeImageByName(QString _Name, QString _taskName)
{
    //std::unique_lock<std::mutex> lock(mtx);

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
        //LoggingWrapper::instance()->info(message);

        return 0;
    }
    else
    {
        QString message = QString("ImageDataManager: the image [%1] does not exist. Operator:[%2]")
            .arg(_Name).arg(_taskName);
        //LoggingWrapper::instance()->error(message);

        return -1;
    }
}

void ImageDataManager::Clear()
{
    try
    {
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
    std::shared_lock<std::shared_mutex> lock(rw_mutex);

    //std::unique_lock<std::mutex> lock(mtx);

    //QString message = QString("FreeImagesByMetricName: name [%1] deleList [%2].")
    //    .arg(QString::fromStdString(name))
    //    .arg("{" + deleList.join(",") + "}");
    //LoggingWrapper::instance()->debug(message);

    QStringList::iterator it;

    for (it = deleList.begin(); it != deleList.end(); ++it)
    {
        int refCount = MetricsProcessorProxy::GetInstance()->GetImageRefCount(*it);

        //QString message = QString("FreeImagesByMetricName: [%1] GetImageRefCount [%2].")
        //    .arg(QString::fromStdString(name)).arg(refCount);
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

void ImageDataManager::ThreadSafeEmit(const QString& _SN)
{
    QMetaObject::invokeMethod(this, [this, _SN] {
        emit Signal_RecvSpecificImage(_SN);
        });
}
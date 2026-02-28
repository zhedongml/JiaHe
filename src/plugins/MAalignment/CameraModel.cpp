#include "CameraModel.h"
#include "MLImageTransformHelper.h"
#include "pluginsystem/Services.h"

CameraModel *CameraModel::self = nullptr;
CameraModel *CameraModel::GetInstance(QObject *parent)
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new CameraModel(parent);
        }
    }
    return self;
}

CameraModel::CameraModel(QObject *parent) : QObject(parent), m_GrayLevel(0)
{
    QHash<QString, QVariant> props;
    props.insert("mv", "vimba2");
    // m_pCamera = ExtensionSystem::Internal::ServicesManger::createServiceObj<MLCamera>("MLCamera", props);
    m_pCamera = ExtensionSystem::Internal::ServicesManger::getService<MLCamera>("CORE::MLCamera", props);
}

CameraModel::~CameraModel()
{
    if (!m_pCamera.isNull())
    {
        disConnect();
        delete m_pCamera;
    }
}
void CameraModel::NotifyAutoFocus(MLStatus status, double progress, int *steparray, double *distance, double *data,
                                  int len)
{
}
void CameraModel::NotifyCameraStateChanged(MLCameraState old_state, MLCameraState new_state)
{
    // emit signal to ui
    emit updateCameraStatusSignal(new_state);
}

void CameraModel::NotifyCameraFrameReceived(MLFrame *frame_ptr, void *buffer_ptr)
{
    QImage img(frame_ptr->width, frame_ptr->height, QImage::Format_RGB32);
    bool result = ImageHelper::MLIamgeTransform(img, buffer_ptr, frame_ptr);
    if (result)
    {
        Enqueue(img);
        emit updateCameraStreamSignal();
    }
}

void CameraModel::NotifyCameraGrayLevel(int gray_level)
{
    m_GrayLevel = gray_level * 100 / 255;
    emit updateCameraGrayLevelSignal(m_GrayLevel);
}

Result CameraModel::connect(const char *cameraSn)
{
    if (m_pCamera == nullptr)
    {
        return Result(false, "Camera MV connection failed, camera MV init error.");
    }

    if (!m_pCamera->IsOpened())
    {
        MLResult res = m_pCamera->OpenBySN(cameraSn);
        if (res.code != 1)
        {
            return Result(false, "Camera MV connection failed.");
        }
    }
    m_bConnected = true;
    return Result();
}

bool CameraModel::disConnect()
{
    if (m_bConnected)
    {
        m_pCamera->Close();
        m_bConnected = false;
    }
    return true;
}

bool CameraModel::isConnected()
{
    return m_bConnected && m_pCamera->IsOpened();
}

void CameraModel::SetMLExposureAuto()
{
    if (m_bConnected)
    {
        m_pCamera->SetMLExposureAuto();
    }
}

void CameraModel::SetExposureTime(double time)
{
    if (m_bConnected)
    {
        m_pCamera->SetExposureTime(time);
    }
}

double CameraModel::GetExposureTime()
{
    double t = 0;
    if (m_bConnected)
    {
        t = m_pCamera->GetExposureTime();
    }
    return t;
}

cv::Mat CameraModel::GetImage()
{
    cv::Mat img;
    if (m_bConnected)
    {
        img = m_pCamera->GetImage();
    }
    return img;
}
int CameraModel::GetBinning()
{
    int binning;
    if (m_bConnected)
    {
        binning = m_pCamera->getBinning();
    }
    return binning;
}

void CameraModel::SubscribeCameraEvent(MLCameraEvent event)
{
    m_pCamera->Subscribe(event, this);
}

void CameraModel::UnsubscribeCameraEvent(MLCameraEvent event)
{
    m_pCamera->Unsubscribe(event, this);
}

void CameraModel::Enqueue(const QImage image)
{
    QMutexLocker local_lock(&m_Lock);

    if (m_Data.size() == m_MaxSize)
    {
        m_Data.pop_front();
    }
    m_Data.push_back(image);
}

QImage CameraModel::GetQImage()
{
    QMutexLocker local_lock(&m_Lock);
    QImage res;
    if (!m_Data.isEmpty())
    {
        try
        {
            res = m_Data.front();
            m_Data.pop_front();
        }
        catch (...)
        {
        }
    }
    return res;
}

void CameraModel::ClearImageQueue()
{
    QMutexLocker local_lock(&m_Lock);
    m_Data.clear();
}

int CameraModel::GetGrayLevel()
{
    return m_GrayLevel;
}

void CameraModel::SetInitExposureTime(double initTime)
{
    if (m_bConnected)
    {
        m_pCamera->initExposureTime = initTime;
    }
}


AutoExposureStatus CameraModel::getAEStatus()
{
    AutoExposureStatus AEStatus = AutoExposureStatus::AE_NONE;
    if (m_bConnected)
    {
        AEStatus = m_pCamera->AEStatus_;
    }
    return AEStatus;
}

std::string CameraModel::getAEStatusStr()
{
    std::string AEStatusStr;
    if (m_bConnected)
    {
        AEStatusStr = m_pCamera->m_AEendStatus;
    }
    else
    {
        AEStatusStr = "Camera not connected";
    }
    return AEStatusStr;
}
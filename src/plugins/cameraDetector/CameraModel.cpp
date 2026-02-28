#include "CameraModel.h"
#include "MLImageTransformHelper.h"
#include "ml_vimba_camera.h"
#include "pluginsystem/Services.h"

CameraModel::CameraModel(QObject *parent)
	: QObject(parent),m_GrayLevel(0)
{
    QHash<QString, QVariant> props;
    props.insert("mv", "vimba");
    m_pCamera = ExtensionSystem::Internal::ServicesManger::getService<MLCamera>("CORE::MLCamera", props);
}

CameraModel::~CameraModel()
{
    /*if (m_pCamera != nullptr) {
        delete m_pCamera;
    }*/
}

void CameraModel::NotifyCameraStateChanged(MLCameraState old_state, MLCameraState new_state)
{
	//emit signal to ui
	emit updateCameraStatusSignal(new_state);
}

void CameraModel::NotifyCameraFrameReceived(MLFrame* frame_ptr, void* buffer_ptr)
{
    QImage img(frame_ptr->width, frame_ptr->height, QImage::Format_RGB32);
    bool result = ImageHelper::MLIamgeTransform(img, buffer_ptr, frame_ptr);
    if (result)
    {
        Enqueue(img);
        m_Image = img.copy();
        emit updateCameraStreamSignal();
    }
}

void CameraModel::NotifyCameraGrayLevel(int gray_level)
{
    m_GrayLevel = gray_level * 100 / 255;
	emit updateCameraGrayLevelSignal(m_GrayLevel);
}

bool CameraModel::connect(const char* cameraSn)
{
    if (!m_bConnected) {
        MLResult res = m_pCamera->OpenBySN(cameraSn);
        if (res.code != 1)
        {
            return false;
        }
        m_bConnected = true;
    }
    return true;
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

void CameraModel::SetMLExposureAuto()
{
    m_pCamera->SetMLExposureAuto();
}

void CameraModel::SetExposureTime(double time)
{
    m_pCamera->SetExposureTime(time);
}

double CameraModel::GetExposureTime()
{
    double t = 0;
    if (m_bConnected) {
        t = m_pCamera->GetExposureTime();
    }
    return t;
}

cv::Mat CameraModel::GetImage()
{
    

    cv::Mat img;
    if (m_bConnected) {
        img = m_pCamera->GetImage();
    }
    return img;
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

    return m_Image;

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

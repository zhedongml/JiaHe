#include "CollimatedLightTubeMode.h"
#include "CollimatedConfig.h"
#include <QHash>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "pluginsystem/Services.h"

#define IMAGEMAXNUM 3

CollimatedLightTubeMode* CollimatedLightTubeMode::self = nullptr;

CollimatedLightTubeMode::CollimatedLightTubeMode(QObject* parent)
    : QObject(parent)
{
    QHash<QString, QVariant> props;
    props.insert("mv", "mindversion");
    m_pCamera = ExtensionSystem::Internal::ServicesManger::getService<CORE::MLCamera>("CORE::MLCamera", props);
}

CollimatedLightTubeMode::~CollimatedLightTubeMode()
{
    if (m_collimatorAngleCal)
    {
        delete m_collimatorAngleCal;
        m_collimatorAngleCal = nullptr;
    }
}

CollimatedLightTubeMode* CollimatedLightTubeMode::GetInstance(QObject* parent)
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new CollimatedLightTubeMode(parent);
        }
    }
    return self;
}

void CollimatedLightTubeMode::NotifyCameraFrameReceived(CORE::MLFrame* frame_ptr, void* buffer_ptr)
{
    if (frame_ptr->format == CORE::MLPixelFormat::kMLMono8)
    {
        cv::Mat mat = cv::Mat(frame_ptr->height, frame_ptr->width, CV_8UC1, buffer_ptr).clone();

        QImage image = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        image = image.copy();
        ImageReceived(image);
        emit showImageSignal();
    }
}

void CollimatedLightTubeMode::ImageReceived(const QImage image)
{
    m_mutex.lock();

    if (m_Images.size() == IMAGEMAXNUM)
    {
        m_Images.pop();
    }
    m_Images.push(image);

    m_mutex.unlock();
}

QImage CollimatedLightTubeMode::GetQImage()
{
    m_mutex.lock();
    QImage res;
    if (!m_Images.empty())
    {
        res = m_Images.front();
        m_Images.pop();
    }
    m_mutex.unlock();

    return res;
}

Result CollimatedLightTubeMode::Connect(const char* cameraSn)
{
    if (!m_bConnected) {
        MLResult res = m_pCamera->OpenBySN(cameraSn);
        if (res.code != 1)
        {
            return Result(false, res.msg);
        }

        MLCollimatorConfig config = CollimatedConfig::instance()->GetLevelingConfig();
        m_collimatorAngleCal = new CollimatorAngleCal(config.BaseCenter.x, config.BaseCenter.y,
            config.pixelSize, config.focalLength, m_pCamera);
        m_collimatorAngleCal->SubscribeAngleCallBack([](double x, double y) {
            auto mode = CollimatedLightTubeMode::GetInstance();
            mode->OnAngleUpdated(x, y);
            });

        m_collimatorAngleCal->AngleCalculate();
        m_bConnected = true;
    }
    return Result();
}

bool CollimatedLightTubeMode::DisConnect()
{
    if (m_bConnected)
    {
        m_pCamera->Close();
        m_bConnected = false;
    }
    return true;
}

bool CollimatedLightTubeMode::IsConnected()
{
    return m_bConnected;
}

void CollimatedLightTubeMode::SetMLExposureAuto()
{
    if (m_bConnected)
    {
        m_pCamera->SetMLExposureAuto();
    }
}

void CollimatedLightTubeMode::SetExposureTime(double time)
{
    if (m_bConnected)
    {
        m_pCamera->SetExposureTime(time);
    }
}

double CollimatedLightTubeMode::GetExposureTime()
{
    double t = 0;
    if (m_bConnected) {
        t = m_pCamera->GetExposureTime();
    }
    return t;
}

cv::Mat CollimatedLightTubeMode::GetImage()
{
    cv::Mat img;
    if (m_bConnected) {
        img = m_pCamera->GetImage();
    }
    return img;
}

void CollimatedLightTubeMode::SubscribeCameraEvent(CORE::MLCameraEvent event)
{
    m_pCamera->Subscribe(event, this);
}

void CollimatedLightTubeMode::UnsubscribeCameraEvent(CORE::MLCameraEvent event)
{
    m_pCamera->Unsubscribe(event, this);
}

CORE::MLCamera* CollimatedLightTubeMode::GetCamera()
{
    return m_pCamera;
}

void CollimatedLightTubeMode::OnAngleUpdated(double angleX, double angleY)
{
    QMutexLocker locker(&m_mutex);
    if (angleX == -99999 && angleY == -99999)
    {
        m_collimatorDeltaX = "NULL";
        m_collimatorDeltaY = "NULL";
    }
    else
    {
        m_collimatorDeltaX = QString::number(angleX);
        m_collimatorDeltaY = QString::number(angleY);
    }
    emit updateCollimatorAngle(m_collimatorDeltaX, m_collimatorDeltaY);
}

QString CollimatedLightTubeMode::GetCollimatorAngleX()
{
    QMutexLocker locker(&m_mutex);
    return m_collimatorDeltaX;
}

QString CollimatedLightTubeMode::GetCollimatorAngleY()
{
    QMutexLocker locker(&m_mutex);
    return m_collimatorDeltaY;
}

Result CollimatedLightTubeMode::WriteAngleToCSV(const QString& filePath, const QString& angleX, const QString& angleY)
{
    QFile csvFile(filePath);
    bool fileExists = csvFile.exists();
    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        return Result(false, "failed to open " + filePath.toStdString() + " file.");
    }
    QTextStream out(&csvFile);
    if (!fileExists)
    {
        out << "timestamp,angleX,angleY\n";
    }
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    out << timestamp << "," << angleX << "," << angleY << "\n";
    csvFile.close();
    return Result();
}
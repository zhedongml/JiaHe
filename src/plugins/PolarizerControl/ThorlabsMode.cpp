#include "ThorlabsMode.h"
#include "ELLOLib.h"
#include "piMotionActor/configItem.h"
#include <QMutex>

ThorlabsMode* ThorlabsMode::self = nullptr;
ThorlabsMode* ThorlabsMode::instance(QObject* parent)
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new ThorlabsMode(parent);
        }
    }
    return self;
}

ThorlabsMode::ThorlabsMode(QObject* parent) {

    m_motionModel = Motion1DModel::getInstance("Thorlabs");
    m_motionModel->Motion1DSubscribeMotionEvent(MLMotionEvent::kStateChanged);
    //QObject::connect(m_motionModel, SIGNAL(updateMotion1DPositionSignal(float)), this, SIGNAL(updateMotion1DPositionSignal(float)));
    //QObject::connect(m_motionModel, SIGNAL(updateMotion1DStatusSignal(MLMotionState)), this, SIGNAL(updateMotion1DStatusSignal(MLMotionState)));
}

ThorlabsMode::~ThorlabsMode()
{
    Disconnect();

    delete m_motionModel;
    m_motionModel = nullptr;
}

Result ThorlabsMode::Connect()
{
    QString ip = ConfigItem::instance()->getMotion1DIpAndOther();
    if (!m_motionModel->connect(ip))
    {
        return Result(false, "Thorlabs connect failed.");
    }
    return Result();
}

bool ThorlabsMode::Disconnect()
{
    m_motionModel->disconnect();
    return IsConnected();
}

bool ThorlabsMode::IsConnected()
{
    return m_motionModel->Motion1DisConnected();
}

Result ThorlabsMode::AbsMoveAsync(double coordinate)
{
    if (!IsConnected())
    {
        return Result(false, "Polarizer not connect!");
    }
    Result ret = rangeJudge(coordinate);
    if (!ret.success)
    {
        return Result(false, ret.errorMsg);
    }
    return m_motionModel->Motion1DMoveAbsAsync(coordinate * 1000);
}

Result ThorlabsMode::RelativeMoveAsync(double offset)
{
    if (!IsConnected())
    {
        return Result(false, "Polarizer not connect!");
    }
    Result ret = rangeJudge(GetPosition() + offset);
    if (!ret.success)
    {
        return Result(false, ret.errorMsg);
    }
    return m_motionModel->Motion1DMoveRelAsync(offset * 1000);
}

Result ThorlabsMode::AbsMoveSync(double coordinate)
{
    if (!IsConnected())
    {
        return Result(false, "Polarizer not connect!");
    }
    Result ret = rangeJudge(coordinate);
    if (!ret.success)
    {
        return Result(false, ret.errorMsg);
    }
    return m_motionModel->Motion1DMoveAbs(coordinate * 1000);
}

Result ThorlabsMode::RelativeMoveSync(double offset)
{
    if (!IsConnected())
    {
        return Result(false, "Polarizer not connect!");
    }
    Result ret = rangeJudge(GetPosition() + offset);
    if (!ret.success)
    {
        return Result(false, ret.errorMsg);
    }
    return m_motionModel->Motion1DMoveRel(offset * 1000);
}

Result ThorlabsMode::StopMove()
{
    if (!IsConnected())
    {
        return Result(false, "Polarizer not connect!");
    }
    return m_motionModel->stopMove();
}

double ThorlabsMode::GetPosition()
{
    if (!IsConnected())
    {
        return DBL_MAX;
    }
    return m_motionModel->getPosition();
}

Result ThorlabsMode::rangeJudge(double coordinate)
{
    cv::Point2f limits = ConfigItem::instance()->getMotion1DLimit();
    if (coordinate< limits.x || coordinate>limits.y)
    {
        return Result(false, QString("Polarizer move position %1 exceeds the range of movement %2-%3")
            .arg(coordinate).arg(limits.x).arg(limits.y).toStdString());
    }
    return Result();
}
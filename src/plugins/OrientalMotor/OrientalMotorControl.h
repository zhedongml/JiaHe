#pragma once
#include "Result.h"
#include "MLSerialManage.h"
#include "OrientalMotorConfig.h"
#include "OrientalMotor.h"
#include "orientalmotor_global.h"
#include <QObject>

class ORIENTALMOTOR_EXPORT OrientalMotorControl : public QObject
{
    Q_OBJECT

public:

    static OrientalMotorControl* getInstance();

    OrientalMotorControl(QObject* parent = nullptr);
    ~OrientalMotorControl();

    Result Connect();
    Result Disconnect();
    bool IsConnected();
    Result MoveByDegreeAsync(int type, double degree);
    Result MoveByDegreeSync(int type, double degree);
    Result StopMove(int type);
    Result StopMove();
    Result HomingSync(int type);
    Result HomingAsync(int type);
    Result HomingAsync();
    Result SetSpeed(int pulse);
    bool IsHome(int type);
    bool IsMoving(int type);
    bool IsMoving();
    double GetPosition(int type);
    //MLMotionState GetState();
    //void Subscribe(MLMotionEvent event, CoreMotionCallback* callback);
    //void Unsubscribe(MLMotionEvent event, CoreMotionCallback* callback);
    Result ClearAlarm();
    bool IsAlarm(int type);
    bool IsAlarm();

signals:
    void sigOrientalMotorConnectStatus(bool res);

private:
    void Init();
    Result JudegOpen(int type);

private:
    //std::mutex _status_mutex;
    bool m_bIsOpen = false;

    static OrientalMotorControl* m_instance;
    std::map<int, OrientalMotor*>m_MotorMap;
};

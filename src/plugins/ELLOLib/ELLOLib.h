#pragma once
#include <iostream>
#include "PrjCommon/service/ml_motion.h"

#include "ellolib_global.h"

class ELLOMOTORImpl;
class ELLOLIB_EXPORT ELLOMOTOR : public CoreMotion
{
public:
    ELLOMOTOR();
    ~ELLOMOTOR();

    virtual Result Connect(const char* type_name);

    virtual bool Disconnect();

    virtual bool IsConnected();

    virtual void SetEnable(bool enable);

    virtual bool IsEnabled();

    virtual Result MoveRelAsync(int step);

    virtual Result MoveAbsAsync(int pos);

    virtual Result MoveRel(int step);

    virtual Result MoveAbs(int pos);

    virtual Result StopMove();

    virtual Result Homing();

    virtual bool IsHome();

    virtual void SetSpeed(int speed);

    virtual int GetSpeed();

    virtual bool IsMoving();

    virtual int GetPosition();

    virtual MLMotionState GetState();

    virtual void Subscribe(MLMotionEvent event, CoreMotionCallback* callback);

    virtual void Unsubscribe(MLMotionEvent event, CoreMotionCallback* callback);

    virtual bool ClearAlarm();

private:
    std::unique_ptr<ELLOMOTORImpl> m_implELLOMOTOR;
};

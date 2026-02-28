#ifndef CORE_MLMOTION2D_H_
#define CORE_MLMOTION2D_H_

#pragma once
#include"ml.h"
#include "../prjcommon_global.h"

class PRJCOMMON_EXPORT MLMotion2DCallback
{
  public:
    virtual void NotifyMotion2DStateChanged(CORE::MLMotionState old_state, CORE::MLMotionState new_state) = 0;
    virtual void NotifyMotion2DPosition() = 0;
};
class PRJCOMMON_EXPORT MLMotion2D
{

public:

    virtual Result Connect(const char *type_name) = 0;

    virtual bool Disconnect() = 0;

    virtual bool IsConnected() = 0;

    virtual void SetEnable(bool enable) = 0;

    virtual bool IsEnabled() = 0;

    virtual Result MoveRelAsync(double posX, double posY) = 0;

    virtual Result MoveAbsAsync(double pos, double posY) = 0;

    virtual Result MoveRel(double posX, double posY) = 0;

    virtual Result MoveAbs(double posX, double posY) = 0;

    virtual Result StopMove() = 0;

    virtual Result Homing() = 0;

    virtual Result IsHome(bool& flag,int type) = 0;

    virtual void SetSpeed(int speed) = 0;

    virtual void SetSpeedX(double speed);

    virtual void SetSpeedY(double speed);

    virtual int GetSpeed() = 0;

    virtual bool IsMoving() = 0;

    virtual CORE::ML_Point GetPosition() = 0;

    virtual CORE::MLMotionState GetState() = 0;

    virtual void Subscribe(CORE::MLMotionEvent event, MLMotion2DCallback *callback) = 0;

    virtual void Unsubscribe(CORE::MLMotionEvent event, MLMotion2DCallback *callback) = 0;

    virtual void clearAlarm() = 0;

    virtual Result homingForced() = 0;
};
#endif // !COMMON_MLMOTION_H_
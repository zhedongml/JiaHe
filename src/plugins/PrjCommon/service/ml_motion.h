#ifndef CORE_MLMOTION_H_
#define CORE_MLMOTION_H_
#include <QObject>
#include"ml.h"
#include "Result.h"
#include "../prjcommon_global.h"

using namespace CORE;

class PRJCOMMON_EXPORT CoreMotionCallback
{
public:
    virtual void NotifyMotionStateChanged(CORE::MLMotionState old_state, CORE::MLMotionState new_state) = 0;
    virtual void NotifyMotionPosition(int position) = 0;
};

class PRJCOMMON_EXPORT CoreMotion : public QObject
{
    Q_OBJECT
public:
    CoreMotion();
    virtual Result Connect(const char* type_name) = 0;

    virtual bool Disconnect() = 0;

    virtual bool IsConnected() = 0;

    virtual void SetEnable(bool enable) = 0;

    virtual bool IsEnabled() = 0;

    virtual Result MoveRelAsync(int step) = 0;

    virtual Result MoveAbsAsync(int pos) = 0;

    virtual Result MoveRel(int step) = 0;

    virtual Result MoveAbs(int pos) = 0;

    virtual Result StopMove() = 0;

    virtual Result Homing() = 0;

    virtual bool IsHome() = 0;

    virtual void SetSpeed(int speed) = 0;

    virtual int GetSpeed() = 0;

    virtual bool IsMoving() = 0;

    virtual int GetPosition() = 0;

    virtual CORE::MLMotionState GetState() = 0;

    virtual void Subscribe(CORE::MLMotionEvent event, CoreMotionCallback *callback) = 0;

    virtual void Unsubscribe(CORE::MLMotionEvent event, CoreMotionCallback *callback) = 0;

    int direction_;
    virtual ~CoreMotion();
};
#endif // !CORE_MLMOTION_H_
#ifndef CORE_MLMOTION3D_H_
#define CORE_MLMOTION3D_H_
#include"ml.h"
#include"ml_motion.h"
#include <opencv2/opencv.hpp>
#include "../prjcommon_global.h"

class PRJCOMMON_EXPORT MLMotion3DCallback
{
public:
    virtual void NotifyMotion3DStateChanged(CORE::MLMotionState old_state, CORE::MLMotionState new_state) = 0;
    virtual void NotifyMotion3DPosition() = 0;
};

class PRJCOMMON_EXPORT MLMotion3D
{

public:

    virtual Result Connect(const char* type_name) = 0;

    virtual bool Disconnect() = 0;

    virtual bool IsConnected() = 0;

    virtual void SetEnable(bool enable) = 0;

    virtual bool IsEnabled() = 0;

    virtual Result MoveRelAsync(int posX, int posY, int posZ) = 0;

    virtual Result MoveAbsAsync(int pos, int posY, int posZ) = 0;

    virtual Result MoveRelAsync(const char* axis, int pos) = 0;

    virtual Result MoveAbsAsync(const char* axis, int pos) = 0;

    virtual Result MoveRel(const char *axis, int pos) = 0;
    virtual Result MoveAbs(const char *axis, int pos) = 0;

    virtual Result MoveRel(int posX, int posY, int posZ) = 0;

    virtual Result MoveAbs(int posX, int posY, int posZ) = 0;

    virtual Result StopMove() = 0;

    virtual Result Homing() = 0;

    virtual bool IsHome() = 0;

    virtual void SetSpeed(int speed) = 0;
    virtual void SetSpeedX(double speed);
    virtual void SetSpeedY(double speed);
    virtual void SetSpeedZ(double speed);
    virtual int GetSpeed() = 0;

    virtual bool IsMoving() = 0;

    virtual CORE::ML_Point3D GetPosition() = 0;

    virtual CORE::MLMotionState GetState() = 0;

    virtual void Subscribe(CORE::MLMotionEvent event, MLMotion3DCallback *callback) = 0;

    virtual void Unsubscribe(CORE::MLMotionEvent event, MLMotion3DCallback *callback) = 0;

    virtual void clearAlarm() = 0;

    virtual Result setAcsSLimit(cv::Point3f max, cv::Point3f min)
    {
        return Result();
    }
};
#endif // !CORE_MLMOTION3D_H_
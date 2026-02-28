// Copyright 2023 MLOptic

#ifndef LOGIC_MOTION_ORIENTALMOTORCONTROL_H_
#define LOGIC_MOTION_ORIENTALMOTORCONTROL_H_

#include <mutex>
#include <string>

#include "ml.h"

#include "MLSerialManage.h"
#include "OrientalMotorConfig.h"
using namespace CORE;

enum class MLOrientalStatus {
    MLOrientalStatus_OK,
    MLOrientalStatus_Connecting,
    MLOrientalStatus_Close,
    MLOrientalStatus_Moving,
    MLOrientalStatus_Stationary,
    MLOrientalStatus_Alarm,
    MLOrientalStatus_Error,
    MLOrientalStatus_SerialException,
    MLOrientalStatus_Timeout,
    MLOrientalStatus_Unknown,
    MLOrientalStatus_NotEnable
};

struct MLOrientalResult {
    bool code = false;
    MLOrientalStatus status = MLOrientalStatus::MLOrientalStatus_Unknown;
    std::string msg = "";
};

class MLOrientalMotorCallback {
public:
    virtual void NotifyOrientalStatusChanged(
        const std::string object, MLOrientalStatus status) = 0;
};

class OrientalMotor {

public:    
    OrientalMotor(const MLOrientalConfiguation& config);
      ~OrientalMotor();
 public:
     MLOrientalMotorCallback* _callback = nullptr;
     MLOrientalStatus m_status = MLOrientalStatus::MLOrientalStatus_Unknown;
    bool m_bIsOpen = false;
    std::string _msg;
    CORE::MLIOCommand m_command = CORE::MLIOCommand();
    std::mutex _status_mutex;

    void Subscribe();

    void UnSubscribe();

    MLOrientalResult ML_OpenSerial(
        MLOrientalMotorCallback* cb = nullptr);

    MLOrientalResult ML_OpenSerialSync();

    MLOrientalResult ML_CloseSerial();

    bool ML_IsOpen();

    bool ML_IsMoving();

    bool ML_IsAlarm();

    MLOrientalResult ML_StopMove();

    MLOrientalResult ML_Reset(MLOrientalMotorCallback* cb = nullptr);

    MLOrientalResult ML_ResetSync();

    MLOrientalResult ML_MoveByDegree(double degree, MLOrientalMotorCallback* cb = nullptr);

    MLOrientalResult ML_MoveByDegreeSync1(double degree);

    MLOrientalResult ML_MoveByDegreeSync(double degree);

    MLOrientalResult ML_MoveByPulse(int pulse, MLOrientalMotorCallback* cb = nullptr);

    MLOrientalResult ML_MoveByPulseSync(int pulse);

    double ML_GetCurrDegree1();

    int ML_GetCurrPulse();

    double ML_GetCurrDegree();

    MLOrientalStatus ML_GetStatus();

    MLOrientalResult ML_WaitForFinish(int timeout = 30000);

    MLOrientalResult ML_WaitResetFinish(int timeout = 30000);

    MLOrientalResult ML_ClearAlarm();

    int ML_ReadAlarmCode();

    MLOrientalResult ML_SetSpeed(int pulse);

private:
    MLOrientalConfiguation m_config;
};

#endif  // LOGIC_MOTION_ORIENTALMOTORCONTROL_H_

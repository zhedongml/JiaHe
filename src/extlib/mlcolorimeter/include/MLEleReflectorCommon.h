// Copyright 2025 MLOptic

#ifndef MLHARDWARE_MLELEREFLECTORCOMMON_H_
#define MLHARDWARE_MLELEREFLECTORCOMMON_H_

#include <map>

#include "MLSerialCommon.h"
namespace ML {
namespace MLEleReflectorCommon {

enum class MLEleEnum { Reflector_IN, Reflector_OUT};

enum class MLEleStatus {
    MLEleStatus_OK,
    MLEleStatus_Close,
    MLEleStatus_Moving,
    MLEleStatus_Stationary,
    MLEleStatus_Alarm,
    MLEleStatus_Error,
    MLEleStatus_SerialException,
    MLEleStatus_TimeOut,
    MLEleStatus_NotEnable,
    MLEleStatus_Unknown
};

struct MLEleConfiguration {
    bool Enable = false;
    std::string Type = "ElectricReflector";
    std::string Name = "EleReflector";
    std::string MotorType = "SongXia";
    std::string Port;
    int Station = 1;
    ML::MLSerialCommon::MLSerialInfo SerialInfo = ML::MLSerialCommon::MLSerialInfo();
    std::map<std::string, int> PosNameList;
    std::map<MLEleEnum, int> PosEnumList;
    bool IOCommandEnable = false;
    ML::MLSerialCommon::IOCommand IOCommand;
};

struct MLEleResult {
    bool Code = true;
    std::string Msg = "";
    MLEleStatus Status = MLEleStatus::MLEleStatus_OK;
};

}  // namespace MLEleReflectorCommon
}  // namespace ML
#endif  // !MLHARDWARE_MLELEREFLECTORCOMMON_H_

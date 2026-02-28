// Copyright 2023 MLOptic

#ifndef LOGIC_MOTION_MLFILTERWHEEL_MLFILTERWHEELCOMMON_H_
#define LOGIC_MOTION_MLFILTERWHEEL_MLFILTERWHEELCOMMON_H_

#include <map>
#include <string>

/// <summary>
///  This file contains the common struct and enum classes for MLFilterWheel and
///  MLRXFilterWheel.
/// </summary>

namespace ML {
namespace MLFilterWheel {
struct MLIOCommand {
    int start;
    int zHome;
    int stop;
    int alarmRst;
    MLIOCommand() {
        start = 3;
        zHome = 4;
        stop = 5;
        alarmRst = 7;
    }

    MLIOCommand(int startT, int zHomeT, int stopT, int alarmRstT) {
        start = startT;
        zHome = zHomeT;
        stop = stopT;
        alarmRst = alarmRstT;
    }
};

struct MLAxisInfo {
    bool enable = true;
    std::string name = "Axis";
    std::string motorType = "Oriental/AZ";
    int station = 4;
    bool MLIOCommand_enable = false;
    MLIOCommand mlIOCommand;
    int PulsePerCycle = 28333;
    int Axis_Index = 0;
    int Min = 0;
    int Max = 360;
};

struct MLSerialInfo {
    int baudrate = 115200;
    int bytesize = 8;
    std::string parity = "even";
    std::string stopbits = "onestop";
    std::string flowcontrol = "none";
};

enum class MLFilterEnum {
    X = 1,
    Y = 2,
    Z,
    ND0,
    ND1,
    ND2,
    ND3,
    ND4,
    ND5,
    Clear,
    Block,
    YA,
    Blue,
    ND24,
    Empty,
    Customer1,
    Customer2,
    Customer3,
    Customer4,
    Customer5,
    Customer6,
    Customer7,
    Customer8,
    Customer9,
    Customer10,
    Unknown,
    Alarm,
    Failed,
    Error,
    Null
};

enum class MLFilterStatus {
    MLFilterStatus_OK,
    MLFilterStatus_Connecting,
    MLFilterStatus_Close,
    MLFilterStatus_Moving,
    MLFilterStatus_Stationary,
    MLFilterStatus_Alarm,
    MLFilterStatus_Error,
    MLFilterStatus_SerialException,
    MLFilterStatus_Timeout,
    MLFilterStatus_Unknown,
    MLFilterStatus_NotEnable
};

struct MLNDFilterConfiguation {
    bool enable = false;
    std::string type = "FilterWheel";
    std::string name = "ND";
    std::string protocol = "modbus/serial";
    std::string motorType = "Oriental/AZ";
    std::string port;
    int station = 1;
    MLSerialInfo serial_info;
    std::map<std::string, int> positionName_List;
    std::map<MLFilterEnum, int> positionEnum_List;
    bool MLIOCommand_enable = false;
    MLIOCommand MLIOCommand;
    MLFilterEnum home_enum;
};

struct MLRXFilterConfiguation {
    bool enable = false;
    std::string type = "RXFilterWheel";
    std::string name = "RX Filter";
    std::string protocol = "modbus/serial";
    std::string motorType = "Oriental/AZ";
    std::string port;
    int station = 3;
    MLSerialInfo serial_info;
    std::map<std::string, int> positionName_List;
    bool MLIOCommand_enable = false;
    MLIOCommand MLIOCommand;
    MLAxisInfo axis_info;
};

struct MLFilterResult {
    bool code = false;
    MLFilterStatus status = MLFilterStatus::MLFilterStatus_Unknown;
    std::string msg = "";
};

class MLFilterWheelCallback {
 public:
    virtual void NotifyFilterStatusChanged(
        const std::string object, ML::MLFilterWheel::MLFilterStatus status) = 0;
};

}  // namespace MLFilterWheel
}  // namespace ML

#endif  // LOGIC_MOTION_MLFILTERWHEEL_MLFILTERWHEELCOMMON_H_

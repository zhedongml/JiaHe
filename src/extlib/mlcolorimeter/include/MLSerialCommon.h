// Copyright 2025 MLOptic

#ifndef MLSERIALCOMMON_H_
#define MLSERIALCOMMON_H_

#include <string>

namespace ML {
namespace MLSerialCommon {
struct IOCommand {
    int start;
    int zHome;
    int stop;
    int alarmRst;
    IOCommand() {
        start = 3;
        zHome = 4;
        stop = 5;
        alarmRst = 7;
    }

    IOCommand(int startT, int zHomeT, int stopT, int alarmRstT) {
        start = startT;
        zHome = zHomeT;
        stop = stopT;
        alarmRst = alarmRstT;
    }
};

struct MLSerialInfo {
    int BaudRate = 115200;
    int ByteSize = 8;
    std::string Parity = "even";
    std::string StopBits = "onestop";
    std::string FlowControl = "none";
};
}
}  // namespace ML
#endif  // !MLSERIALCOMMON_H_

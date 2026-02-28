// Copyright 2023 MLOptic
#include "MLSerial.h"

#include <algorithm>
#include <utility>

#include "MLSpdlog.h"

MLSerial::MLSerial() {
    m_Serial = new serial::Serial();
    m_MLIOCommand = CORE::MLIOCommand();
    m_totalIdx = 8;
    ML::MLSpdlog::instance()->info("MLSerial: create instance");
}

MLSerial::~MLSerial() {
    if (m_Serial != nullptr) {
        delete m_Serial;
        m_Serial = nullptr;
    }
}

void MLSerial::InitSerialPort(CORE::MLSerialInfo serial_info) {
    serial::Timeout timeout = serial::Timeout(100, 300, 300, 300);
    m_Serial->setTimeout(timeout);

    serial::bytesize_t byte = serial::bytesize_t(serial_info.bytesize);
    serial::parity_t parity = transformToParity(serial_info.parity);
    serial::stopbits_t stopbit = transformToStopBit(serial_info.stopbits);
    serial::flowcontrol_t flowcontrol =
        transformToFlowcontrol(serial_info.flowcontrol);

    m_Serial->setBaudrate(serial_info.baudrate);
    m_Serial->setBytesize(byte);
    m_Serial->setParity(parity);
    m_Serial->setStopbits(stopbit);
    m_Serial->setFlowcontrol(flowcontrol);
    ML::MLSpdlog::instance()->info("MLSerial: initialize serial port");
}

void MLSerial::SetMLIOCommand(CORE::MLIOCommand MLIOCommand) {
    m_MLIOCommand = MLIOCommand;
}

bool MLSerial::Open(std::string port) {
    try {
        if (m_Serial == nullptr) {
            return false;
        }
        m_Serial->setPort(port);
        if (m_Serial->isOpen()) {
            ML::MLSpdlog::instance()->info("MLSerial: port " + port +
                                           " is already opened");
            return true;
        }
        m_Serial->open();
        if (m_Serial->isOpen()) {
            ML::MLSpdlog::instance()->info("MLSerial: open serial");
            return true;
        }
        ML::MLSpdlog::instance()->error("MLSerial: open serial failed");
        return false;
    } catch (const std::exception) {
        ML::MLSpdlog::instance()->error("MLSerial: open serial exception");
        return false;
    }
}

bool MLSerial::Close() {
    if (m_Serial == nullptr) {
        return false;
    }
    if (m_Serial->isOpen()) {
        m_Serial->close();
        if (m_Serial->isOpen()) {
            return false;
        }
        ML::MLSpdlog::instance()->info("MLSerial: close serial");
        return true;
    }
    return false;
}

bool MLSerial::IsOpen() {
    if (m_Serial == nullptr)
        return false;
    return m_Serial->isOpen();
}

bool MLSerial::IsMoving(int SlaveId) {
    bool isMoving = false;
    int val = QueryStatus(SlaveId);
    if (val == INT_MAX) {
        return false;
    }
    isMoving = val & 0x2000;
    return isMoving;
}

bool MLSerial::IsAlarm(int SlaveId) {
    bool isAlarm = false;
    int val = QueryStatus(SlaveId);
    if (val == INT_MAX) {
        return false;
    }
    int index;
    index = static_cast<int>(pow(2, m_MLIOCommand.alarmRst));
    int a = val & index;
    isAlarm = a;
    return isAlarm;
}

bool MLSerial::CheckSerial(int SlaveId) {
    size_t readSize = 8;
    uint8_t sID = SlaveId;
    uint8_t code = 0x06;
    uint8_t writeData[] = {sID, code, 0x00, 0x7d, 0x00, 0x00};

    bool ret = SendAndCheckCmd(writeData, readSize);
    return ret;
}

bool MLSerial::MoveByIndex(int SlaveId, int index) {
    if (!StartOFF(SlaveId)) {
        return false;
    }
    size_t readSize = 8;
    uint8_t sID = SlaveId;
    uint8_t code = 0x06;
    int idx;
    idx = index + static_cast<int>(pow(2, m_MLIOCommand.start));
    uint8_t high = (uint8_t)((idx & 0xFF00) >> 8);
    uint8_t low = (uint8_t)(idx & 0x00FF);
    uint8_t writeData[] = {sID, code, 0x00, 0x7d, high, low};

    bool ret = SendAndCheckCmd(writeData, readSize);
    if (ret) {
        ML::MLSpdlog::instance()->info(
            "MLSerial: move to No." + std::to_string(index) +
            " for slave station " + std::to_string(SlaveId));
        return true;
    }
    ML::MLSpdlog::instance()->info(
        "MLSerial: move to No." + std::to_string(index) +
        " failed for slave station " + std::to_string(SlaveId));
    return ret;
}


bool MLSerial::MoveByPulse(int SlaveId, int pulse, int index) {
    //不管脉冲是否大于65535现在都走else

    /*if (pulse <= pow(2, 16)) {
        size_t readSize = 8;
        uint8_t sID = SlaveId;
        uint8_t code = 0x06;
        int address;
        if (m_strMotorType.find("AZ") != std::string::npos) {
            address = 6147 + index * 64;
        } else {
            address = 1025 + index * 2;
        }
        uint8_t u1 = (uint8_t)((address & 0xFF00) >> 8);
        uint8_t u2 = (uint8_t)(address & 0x00FF);

        uint8_t high = (uint8_t)((pulse & 0xFF00) >> 8);
        uint8_t low = (uint8_t)(pulse & 0x00FF);
        uint8_t writeData[] = {sID, code, u1, u2, high, low};
        bool ret = SendAndCheckCmd(writeData, readSize);
        if (ret) {
            ret = MoveByIndex(SlaveId, index);
            return ret;
        }
        return false;
    }
    else*/
    {
        //这里是脉冲超过65535的情况
        size_t readSize = 13;
        uint8_t sID = SlaveId;
        uint8_t code = 0x10;
        int address;
        if (m_strMotorType.find("AZ") != std::string::npos) {
            address = 6146 + index * 64;
        }
        else {
            address = 1024 + index * 2;
        }
        uint8_t u1 = (uint8_t)((address & 0xFF00) >> 8); //寄存器地址上位，高字节，如果index=2,算出0x18
        uint8_t u2 = (uint8_t)(address & 0x00FF); //寄存器地址下位，低字节，如果index=2，算出0x82
        //如果pulse = 100000
        uint8_t high1 = (pulse >> 24) & 0xFFFF; //寄存器高位地址写入值，0x00
        uint8_t low1 = (pulse >> 16) & 0xFFFF; //寄存器地位地址写入值，0x01
        uint8_t high2 = (pulse >> 8) & 0xFFFF; //寄存器高位地址+1的写入值，0x86
        uint8_t low2 = (pulse & 0xFFFF); //寄存器低位地址+1的写入值，0xA0
        uint8_t writeData[] = { sID,  code,  u1,   u2,    0x00, 0x02,
                               0x04, high1, low1, high2, low2 };
        bool ret = SendAndCheckCmd(writeData, readSize);
        if (ret) {
            ret = MoveByIndex(SlaveId, index);
            return ret;
        }
        return false;
    }
}

bool MLSerial::Reset(int SlaveId) {
    if (!StartOFF(SlaveId)) {
        return false;
    }
    size_t readSize = 8;
    uint8_t sID = SlaveId;
    uint8_t code = 0x06;
    int index = static_cast<int>(pow(2, m_MLIOCommand.zHome));
    uint8_t high = (uint8_t)((index & 0xFF00) >> 8);
    uint8_t low = (uint8_t)(index & 0x00FF);
    uint8_t writeData[] = {sID, code, 0x00, 0x7d, high, low};

    bool ret = SendAndCheckCmd(writeData, readSize);
    if (ret) {
        ML::MLSpdlog::instance()->info("MLSerial: reset for slave station " +
                                       std::to_string(SlaveId));
        return true;
    }
    ML::MLSpdlog::instance()->info("MLSerial: reset failed for slave station " +
                                   std::to_string(SlaveId));
    return false;
}

int MLSerial::GetCurrIndex(int SlaveId) {
    std::vector<int> FilterPosList;
    int precision = 100;
    int curr_pos = GetCurrPosition(SlaveId);
    if (curr_pos == INT_MAX) {
        return INT_MAX;
    }
    m_totalIdx = static_cast<int>(pow(2, m_MLIOCommand.start));
    if (m_FilterPosList.count(SlaveId) == 0) {
        for (int i = 0; i < m_totalIdx; i++) {
            int filter_pos = GetIndexPosition(SlaveId, i);
            ML::MLSpdlog::instance()->info(
                "MLSerial: get No." + std::to_string(i) +
                " pulse for slave station " + std::to_string(SlaveId) +
                ", pulse = " + std::to_string(filter_pos));
            if (filter_pos == INT_MAX) {
                return INT_MAX;
            }
            FilterPosList.push_back(filter_pos);
        }
        m_FilterPosList.insert(
            std::pair<int, std::vector<int>>(SlaveId, FilterPosList));
    }

    for (int i = 0; i < m_FilterPosList[SlaveId].size(); ++i) {
        if (curr_pos == m_FilterPosList[SlaveId].at(i)) {
            return i;
        }
    }

    for (int i = 0; i < m_FilterPosList[SlaveId].size(); ++i) {
        if (curr_pos >= m_FilterPosList[SlaveId].at(i) - precision &&
            curr_pos < m_FilterPosList[SlaveId].at(i) + precision) {
            return i;
        }
    }
    return INT_MAX;
}

int MLSerial::GetSpeed(int SlaveId) {
    std::vector<uint8_t> readBuffer;
    size_t readSize = 9;
    size_t dataSize = 8;
    uint8_t sID = SlaveId;
    uint8_t code = 0x03;
    uint8_t writeData[] = {sID, code, 0x00, 0xCE, 0x00, 0x02};
    std::vector<uint8_t> cmdData = SendCmd(writeData, dataSize);
    if (cmdData.empty()) {
        return INT_MAX;
    }
    m_Serial->flushOutput();
    readBuffer = CmdResult(cmdData.data(), readSize);
    if (!readBuffer.empty()) {
        uint32_t speed =
            (((uint32_t)readBuffer[3] << 24) | ((uint32_t)readBuffer[4] << 16) |
             ((uint16_t)readBuffer[5] << 8) | readBuffer[6]);
        return speed;
    }
    return INT_MAX;
}

bool MLSerial::SetSpeed(int SlaveId, int pulse)
{
    size_t readSize = 8;
    uint8_t sID = SlaveId;

    uint8_t high = (uint8_t)((pulse & 0xFF00) >> 8);
    uint8_t low = (uint8_t)(pulse & 0x00FF);

    uint8_t writeData[] = { sID, 0x10, 0x04, 0x80, 0x00, 0x02, 0x04, 0x00, 0x00, high, low };

    bool ret = SendAndCheckCmd(writeData, readSize);
    return ret;
}

bool MLSerial::StopMove(int SlaveId) {
    if (!StartOFF(SlaveId)) {
        return false;
    }
    if (!StartOFF(SlaveId)) {
        return false;
    }
    size_t readSize = 8;
    uint8_t sID = SlaveId;
    uint8_t code = 0x06;
    int index;
    index = static_cast<int>(pow(2, m_MLIOCommand.stop));
    uint8_t high = (uint8_t)((index & 0xFF00) >> 8);
    uint8_t low = (uint8_t)(index & 0x00FF);
    uint8_t writeData[] = {sID, code, 0x00, 0x7d, high, low};

    bool ret = SendAndCheckCmd(writeData, readSize);
    if (ret) {
        ML::MLSpdlog::instance()->info(
            "MLSerial: stop move for slave station " + std::to_string(SlaveId));
        return true;
    }
    ML::MLSpdlog::instance()->info(
        "MLSerial: stop move failed for slave station " +
        std::to_string(SlaveId));
    return false;
}

//bool MLSerial::ClearAlarm(int SlaveId) {
//    if (!StartOFF(SlaveId)) {
//        return false;
//    }
//    if (!StartOFF(SlaveId)) {
//        return false;
//    }
//    bool ret;
//    size_t readSize = 8;
//    uint8_t sID = SlaveId;
//    uint8_t code = 0x06;
//    int index = static_cast<int>(pow(2, m_MLIOCommand.alarmRst));
//    uint8_t high = (uint8_t)((index & 0xFF00) >> 8);
//    uint8_t low = (uint8_t)(index & 0x00FF);
//    uint8_t writeData[] = {sID, code, 0x00, 0x7d, high, low};
//
//    ret = SendAndCheckCmd(writeData, readSize);
//    if (ret) {
//        ML::MLSpdlog::instance()->info(
//            "MLSerial: clear alarm for slave station " +
//            std::to_string(SlaveId));
//        return true;
//    }
//    ML::MLSpdlog::instance()->info(
//        "MLSerial: clear alarm failed for slave station " +
//        std::to_string(SlaveId));
//    return false;
//}

bool MLSerial::ClearAlarm(int SlaveId) {
    if (!StartOFF(SlaveId)) {
        return false;
    }
    bool ret;
    size_t readSize = 8;
    uint8_t sID = SlaveId;
    uint8_t code = 0x06;
    int index = static_cast<int>(pow(2, m_MLIOCommand.alarmRst));
    uint8_t high = (uint8_t)((index & 0xFF00) >> 8);
    uint8_t low = (uint8_t)(index & 0x00FF);
    uint8_t writeData[] = { sID, code, 0x00, 0x7d, high, low };

    if (IsAlarm(SlaveId)) {
        m_Serial->flushOutput();
        uint32_t alarm_code;
        alarm_code = ReadAlarmCode(SlaveId);
        m_Serial->flushOutput();
        if (alarm_code != 0x33) {
            ret = SendAndCheckCmd(writeData, readSize);
            if (ret) {
                ML::MLSpdlog::instance()->info(
                    "MLSerial: clear alarm for slave station " +
                    std::to_string(SlaveId));
                return true;
            }
            ML::MLSpdlog::instance()->info(
                "MLSerial: clear alarm failed for slave station " +
                std::to_string(SlaveId));
            return false;
        }
    }

    //ret = SendAndCheckCmd(writeData, readSize);
    //if (ret) {
    //    ML::MLSpdlog::instance()->info(
    //        "MLSerial: clear alarm for slave station " +
    //        std::to_string(SlaveId));
    //    return true;
    //}
    //ML::MLSpdlog::instance()->info(
    //    "MLSerial: clear alarm failed for slave station " +
    //    std::to_string(SlaveId));
    //return false;
    return true;
}

int MLSerial::ReadAlarmCode(int SlaveId) {
    std::vector<uint8_t> readBuffer;
    size_t readSize = 9;
    size_t dataSize = 8;
    readBuffer.clear();
    uint8_t sID = SlaveId;
    uint8_t code = 0x03;
    uint8_t writeData[] = {sID, code, 0x00, 0x80, 0x00, 0x02};

    std::vector<uint8_t> cmdData = SendCmd(writeData, dataSize);
    if (cmdData.empty()) {
        return INT_MAX;
    }
    m_Serial->flushOutput();
    readBuffer = CmdResult(cmdData.data(), readSize);
    if (!readBuffer.empty()) {
        int alarm_code =
            (((uint32_t)readBuffer[3] << 24) | ((uint32_t)readBuffer[4] << 16) |
             ((uint16_t)readBuffer[5] << 8) | readBuffer[6]);
        ML::MLSpdlog::instance()->info(
            "MLSerial: read alarm code for slave station " +
            std::to_string(SlaveId) + ", code = " + std::to_string(alarm_code));
        return alarm_code;
    }
    return INT_MAX;
}

int MLSerial::GetCurrPosition(int SlaveId) {
    std::vector<uint8_t> readBuffer;
    size_t readSize = 9;
    size_t dataSize = 8;
    uint8_t sID = SlaveId;
    uint8_t code = 0x03;
    uint8_t writeData[] = {sID, code, 0x00, 0xCC, 0x00, 0x02};
    std::vector<uint8_t> cmdData = SendCmd(writeData, dataSize);
    if (cmdData.empty()) {
        return INT_MAX;
    }
    readBuffer = CmdResult(cmdData.data(), readSize);
    if (!readBuffer.empty()) {
        uint32_t pos =
            (((uint32_t)readBuffer[3] << 24) | ((uint32_t)readBuffer[4] << 16) |
             ((uint16_t)readBuffer[5] << 8) | readBuffer[6]);
        ML::MLSpdlog::instance()->info(
            "MLSerial: get current pulse for slave station " +
            std::to_string(SlaveId) + ", pulse = " + std::to_string(pos));
        return pos;
    }
    return INT_MAX;
}

int MLSerial::GetIndexPosition(int SlaveId, int index) {
    if (index < 0 || index > 60) {
        return INT_MAX;
    }
    std::vector<uint8_t> readBuffer;
    size_t readSize = 9;
    size_t dataSize = 8;
    uint8_t sID = SlaveId;
    uint8_t code = 0x03;
    int idx = 0;
    if (m_strMotorType.find("AZ") != std::string::npos) {
        idx = 6144 + index * 64 + 2;
    } else {
        idx = 1024 + index * 2;
    }

    uint8_t d1 = (uint8_t)((idx & 0xFF00) >> 8);
    uint8_t d2 = (uint8_t)(idx & 0x00FF);
    uint8_t writeData[] = {sID, code, d1, d2, 0x00, 0x02};

    std::vector<uint8_t> cmdData = SendCmd(writeData, dataSize);
    if (cmdData.empty()) {
        return INT_MAX;
    }
    readBuffer = CmdResult(cmdData.data(), readSize);
    if (!readBuffer.empty()) {
        uint32_t pos =
            (((uint32_t)readBuffer[3] << 24) | ((uint32_t)readBuffer[4] << 16) |
             ((uint16_t)readBuffer[5] << 8) | readBuffer[6]);
        return pos;
    }
    return INT_MAX;
}

bool MLSerial::StartOFF(int SlaveId) {
    std::vector<uint8_t> readBuffer;
    size_t readSize = 8;

    readBuffer.clear();
    uint8_t sID = SlaveId;
    uint8_t code = 0x06;
    uint8_t writeData[] = {sID, code, 0x00, 0x7d, 0x00, 0x00};
    bool ret = SendAndCheckCmd(writeData, readSize);

    return ret;
}

bool MLSerial::StartOn(int SlaveId) {
    std::vector<uint8_t> readBuffer;
    size_t readSize = 8;

    readBuffer.clear();
    uint8_t sID = SlaveId;
    uint8_t code = 0x06;
    uint8_t writeData[] = {sID, code, 0x00, 0x7d, 0x00, 0x08};
    bool ret = SendAndCheckCmd(writeData, readSize);

    return ret;
}

bool MLSerial::WriteRegister(const std::vector<uint8_t>& data) {
    try {
        if (m_Serial == nullptr)
            return false;
        if (!m_Serial->isOpen())
            return false;
        if (data.empty()) {
            return false;
        }
        size_t bytes_wrote = m_Serial->write(data);
        if (bytes_wrote == data.size()) {
            return true;
        }
        return false;
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<uint8_t> MLSerial::ReadRegister(size_t size) {
    try {
        m_Serial->flushOutput();
        std::vector<uint8_t> read_buffer;
        size_t bytes_readed;
        bytes_readed = m_Serial->read(read_buffer, size);
        if (bytes_readed == size) {
            return read_buffer;
        } else {
            return std::vector<uint8_t>();
        }
    } catch (const std::exception&) {
        return std::vector<uint8_t>();
    }
}

void MLSerial::SetMotorType(std::string type) {
    transform(type.begin(), type.end(), type.begin(), ::toupper);
    m_strMotorType = type;
}

int MLSerial::QueryStatus(int SlaveId) {
    std::string msg = "";
    std::vector<uint8_t> readBuffer;
    size_t readSize = 7;
    size_t dataSize = 8;
    uint8_t sID = SlaveId;
    uint8_t code = 0x03;
    uint8_t writeData[] = {sID, code, 0x00, 0x7f, 0x00, 0x01};
    std::vector<uint8_t> cmdData = SendCmd(writeData, dataSize);
    if (cmdData.empty()) {
        return INT_MAX;
    }
    readBuffer = CmdResult(cmdData.data(), readSize);
    if (!readBuffer.empty()) {
        int value = ((readBuffer[3] << 8) | readBuffer[4]);
        return value;
    }
    return INT_MAX;
}

// uint16_t MLSerial::CRC16_Modbus(uint8_t data[]) {
//     uint16_t CRC = 0xFFFF;
//     int len = static_cast<int>(sizeof(data) / sizeof(data[0]));
//     for (int i = 0; i < len - 2; i++) {
//         CRC = CRC ^ data[i];
//         for (int j = 0; j < 8; j++) {
//             if (CRC & 1) {
//                 CRC >>= 1;
//                 CRC ^= 0xA001;
//             } else {
//                 CRC >>= 1;
//             }
//         }
//     }
//     CRC = (((uint16_t)CRC << 8) & 0xFF00) |
//           ((uint16_t)CRC >> 8);  // Replace low high with high low
//     return CRC;
// }

uint16_t MLSerial::CRC16_Modbus(uint8_t data[],
                                                   size_t data_size) {
    uint16_t CRC = 0xFFFF;
    for (int i = 0; i < data_size - 2; i++) {
        CRC = CRC ^ data[i];
        for (int j = 0; j < 8; j++) {
            if (CRC & 1) {
                CRC >>= 1;
                CRC ^= 0xA001;
            } else {
                CRC >>= 1;
            }
        }
    }
    CRC = (((uint16_t)CRC << 8) & 0xFF00) |
          ((uint16_t)CRC >> 8);  // Replace low high with high low
    return CRC;
}

// std::vector<uint8_t> MLSerial::ConvertCmd(uint8_t data[]) {
//     // Merge input cmd data with check bits
//     uint16_t crc = CRC16_Modbus(data);
//
//     uint8_t high = (uint8_t)((crc & 0xFF00) >> 8);
//     uint8_t low = (uint8_t)(crc & 0x00FF);
//     std::vector<uint8_t> cmdData;
//     int len = static_cast<int>(sizeof(data) / sizeof(data[0]));
//     for (int i = 0; i < len - 2; i++) {
//         cmdData.push_back(data[i]);
//     }
//     cmdData.push_back(high);
//     cmdData.push_back(low);
//     return cmdData;
// }

std::vector<uint8_t> MLSerial::ConvertCmd(uint8_t data[]) {
    // Merge input cmd data with check bits
    uint16_t crc = CRC16_Modbus(data);

    uint8_t high = (uint8_t)((crc & 0xFF00) >> 8);
    uint8_t low = (uint8_t)(crc & 0x00FF);
    std::vector<uint8_t> cmdData;
    //注意：这里data是指针，不是数组，sizeof(data)计算的是指针占用空间大小，64位系统指针占8字节，len=8
    int len = static_cast<int>(sizeof(data) / sizeof(data[0]));
    for (int i = 0; i < len - 2; i++) {
        cmdData.push_back(data[i]);
    }
    cmdData.push_back(high);
    cmdData.push_back(low);
    return cmdData;
}

std::vector<uint8_t> MLSerial::ConvertCmd2(uint8_t data[], int dataLen)
{
    uint16_t crc = 0xFFFF; // 初始化 CRC 值
    uint16_t polynomial = 0xA001; // CRC-16-IBM 多项式

    for (size_t i = 0; i < dataLen; i++) {
        crc ^= data[i]; // 数据与 CRC 值异或
        for (int j = 0; j < 8; j++) { // 处理每一位
            if (crc & 0x0001) {
                crc >>= 1; // 右移一位
                crc ^= polynomial; // 异或多项式
            }
            else {
                crc >>= 1; // 右移一位
            }
        }
    }

    crc = (((uint16_t)crc << 8) & 0xFF00) |
        ((uint16_t)crc >> 8);  // Replace low high with high low

    uint8_t high = (uint8_t)((crc & 0xFF00) >> 8);
    uint8_t low = (uint8_t)(crc & 0x00FF);

    std::vector<uint8_t> cmdData;
    for (int i = 0; i < dataLen; i++) {
        cmdData.push_back(data[i]);
    }
    cmdData.push_back(high);
    cmdData.push_back(low);

    return cmdData;
}

uint16_t MLSerial::CRC16_Modbus(uint8_t data[]) {
    uint16_t CRC = 0xFFFF;
    int len = static_cast<int>(sizeof(data) / sizeof(data[0]));
    for (int i = 0; i < len - 2; i++) {
        CRC = CRC ^ data[i];
        for (int j = 0; j < 8; j++) {
            if (CRC & 1) {
                CRC >>= 1;
                CRC ^= 0xA001;
            }
            else {
                CRC >>= 1;
            }
        }
    }
    CRC = (((uint16_t)CRC << 8) & 0xFF00) |
        ((uint16_t)CRC >> 8);  // Replace low high with high low
    return CRC;
}

std::vector<uint8_t> MLSerial::SendCmd(uint8_t data[], int dataLen) {
    // send input cmd to driver without check bit
    try {
        if (m_Serial == nullptr)
            return std::vector<uint8_t>();
        if (!m_Serial->isOpen())
            return std::vector<uint8_t>();
        /* std::vector<uint8_t> data2 = { 0x03, 0x10, 0x04, 0x02, 0x00, 0x02, 0x04, 0x00, 0x07, 0xA1, 0x20, 0x82, 0x87 };
         m_Serial->write(data2);
         std::vector<uint8_t> read_buffer;
         m_Serial->read(read_buffer, 8);*/

        std::vector<uint8_t> cmdData;
        if (dataLen < 13)
            cmdData = ConvertCmd(data);
        else
            cmdData = ConvertCmd2(data, dataLen - 2); // 11 + 2 传入11位数计算两位校验位
        if (cmdData.empty()) {
            return std::vector<uint8_t>();
        }
        std::lock_guard<std::mutex> locker(mutex_);
        Sleep(20);
        size_t bytes_wrote = m_Serial->write(cmdData);
        if (bytes_wrote == cmdData.size()) {
            return cmdData;
        }
        return std::vector<uint8_t>();
    }
    catch (const std::exception&) {
        return std::vector<uint8_t>();
    }
}

std::vector<uint8_t> MLSerial::CmdResult(uint8_t checkData[],
                                         size_t read_size) {
    try {
        if (m_Serial == nullptr)
            return std::vector<uint8_t>();
        if (!m_Serial->isOpen())
            return std::vector<uint8_t>();
        size_t bytes_readed;
        std::lock_guard<std::mutex> locker(mutex_);
        Sleep(50);
        std::vector<uint8_t> read_buffer;
        bytes_readed = m_Serial->read(read_buffer, read_size);
        int len = 8;
        if (*(checkData + 1) == 0x06) {
            if (bytes_readed == read_size) {
                for (int i = 0; i < len - 2; i++) {
                    if (*(checkData + i) != read_buffer[i]) {
                        return std::vector<uint8_t>();
                    }
                }
                return read_buffer;
            }
            return std::vector<uint8_t>();
        } else if (*(checkData + 1) == 0x10) {
            for (int i = 0; i < 6; i++) {
                if (*(checkData + i) != read_buffer[i]) {
                    return std::vector<uint8_t>();
                }
            }
            return read_buffer;
        } else {
            if (bytes_readed == read_size) {
                return read_buffer;
            }
            return std::vector<uint8_t>();
        }
    } catch (const std::exception&) {
        return std::vector<uint8_t>();
    }
}

bool MLSerial::SendAndCheckCmd1(uint8_t data[], size_t read_size,
                               size_t data_size) {
    std::vector<uint8_t> readBuffer;
    size_t readSize = read_size;
    std::vector<uint8_t> writeData;
    std::vector<uint8_t> cmdData = SendCmd(data, data_size);
    if (cmdData.empty()) {
        return false;
    }
    m_Serial->flushOutput();
    readBuffer = CmdResult(cmdData.data(), readSize);
    if (!readBuffer.empty()) {
        return true;
    }
    return false;
}

bool MLSerial::SendAndCheckCmd(uint8_t data[], size_t read_size) {
    std::vector<uint8_t> readBuffer;
    size_t readSize = read_size;
    std::vector<uint8_t> writeData;
    std::vector<uint8_t> cmdData = SendCmd(data, read_size);
    if (cmdData.empty()) {
        return false;
    }
    m_Serial->flushOutput();
    readBuffer = CmdResult(cmdData.data(), read_size);
    if (!readBuffer.empty()) {
        return true;
    }
    return false;
}

serial::parity_t MLSerial::transformToParity(std::string parity) {
    transform(parity.begin(), parity.end(), parity.begin(), ::tolower);
    if (parity.find("none") != std::string::npos) {
        return serial::parity_none;
    } else if (parity.find("odd") != std::string::npos) {
        return serial::parity_odd;
    } else if (parity.find("even") != std::string::npos) {
        return serial::parity_even;
    } else if (parity.find("mark") != std::string::npos) {
        return serial::parity_mark;
    } else if (parity.find("space") != std::string::npos) {
        return serial::parity_space;
    } else {
        return serial::parity_even;
    }
}

serial::stopbits_t MLSerial::transformToStopBit(std::string stopbit) {
    transform(stopbit.begin(), stopbit.end(), stopbit.begin(), ::tolower);
    if (stopbit.find("onestop") != std::string::npos) {
        return serial::stopbits_one;
    } else if (stopbit.find("twostop") != std::string::npos) {
        return serial::stopbits_two;
    } else if (stopbit.find("onepointfive") != std::string::npos) {
        return serial::stopbits_one_point_five;
    }
    return serial::stopbits_one;
}

serial::flowcontrol_t MLSerial::transformToFlowcontrol(
    std::string flowcontrol) {
    transform(flowcontrol.begin(), flowcontrol.end(), flowcontrol.begin(),
              ::tolower);
    if (flowcontrol.find("none") != std::string::npos) {
        return serial::flowcontrol_none;
    } else if (flowcontrol.find("software") != std::string::npos) {
        return serial::flowcontrol_software;
    } else if (flowcontrol.find("hardware") != std::string::npos) {
        return serial::flowcontrol_hardware;
    }
    return serial::flowcontrol_none;
}

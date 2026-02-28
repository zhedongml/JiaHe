// Copyright 2023 MLOptic
#pragma once

#include <Windows.h>
#include <serial.h>
#include "ml.h"

#include <map>
#include <mutex>
#include <string>
#include <vector>

using namespace CORE;

class MLSerial {
 public:
    MLSerial();
    ~MLSerial();

    /// <summary>
    /// Function that initializes serial port settings
    /// </summary>
    /// <param name="serial_info">struct contains the serial port
    /// settings</param>
    /// <returns>void</returns>
    void InitSerialPort(CORE::MLSerialInfo serial_info = CORE::MLSerialInfo());

    /// <summary>
    /// Function that sets the IOCommand for dirver, if the numbers of wheels
    /// greater than 7, you should set the MLIOCommand.
    /// </summary>
    /// <param name="MLIOCommand">struct contains MLIOCommand settings</param>
    /// <returns>void</returns>
    void SetMLIOCommand(CORE::MLIOCommand MLIOCommand);

    /// <summary>
    /// Open serial port.
    /// </summary>
    /// <param name="port">the address of the serial port, for example:
    /// "COM1"</param>
    /// <returns>If the port opens successfully</returns>
    bool Open(std::string port);

    /// <summary>
    /// Close serial port.
    /// </summary>
    /// <returns>If the port closes successfully</returns>
    bool Close();

    /// <summary>
    /// Determine if the port has been successfully opened.
    /// </summary>
    bool IsOpen();

    /// <summary>
    /// Determine if the motor is moving.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    bool IsMoving(int SlaveId);

    /// <summary>
    /// Determine if the motor is alarm.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    bool IsAlarm(int SlaveId);
    
    /// <summary>
    /// Check serial communication.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    bool CheckSerial(int SlaveId);

    /// <summary>
    /// Switch the channel of the motor by index.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    /// <param name="index">the index of channel wanted to change</param>
    bool MoveByIndex(int SlaveId, int index);

    /// <summary>
    /// Switch the position of the motor by pulse.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    /// <param name="pulse">the pulse position to be written into</param>
    /// <param name="index">the index channel to write the pulse
    /// position</param>
    bool MoveByPulse(int SlaveId, int pulse, int index);

    /// <summary>
    /// Make the motor perform a homing action.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    bool Reset(int SlaveId);

    /// <summary>
    /// Get current index channel of the motor.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    int GetCurrIndex(int SlaveId);

    /// <summary>
    /// Obtain current speed of the motor.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    int GetSpeed(int SlaveId);

    /// <summary>
    /// set current speed of the motor.
    /// pulse/s
    /// To be verified
    bool SetSpeed(int SlaveId, int pulse);

    /// <summary>
    /// Stop the motor from moving.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    bool StopMove(int SlaveId);

    /// <summary>
    /// Clear the motor alarm status.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    bool ClearAlarm(int SlaveId);

    /// <summary>
    /// Obtain the alarm code of the motor.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    int ReadAlarmCode(int SlaveId);

    /// <summary>
    /// Obtain the current pulse position of motor.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    int GetCurrPosition(int SlaveId);

    /// <summary>
    /// Obtain index channel pulse positon of the motor.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    /// <param name="index">the index channel to get its pulse position</param>
    int GetIndexPosition(int SlaveId, int index);

    /// <summary>
    /// After you start a movement by function code(0x06), you need execute
    /// StartOFF.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    bool StartOFF(int SlaveId);

    /// <summary>
    /// Before you start a movement by function code(0x06), you need execute
    /// StartOn.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    bool StartOn(int SlaveId);

    /// <summary>
    /// Write a hex data to register.
    /// </summary>
    /// <param name="data">the data to be written into the register</param>
    bool WriteRegister(const std::vector<uint8_t>& data);

    /// <summary>
    /// Read hex data from register.
    /// </summary>
    /// <param name="size">the size to read the buffer</param>
    /// <returns>the buffer read from register</returns>
    std::vector<uint8_t> ReadRegister(size_t size);

    /// <summary>
    /// Set tpye of the motor, "AZ" or "AR".
    /// </summary>
    /// <param name="type">the type to set</param>
    void SetMotorType(const std::string type);

 private:
    /// <summary>
    /// Query status from the register.
    /// </summary>
    /// <param name="SlaveId">salve station ID for current motor</param>
    int QueryStatus(int SlaveId);

    /// <summary>
    /// Perform CRC16 verification (Deprecated).
    /// </summary>
    /// <param name="data">the hex data that needs to be passed to the
    /// driver(without check bit)</param>
    /// <returns>the high and low bit data after CRC16 verification</returns>
    //uint16_t CRC16_Modbus(uint8_t data[]);

    /// <summary>
    /// Perform CRC16 verification.
    /// </summary>
    /// <param name="data">the hex data that needs to be passed to the
    /// driver(without check bit)</param>
    /// <param name="data_size">size of the hex data</param>
    /// <returns>the high and low bit data after CRC16 verification</returns>
    uint16_t CRC16_Modbus(uint8_t data[], size_t data_size);

    /// <summary>
    /// Merge sending data with CRC16 data got from CRC16_Modbus (Deprecated).
    /// </summary>
    /// <param name="data">the hex data that needs to be passed to the
    /// driver(without check bit)</param>
    /// <returns>Converted data</returns>
    //std::vector<uint8_t> ConvertCmd(uint8_t data[]);

    /// <summary>
    /// Merge sending data with CRC16 data got from CRC16_Modbus.
    /// </summary>
    /// <param name="data">the hex data that needs to be passed to the
    /// driver(without check bit)</param>
    /// <param name="data_size">size of the hex data</param>
    /// <returns>Converted data</returns>
    //std::vector<uint8_t> ConvertCmd(uint8_t data[], size_t data_size);

    /// <summary>
/// Merge sending data with CRC16 data got from CRC16_Modbus.
/// </summary>
/// <param name="data">the hex data that needs to be passed to the
/// driver(without check bit)</param>
/// <returns>Converted data</returns>
    std::vector<uint8_t> ConvertCmd(uint8_t data[]);
    //针对11位+2位校验位
    std::vector<uint8_t> ConvertCmd2(uint8_t data[], int dataLen);

    uint16_t CRC16_Modbus(uint8_t data[]);


    /// <summary>
    /// Send command data to the driver(without check bit) (Deprecated).
    /// </summary>
    /// <param name="data">the hex data that needs to be passed to the
    /// driver(without check bit)</param>
    /// <returns>the data to be sent after converting</returns>
    //std::vector<uint8_t> SendCmd(uint8_t data[]);

    /// <summary>
/// Send command data to the driver(without check bit).
/// </summary>
/// <param name="data">the hex data that needs to be passed to the
/// driver(without check bit)</param>
/// <returns>the data to be sent after converting</returns>
    std::vector<uint8_t> SendCmd(uint8_t data[], int dataLen);

    /// <summary>
    /// Check if the command data was successfully sent.
    /// </summary>
    /// <param name="data">the input hex data that be written to the
    /// register</param> <param name="data">the data size that read from
    /// register</param> <returns>the buffer read from register</returns>
    std::vector<uint8_t> CmdResult(uint8_t checkData[], size_t read_size);

    /// <summary>
    /// Send and check the data been written to the register.
    /// </summary>
    /// <param name="data">the input hex data that be written to the
    /// register</param>
    /// <param name="data">the data size that read from
    /// register</param>
    /// <returns>the buffer read from register</returns>
    bool SendAndCheckCmd1(uint8_t data[], size_t read_size, size_t data_size = 8);

    bool SendAndCheckCmd(uint8_t data[], size_t read_size);

    serial::parity_t transformToParity(const std::string parity);

    serial::stopbits_t transformToStopBit(const std::string stopbit);

    serial::flowcontrol_t transformToFlowcontrol(const std::string flowcontrol);

 private:
    serial::Serial* m_Serial;
    std::string m_strMotorType;
    CORE::MLIOCommand m_MLIOCommand;
    std::map<int, std::vector<int>> m_FilterPosList;
    int m_totalIdx;
    std::mutex mutex_;
};


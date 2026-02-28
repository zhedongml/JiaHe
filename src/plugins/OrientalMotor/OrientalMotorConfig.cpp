#include "OrientalMotorConfig.h"
#include <mutex>
#include <fstream>
#include <QMessageBox>
#include "loggingwrapper.h"

OrientalMotorConfig *OrientalMotorConfig::m_instance = nullptr;

OrientalMotorConfig::OrientalMotorConfig()
{
    Result ret = ReadDutConfigInfo();
    if (!ret.success)
    {
        QMessageBox::warning(NULL, "Error", QString::fromStdString(ret.errorMsg), QMessageBox::Yes);
    }
}

OrientalMotorConfig::~OrientalMotorConfig()
{
}

OrientalMotorConfig *OrientalMotorConfig::instance()
{
    if (!m_instance)
    {
        static std::mutex mutex;
        std::unique_lock<std::mutex> locker(mutex);
        if (!m_instance)
        {
            m_instance = new OrientalMotorConfig();
        }
    }
    return m_instance;
}

Result OrientalMotorConfig::ReadDutConfigInfo()
{
    string message;
    try
    {
        m_OrientalMotorConfigMap.clear();
        json m_JsonControl;
        std::ifstream ifs(m_ConfigPath.c_str(), std::ifstream::in);
        if (ifs.fail())
        {
            message = "OrientalMotorConfig: load " + m_ConfigPath + " failed";
            LoggingWrapper::instance()->error(
                QString::fromStdString("OrientalMotorConfig: load " + m_ConfigPath + " failed"));
            return Result(false, message);
        }
        ifs >> m_JsonControl;

        if (m_JsonControl.contains("OrientalMotorMap"))
        {
            std::string key;
            for (auto& it : m_JsonControl["OrientalMotorMap"].items())
            {
                MLOrientalConfiguation config;
                key = it.value()["Key"].get<string>();
                config.type = it.value()["Type"].get<string>();
                config.name = it.value()["Name"].get<string>();
                config.protocol = it.value()["Protocol"].get<string>();
                config.motorType = it.value()["MotorType"].get<string>();
                config.port = it.value()["Port"].get<string>();
                config.station = it.value()["Station"].get<int>();
                config.serial_info.baudrate = it.value()["SerialInfo"]["BaudRate"].get<int>();
                config.serial_info.bytesize = it.value()["SerialInfo"]["ByteSize"].get<int>();
                config.serial_info.parity = it.value()["SerialInfo"]["Parity"].get<string>();
                config.serial_info.stopbits = it.value()["SerialInfo"]["StopBit"].get<string>();
                config.serial_info.flowcontrol = it.value()["SerialInfo"]["FlowControl"].get<string>();
                config.MLIOCommand.start = it.value()["IOCommand"]["Start"].get<int>();
                config.MLIOCommand.zHome = it.value()["IOCommand"]["ZHome"].get<int>();
                config.MLIOCommand.stop = it.value()["IOCommand"]["Stop"].get<int>();
                config.MLIOCommand.alarmRst = it.value()["IOCommand"]["AlarmRst"].get<int>();
                config.Init_Index = it.value()["Init_Index"].get<int>();
                config.Move_Index = it.value()["Move_Index"].get<int>();
                config.PulsePerCycle = it.value()["PulsePerCycle"].get<int>();
                config.Min = it.value()["Min"].get<int>();
                config.Max = it.value()["Max"].get<int>();
                int type = SwitchStrToOrientalAxle(key);
                m_OrientalMotorConfigMap[type] = config;
            }
        }
        return Result();
    }
    catch (const std::exception e)
    {
        std::string ex = e.what();
        message = "Read config exception, " + ex;
        LoggingWrapper::instance()->error(QString::fromStdString(message));
        return Result(false, message);
    }
}

MLOrientalConfiguation OrientalMotorConfig::GetOrientalMotorConfig(int type)
{
    if (m_OrientalMotorConfigMap.count(type))
    {
        return m_OrientalMotorConfigMap[type];
    }
    else
    {
        return MLOrientalConfiguation();
    }
}

std::map<int, MLOrientalConfiguation> OrientalMotorConfig::GetOrientalMotorConfigMap()
{
    return m_OrientalMotorConfigMap;
}

std::string OrientalMotorConfig::SwitchOrientalAxleToStr(OrientalAxle axis)
{
    std::string str;
    switch (axis)
    {
    case DX:
        str = "X";
        break;
    case DY:
        str = "Y";
        break;
    case DZ:
        str = "Z";
        break;
    default:
        break;
    }
    return str;
}

OrientalAxle OrientalMotorConfig::SwitchStrToOrientalAxle(std::string str)
{
    if (str == "X")
        return OrientalAxle::DX;
    else if(str == "Y")
        return OrientalAxle::DY;
    else
        return OrientalAxle::DZ;
}
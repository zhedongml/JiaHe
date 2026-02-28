#pragma once
#include "json.hpp"
#include "ml.h"
#include <QObject>

#define AxleX "X"
#define AxleY "Y"
#define AxleZ "Z"

using json = nlohmann::ordered_json;
using namespace std;

enum OrientalAxle
{
    DX = 0,
    DY,
    DZ
};

struct MLOrientalConfiguation {
    std::string type = "OrientalMotor";
    std::string name = "Oriental Motor";
    std::string protocol = "modbus/serial";
    std::string motorType = "Oriental/AZ";
    std::string port;
    int station = 3;
    CORE::MLSerialInfo serial_info;
    bool MLIOCommand_enable = false;
    CORE::MLIOCommand MLIOCommand;
    int PulsePerCycle = 28333;
    int Init_Index = 0;
    int Move_Index = 1;
    int Min = 0;
    int Max = 360;
};

class OrientalMotorConfig
{
  public:
     OrientalMotorConfig();
    ~OrientalMotorConfig();
    static OrientalMotorConfig* instance();

    public:
    Result ReadDutConfigInfo();
    MLOrientalConfiguation GetOrientalMotorConfig(int type);
    std::map<int, MLOrientalConfiguation> GetOrientalMotorConfigMap();
    std::string SwitchOrientalAxleToStr(OrientalAxle axis);
    OrientalAxle SwitchStrToOrientalAxle(std::string str);

  private:
    static OrientalMotorConfig* m_instance;
    std::string m_ConfigPath = "./config/device/OrientalMotorConfig.json";
	std::map<int, MLOrientalConfiguation> m_OrientalMotorConfigMap;
};
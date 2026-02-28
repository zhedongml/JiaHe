#pragma once
#include "ML_addInInterface.h"
#include "MLFilterWheelClass.h"
#include "json.hpp"
#include "MLOrientalMotorControl.h"
#include <QObject>

using json = nlohmann::ordered_json; 
using namespace ML::MLFilterWheel;
using namespace std;

class MLOrientalMotorConfig
{
  public:
      MLOrientalMotorConfig();
    ~MLOrientalMotorConfig();
    static MLOrientalMotorConfig*instance();

    public:
    bool ReadDutConfigInfo();
      InstrumentInfo GetDutMotionInfo();

  private:
    static DUTMotionConfig *m_instance;
    std::string m_ConfigPath = "./config/DUTMotionConfig.json";
    InstrumentInfo m_motionConfig;
};
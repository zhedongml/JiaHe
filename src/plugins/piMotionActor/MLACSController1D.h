#ifndef MLACSCONTROLLER1D_H_
#define MLACSCONTROLLER1D_H_
#include "windows.h"
#include "pimotionactor_global.h"
#include "PrjCommon/service/ml_motion.h"
#include "LogPlus.h"
#include <thread>
#include <list>
#include <iostream>
#include <map>
#include <ACSC.h>
//#include "PrjCommon/service/ml_motion3D.h"

using namespace CORE;

class PIMOTIONACTOR_EXPORT MLACSController1D : public CoreMotion
{
  public:
    MLACSController1D();
    virtual ~MLACSController1D();
    void waitForMotionFinish();

    virtual Result Connect(const char* type_name) override;
    virtual bool Disconnect() override;
    virtual bool IsConnected() override;
    virtual void SetEnable(bool enable) override;
    virtual bool IsEnabled() override;
    virtual Result MoveRelAsync(int posX) override;
    virtual Result MoveAbsAsync(int posX) override;
    virtual Result MoveRel(int posX) override;
    virtual Result MoveAbs(int posX) override;
    virtual Result StopMove() override;
    virtual Result Homing() override;
    virtual bool IsHome() override;
    virtual void SetSpeed(int speedX) override;
    virtual int GetSpeed() override; 
    virtual bool IsMoving() override;
    virtual int GetPosition() override;
    virtual MLMotionState GetState() override;
    virtual void Subscribe(MLMotionEvent event, CoreMotionCallback* callback) override;
    virtual void Unsubscribe(MLMotionEvent event, CoreMotionCallback* callback) override;
    static CoreMotion* createInstance();

private:
    Result judgeInitBuffer();

private:
    HANDLE m_AcsHandle= ACSC_INVALID;
    int m_Reference = 6500;
    int m_RangeLow = 500;
    int m_RangeHigh = 12500;
    int m_VelocityHigh = 12000;
    int m_VelocityLow = 500;
    int m_VelocityNormalX = 9000;
    int m_VelocityNormalY = 9000;
    double m_Scale = 1000.0;
    MLMotionState m_State;
    int m_Axis = 0;
    bool isEnable = true;
    bool m_IsConnected = false;
    int m_TotalAxes = 0;
    std::map<MLMotionEvent,std::list<CoreMotionCallback* >> m_callbacks;

    void invokeStatesChangedCB(MLMotionState old_state, MLMotionState new_state);

    std::thread m_thread_;
    std::atomic<bool> m_runThread_;

    void threadFunc();
    void threadStart();
    void threadStop();
    Result InitialMotionSystem();
    std::string GetErrorMessage();
    std::vector<std::string> split(std::string str, char a);
};
#endif
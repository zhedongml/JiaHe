#ifndef MLPICONTROLLER1D_H_
#define MLPICONTROLLER1D_H_

#include "windows.h"
#include "pimotionactor_global.h"
#include "PrjCommon/service/ml_motion.h"
#include "LogPlus.h"
#include <thread>
#include <list>
#include <iostream>
#include <map>

using namespace CORE;

class PIMOTIONACTOR_EXPORT MLPIController1D : public CoreMotion
{
    Q_OBJECT
public:
    MLPIController1D();
    virtual ~MLPIController1D();
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
    int m_ControllerID = -1;
    int m_Reference = 6500;
    int m_RangeLow = 500;
    int m_RangeHigh = 12500;
    int m_VelocityHigh = 12000;
    int m_VelocityLow = 500;
    int m_VelocityNormalX = 9000;
    int m_VelocityNormalY = 9000;
    double m_Scale = 1000.0;
    MLMotionState m_State;
    string m_Axis = "1";
    bool isEnable = true;

    std::map<MLMotionEvent,std::list<CoreMotionCallback* >> m_callbacks;

    void invokeStatesChangedCB(MLMotionState old_state, MLMotionState new_state);
    void notifyPosition();
    std::thread m_thread_;
    std::atomic<bool> m_runThread_;

    void threadFunc();
    void threadStart();
    void threadStop();
};
#endif
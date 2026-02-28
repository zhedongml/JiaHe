#ifndef MLACSCONTROLLER2D_H_
#define MLACSCONTROLLER2D_H_

#include "pimotionactor_global.h"
#include "PrjCommon/service/ml_motion2D.h"

#include <ACSC.h>
#include <list>
#include <Qlist>
#include <LogPlus.h>
#include <QMap>

using namespace CORE;

class PIMOTIONACTOR_EXPORT MLACSCController2D : public MLMotion2D
{
  public:
    MLACSCController2D(int type);
    ~MLACSCController2D();

  public:
    virtual Result Connect(const char *type_name);

    virtual bool Disconnect();

    virtual bool IsConnected();

    virtual void SetEnable(bool enable);

    virtual bool IsEnabled();

    virtual Result MoveRelAsync(double posX, double posY);

    virtual Result MoveAbsAsync(double pos, double posY);

    virtual Result MoveRel(double posX, double posY);

    virtual Result MoveAbs(double posX, double posY);

     virtual Result StopMove();

     virtual Result Homing();
     virtual Result homingForced();

     virtual Result IsHome(bool &flag,int type);

     virtual void SetSpeed(int speed);

     virtual int GetSpeed();

     virtual bool IsMoving();

     virtual ML_Point GetPosition();

     virtual MLMotionState GetState();

     virtual void Subscribe(MLMotionEvent event, MLMotion2DCallback *callback) ;

     virtual void Unsubscribe(MLMotionEvent event, MLMotion2DCallback *callback);

     virtual void clearAlarm();

    void SetSpeedX(double speed);
    void SetSpeedY(double speed);
    void RegisterEmergencyStop();

    void UnregisterEmergencyStop();

    std::vector<std::string> Split(std::string str, char a);


  private:
    std::string GetErrorMessage();

    Result InitialMotionSystem();

    Result judgeInitBuffer();

    void threadFunc();
    void threadStart();
    void threadStop();

    Result setDirection(const std::vector<std::string>& list);
    float updateDirection(int axis, float value);

    Result judgePos(int posX, int posY);
    Result judgePos(const char* axis, int posDest);
    Result getPosition(const char* axis, int& pos);

  public slots:
    void setBufFlag();
  private:
    QList<int> numBuffer = {};
    bool m_IsConnected = false;
    HANDLE m_AcsHandle = ACSC_INVALID;
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
    int m_TotalAxes = 0;
    std::map<MLMotionEvent, MLMotion2DCallback *> m_Callbacks;
    int X;
    int Y;
    bool m_IsHome;
    std::atomic<bool> m_runThread_;
    std::thread m_CallbackHandle;
    bool m_CallbackFlag;
    bool m_bufFlag = false;

    QMap<int, bool> m_directionMap;
};
#endif

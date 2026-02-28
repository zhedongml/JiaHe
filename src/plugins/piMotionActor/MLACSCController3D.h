#ifndef ACSCONTROLLER_3D_H_
#define ACSCONTROLLER_3D_H_
#include "pimotionactor_global.h"
#include <map>
#include <thread>
#include "PrjCommon/service/ml_motion3D.h"
#include <ACSC.h>
#include <QMap>

using std::map;
using std::thread;

class PIMOTIONACTOR_EXPORT MLACSCController3D : public MLMotion3D
{
public:
    MLACSCController3D();
    ~MLACSCController3D();

public:
    virtual Result Connect(const char* type_name);

    virtual bool Disconnect();

    virtual bool IsConnected();

    virtual void SetEnable(bool enable);

    virtual bool IsEnabled();

    virtual Result MoveRelAsync(int posX, int posY, int posZ);

    virtual Result MoveAbsAsync(int pos, int posY, int posZ);

    virtual Result MoveRelAsync(const char* axis, int pos);

    virtual Result MoveAbsAsync(const char* axis, int pos);

    virtual Result MoveRel(const char *axis, int pos);
    virtual Result MoveAbs(const char *axis, int pos);

    virtual Result MoveRel(int posX, int posY, int posZ);

    virtual Result MoveAbs(int posX, int posY, int posZ);

    virtual Result StopMove();

    virtual Result Homing();

    virtual bool IsHome();

    virtual void SetSpeed(int speed);

    virtual int GetSpeed();

    virtual bool IsMoving();

    virtual CORE::ML_Point3D GetPosition();

    virtual CORE::MLMotionState GetState();

    virtual void Subscribe(CORE::MLMotionEvent event, MLMotion3DCallback *callback);

    virtual void Unsubscribe(CORE::MLMotionEvent event, MLMotion3DCallback *callback);

    virtual void clearAlarm() {};
    void SetSpeedX(double speed);
    void SetSpeedY(double speed);
    void SetSpeedZ(double speed);
    void RegisterEmergencyStop();

    void UnregisterEmergencyStop();

    std::vector<std::string> Split(std::string str, char a);
    HANDLE Connect(const char* address, int port);

    Result setAcsSLimit(cv::Point3f max, cv::Point3f min);

  private:
    void threadFunc();
    Result InitialMotionSystem();
    std::string GetErrorMessage();
    Result judgeInitBuffer();
    Result errorStartOrEnd(bool start);
    Result setDirection(const std::vector<std::string> &list);
    int updateDirection(int axis, int value);

    Result judgePos(int posX, int posY, int posZ);
    Result judgePos(const char *axis, int posDest);
    Result getPosition(const char *axis, int &pos);

  private:
    int X;
    int Y;
    int Z;
    int numBuffer = 0;
    const double m_Scale = 1000.0;
    bool m_IsConnected;
    bool m_Enable;
    CORE::MLMotionState m_State;
    int m_TotalAxes;
    HANDLE m_AcsHandle;
    QMap<CORE::MLMotionEvent, MLMotion3DCallback *> m_Callbacks;
    std::thread m_CallbackHandle;
    bool m_CallbackFlag;
    bool m_IsHome;
   
    QMap<int, bool> m_directionMap;
};

#endif // !ACSCONTROLLER_3D_H_
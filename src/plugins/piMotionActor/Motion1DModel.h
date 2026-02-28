#pragma once
#include "pimotionactor_global.h"
#include "PrjCommon/service/ml_motion.h"
#include<QObject>
#include "Result.h"

class PIMOTIONACTOR_EXPORT Motion1DModel :public QObject,public CoreMotionCallback
{
    Q_OBJECT;
public:
    ~Motion1DModel();

public:
    Result Motion1DMoveRelAsync(int pos);

    Result Motion1DMoveAbsAsync(int pos);

    //void Motion3DMoveRelAsync(const char* axis, int pos);

    //void Motion3DMoveAbsAsync(const char* axis, int pos);

    Result Motion1DMoveRel(int pos);

    Result Motion1DMoveAbs(int pos);

    Result Motion1DMoveStop();

    void Motion1DSetEnabled(bool);
    bool Motion1DisEnabled();

    bool is1DMoving();

    void Motion1DSubscribeMotionEvent(MLMotionEvent event);

    void Motion1DUnsubscribeMotionEvent(MLMotionEvent event);

    bool connect(QString str);//str.c_str();
    Result connectNew(QString str);//str.c_str();
    bool Motion1DisConnected();
    void disconnect();
    int getPosition();
    void NotifyMotionStateChanged(MLMotionState old_state, MLMotionState new_state) override;
    void NotifyMotionPosition(int x) override;
    static Motion1DModel* getInstance(QString type ="PI");
    bool getEStop();
    void setEStop(bool val);

    Result stopMove();

private:
    void updateMotion1DStatus(MLMotionState status);

private:
    Motion1DModel(QString type = "PI");
    static Motion1DModel* self;
    QString motiontype = "PI";
    bool is_Connected = false;
    bool isEStop = false;
signals:
    void updateMotion1DStatusSignal(MLMotionState status);
    void updateMotion1DPositionSignal(float x);
protected:
    CoreMotion* m_pMotion1D = nullptr;
};


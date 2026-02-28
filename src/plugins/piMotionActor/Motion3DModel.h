/****************************************************************************
* @author  : Michael Gao
* @date    : 2021/12/23 
* @file    :
* @brief   : realize x,y,z control and update motion position
*----------------------------------------------------------------------------
*                           Change History
*----------------------------------------------------------------------------
* Date        | Version   | Author         | Description
*----------------------------------------------------------------------------
*             |           |                |
****************************************************************************/

#pragma once
#include "pimotionactor_global.h"
#include "PrjCommon/service/ml_motion3D.h"
#include <QObject>

using namespace CORE;

enum motion3DType
{
    withCamera,
    withDUT
};
class PIMOTIONACTOR_EXPORT Motion3DModel : public QObject,public MLMotion3DCallback
{
    Q_OBJECT;
public:
	~Motion3DModel();

public:
    Result Motion3DMoveRelAsync(int posX, int posY, int posZ);

    Result Motion3DMoveAbsAsync(int pos, int posY, int posZ);

    Result Motion3DMoveRelAsync(const char* axis, int pos);

    Result Motion3DMoveAbsAsync(const char* axis, int pos);

    Result Motion3DMoveRel(const char *axis, int pos);
    Result Motion3DMoveAbs(const char *axis, int pos);

    Result Motion3DMoveRel(int posX, int posY, int posZ);

    Result Motion3DMoveAbs(int posX, int posY, int posZ);

    Result Motion3DMoveStop();

    Result acsMovezeroSqeue();
    void Motion3DSetEnabled(bool);
    bool Motion3DisEnabled();

    bool is3DMoving();

    void Motion3DSubscribeMotionEvent(MLMotionEvent event);

    void Motion3DUnsubscribeMotionEvent(MLMotionEvent event);

    Result connectAndInit(QString ipstr);
    bool connect(QString ipstr);
    void disconnect();
    bool Motion3DisConnected();
    CORE::ML_Point3D getPosition();

    void NotifyMotion3DStateChanged(MLMotionState old_state, MLMotionState new_state) override;
    void NotifyMotion3DPosition() override;

    static Motion3DModel *getInstance(int type);
    bool getEStop();
    void setEStop(bool val);
    void SetSpeed(double speed);
    void SetSpeedX(double speed);
    void SetSpeedY(double speed);
    void SetSpeedZ(double speed);

    int GetSpeed();

    Result setAcsSLimit(cv::Point3f max, cv::Point3f min);

  private:
    Result collisionControl();

  private:
    Motion3DModel(int type);
    static Motion3DModel* selfDUT;
    static Motion3DModel* selfCamera;
    bool is_Connected = false;
    bool isEStop = false;
signals:
    void updateMotion3DStatusSignal(MLMotionState status);
    void updateMotion3DPositionSignal(float x, float y, float z);
protected:
    MLMotion3D* m_pMotion3D = nullptr;
    motion3DType m_type;
};

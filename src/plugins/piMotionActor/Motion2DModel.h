#pragma once
#include "PrjCommon/service/ml_motion2D.h"
#include "pimotionactor_global.h"
#include <QObject>
using namespace CORE;
enum motion2DType
{
    ACS2DDUT,
    ACS2DPro,
    ACS2DReticle,
    ACS2DDUTPlane,
    ACS2DCameraTilt
};
class PIMOTIONACTOR_EXPORT Motion2DModel : public QObject, public MLMotion2DCallback
{
    Q_OBJECT;

  public:
    ~Motion2DModel();

  public:
    Result Motion2DMoveRelAsync(double posX, double posY);

    Result Motion2DMoveAbsAsync(double posX, double posY);

    Result Motion2DMoveRel(double posX, double posY);

    Result Motion2DMoveAbs(double posX, double posY);

    Result Motion2DMoveStop();

    bool is2DMoving();

    Result connectAndInit(QString ipstr);

    bool disconnect();

    bool Motion2DisConnected();

    CORE::ML_Point getPosition();

    bool getPosition(float &x, float &y);

    void SetSpeed(int speed);

    Result Home();

    Result isHoming();

    Result homingForced();

    void Motion2DSubscribeMotionEvent(CORE::MLMotionEvent event);

    void Motion2DUnsubscribeMotionEvent(CORE::MLMotionEvent event);

    void NotifyMotion2DStateChanged(CORE::MLMotionState old_state, CORE::MLMotionState new_state) override;

    void NotifyMotion2DPosition() override;

    static Motion2DModel *getInstance(int type);

    int GetSpeed();

    bool GetIsConnected();

    bool getEStop();
    void setEStop(bool val);

    bool getPositionRaw(cv::Point2f &pos);
    Result Motion2DMoveAbsRaw(cv::Point2f &pos);

    void updateMotion2DZero(int eyeType, const cv::Point2f &zeroPos);

    void SetSpeedX(int speed);
    void SetSpeedY(int speed);

  signals:
    void updateMotion2DStatusSignal(MLMotionState status);
    void updateMotion2DPositionSignal(float x, float y);
    void updateMotion2DPositionOriginalSignal(float x, float y);

  private slots:
    void updateDutConfigSlot();

  private:
    Motion2DModel(int type);
    void updatePositon(double &posX, double &posY);
    Result collisionControl();
    Result judgeLimit(cv::Point2f target);
    Result judgeLimitRel(cv::Point2f offset);

    Result getPosition(cv::Point2f &coord, cv::Point2f &coordOriginal);

  private:
    static Motion2DModel *selfACS2DPro;
    static Motion2DModel *selfACS2DDUT;
    static Motion2DModel *selfACS2DReticle;
    static Motion2DModel* selfACS2DDUTPlane;
    static Motion2DModel* selfACS2DCameraModule;

    bool is_Connected = false;
    double m_scale = 1000.0;
    int typeMotion;
    // signals:
    //  void updateMotion3DStatusSignal(MLMotionState status);
    //  void updateMotion3DPositionSignal(float x, float y, float z);
    bool isEStop = false;
    bool isHome = false;
    cv::Point2f m_zeroPosition = cv::Point2f(0,0);
  protected:
    MLMotion2D *m_pMotion2D = nullptr;
};

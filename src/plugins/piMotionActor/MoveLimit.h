#pragma once
#include "MoveLimitConfig.h"
#include "pimotionactor_global.h"
#include "Motion3DModel.h"
#include "Motion2DModel.h"
#include "calibrationconfig.h"

#define UNIT_CONVERSION 1000.0

struct InputAlignState{
    bool isAlign = false;
    cv::Point3f alignCoord;

    QString dutSN;
    int eyeType;
    cv::Point2f dutAngle;
    cv::Point2f prAngle;

    InputAlignState(){}

    InputAlignState(bool isAlignT, cv::Point3f alignCoordT)
    {
        isAlign = isAlignT;
        alignCoord = alignCoordT;
    }
};

class AlignCalibrateCallback
{
  public:
    virtual void notifyInputAAStateChanged(bool isAlign) = 0;
};

class PIMOTIONACTOR_EXPORT MoveLimit
{
  public:
    static MoveLimit *instance();
    ~MoveLimit();

    Result setInputAlignState(bool isAlign);
    bool completeInputAA();
    Result resetLastInputAA();

    Result setMotion3DMoveAlignSpeed(bool flag);

    void subscribe(AlignCalibrateCallback *callback);
    void unsubscribe(AlignCalibrateCallback *callback);

    static Result moveAbs(const cv::Point3f &coordinate, motion3DType type);
    static Result moveRel(const cv::Point3f &coord, motion3DType type);

    static Result moveAbsNoCollision(const cv::Point3f &coordinate, motion3DType type);
    static Result moveRelNoCollision(const cv::Point3f &coord, motion3DType type);

    static Result moveAbsToSafe( motion3DType type);

    static Result dutMoveAbs(const cv::Point3f &coordinate);
    static Result mcMoveAbs(const cv::Point3f &coordinate);

    static Result judgeDut(const cv::Point3f &coordinate);
    static Result judgeMeasureCamera(const cv::Point3f &coordinate);

    static Result moveInit();
    static Result getCoordinate(motion3DType motionType, cv::Point3f &coord);

    static Result moveAbs2DTilt(const cv::Point2f &coordinate, motion2DType type);
    static Result getCoordinate2DTilt(motion2DType motionType, cv::Point2f &coord);

    static Result moveBeforeInputAA();
    static Result judgeEyeboxLimit(const std::vector<cv::Point3f> &eyeboxs);
    static Result judgeEyeboxLimit(const cv::Point3f &destCoordMc, int eyeboxIndex = -1);

  private:
    MoveLimit();
    static Result dutMoveAbsNoCollision(const cv::Point3f &coordinate);
    static Result mcMoveAbsNoCollision(const cv::Point3f &coordinate);

    static Result dutMoveOrder(const cv::Point3f &coordinate);
    static Result mcMoveOrder(const cv::Point3f &coordinate);

    static Result dutMcCollision(const cv::Point3f &dutCoord, const cv::Point3f &mcCoord);
    static Result dutProjectorCollision(const cv::Point3f &dutCoord);
    static Result dutProjectorCollisionMoveUp(const cv::Point3f &destCoord);

    static Result dutPreJudge(const cv::Point3f &coordinate);
    static Result mcPreJudge(const cv::Point3f &coordinate);
    static Result preJudge(const cv::Point3f &coordinate, const CoordinateLimit &limit);

    static QString coordToString(const cv::Point3f &coordinate);

    Result isDutChange(const InputAlignState &inputAlignState);
    static Result dutMoveToSafeBeforeMoveUp(const cv::Point3f &destCoord);
    static Result dutProjectorCollisionMoveUpCal(float &offsetZ, const cv::Point3f &dutPrNearCoordinate,
                                                 const cv::Point3f &dutCurrentCoord, const cv::Point3f &destCoord);

  private:
    static MoveLimit *m_instance;
    InputAlignState m_inputAlignState;
    QMap<motion3DType, cv::Point3i> m_motion3DSpeedMap;
    QList<AlignCalibrateCallback*> m_alignCalibrateCallbacks;
};

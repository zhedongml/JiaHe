#pragma once
#include "MoveLimitConfigJX.h"
#include "pimotionactor_global.h"
#include "Motion3DModel.h"
#include "Motion2DModel.h"
#include "calibrationconfig.h"
#include "ConfigItem.h"
#include "MoveLimit.h"

#define UNIT_CONVERSION 1000.0

struct AlignStateJX {
    bool isAlign = false;
    cv::Point2f inputAlignCoord;

    QString dutSN;
    int eyeType;
    cv::Point2f prAngle;

    AlignStateJX() {}

    AlignStateJX(bool isAlignT, cv::Point2f alignCoordT)
    {
        isAlign = isAlignT;
        inputAlignCoord = alignCoordT;
    }
};


class PIMOTIONACTOR_EXPORT MoveLimitJX
{
  public:
    static MoveLimitJX *instance();
    ~MoveLimitJX();

    Result setAlignState(bool isAlign, const cv::Point2f &inputAlignCoord = cv::Point2f());
    bool completeAA();
    Result resetLastInputAA();

    Result setMotion3DMoveAlignSpeed(bool flag);

    void subscribe(AlignCalibrateCallback* callback);
    void unsubscribe(AlignCalibrateCallback* callback);

    static Result moveAbs(const cv::Point3f &coordinate, motion3DType type = withCamera);
    static Result moveRel(const cv::Point3f &coord, motion3DType type = withCamera);

    static Result moveAbsNoCollision(const cv::Point3f &coordinate, motion3DType type = withCamera);
    static Result moveRelNoCollision(const cv::Point3f &coord, motion3DType type = withCamera);

    static Result moveAbsToSafe( motion3DType type = withCamera);
    static Result moveDutAbsToSafe();

    static Result dutMoveAbs(const cv::Point2f&coordinate);
    static Result mcMoveAbs(const cv::Point3f &coordinate);

    static Result dutMoveRel(const cv::Point2f& coordinate);
    static Result mcMoveRel(const cv::Point3f& coordinate);

    static Result judgeDut(const cv::Point2f& coordinate);
    static Result judgeMeasureCamera(const cv::Point3f &coordinate);

    static Result moveInit();
    static Result getCoordinate3D(cv::Point3f &coord, motion3DType motionType = withCamera);

    static Result moveAbs2DTilt(const cv::Point2f &coordinate, motion2DType type = ACS2DPro);
    static Result getCoordinate2DTilt(cv::Point2f &coord, motion2DType motionType = ACS2DPro);
    static Result getCoordinate2DTiltRaw(cv::Point2f& coord, motion2DType motionType = ACS2DPro);
    static Result getCoordinate2D(cv::Point2f& coord, motion2DType motionType = ACS2DDUTPlane);

    static Result moveBeforeInputAA(const cv::Point2f &dutCoord);
    static Result judgeEyeboxLimit(const std::vector<cv::Point3f> &eyeboxs);
    static Result judgeEyeboxLimit(const cv::Point3f &destCoordMc, int eyeboxIndex = -1);

    static Result dutMoveRelNoCollision(const cv::Point2f& coordinate);
    static Result dutMoveAbsNoCollision(const cv::Point2f& coordinate);
    static Result mcMoveAbsNoCollision(const cv::Point3f& coordinate);

    static Result moveDutInitTestPos();
    static Result moveMcInitTestPos();

  private:
    MoveLimitJX();

    static Result dutMoveOrder(const cv::Point2f &coordinate);
    static Result mcMoveOrder(const cv::Point3f &coordinate);

    static Result dutMcCollision(const cv::Point2f &dutCoord, const cv::Point3f &mcCoord);
    static Result dutProjectorCollision(const cv::Point2f &dutCoord);
    static Result mcProjectorCollision(const cv::Point3f& mcCoord);

    static Result dutPreJudge(const cv::Point2f&coordinate);
    static Result mcPreJudge(const cv::Point3f &coordinate);
    static Result preJudge(const cv::Point3f &coordinate, const CoordinateLimit &limit);
    static Result preJudge(const cv::Point2f& coordinate, const CoordinateLimit& limit);
    static Result preJudge(const cv::Point2f& coordinate, const Limit2D& limit);

    static QString coordToString(const cv::Point3f &coordinate);
    static QString coordToString(const cv::Point2f& coordinate);

    Result isDutChange(const AlignStateJX &inputAlignState);

  private:
    static MoveLimitJX *m_instance;
    QMap<motion3DType, cv::Point3i> m_motion3DSpeedMap;
    AlignStateJX m_alignState;
    QList<AlignCalibrateCallback*> m_alignCalibrateCallbacks;
};

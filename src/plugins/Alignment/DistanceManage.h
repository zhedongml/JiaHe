#pragma once
#include "alignment_global.h"
#include "PrjCommon/service/ml_motion.h"
#include "piMotionActor/calibrationconfig.h"
#include "WaitPause.h"
#include "MLKeyenceLRF/MLKeyenceCL.h"

using namespace CORE;

namespace Align
{
class ALIGNMENT_EXPORT DistanceManage
{
  public:
    explicit DistanceManage();
    ~DistanceManage();

    // continue or stop
    void continueRun(bool isContinue);
    void notifyPause(bool isPause);

    Result eyeRelief(const EyeReliefBaseInfo &baseInfo);

    Result eyeRelief(const EyeReliefBaseInfo &baseInfo, double &acsZ);

    Result getKeyenceToDutDistance(double &distance, MLKeyenceCLType type);

    Result keyenceAcsByLens(cv::Point3f &keyenceAcs, const AlignBaseInfo3D &alignInfo,
                            const EyeReliefBaseInfo &eyeReliefInfo, cv::Point3f lensAcs);

    cv::Point3f keyenceAcsByMv(const AlignBaseInfo3D &alignInfo, const EyeReliefBaseInfo &eyeReliefInfo,
                               cv::Point3f mvFiducialCoord);

    Result fiducialSameDistance(float &distance, const EyeReliefBaseInfo &baseInfo);

    std::vector<cv::Point3f> keyenceToLens(const EyeReliefBaseInfo &baseInfo, const std::vector<cv::Point3f> &keyenceCoords, float distance);

  private:
    Result getKeyenceToDutDistance(double &distance, MinMax keyenceRange, double keyenceZeroDistance, MLKeyenceCLType type = KEYENCE_TYPE_UP);

    // Working distance: To facilitate verify, print info
    QString eyeReliefPrintMessage(const EyeReliefBaseInfo &baseInfo);

    Result refreshAcsBaseCoord(cv::Point3f &acsCoord);

    Result waitPause(bool isPause);

    Result keyenceLight(MLKeyenceCLType type, bool isOpen);

  private:
    WaitPause m_waitPause;
};
}; // namespace Align

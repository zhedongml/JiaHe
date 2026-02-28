#include "DistanceManage.h"
#include "windows.h"
#include "piMotionActor/Motion3DModel.h"
#include <synchapi.h>
#include "AlignUtils.h"
#include "PrjCommon/service/ml_rangefinder.h"
#include "PLCControl/PLCController.h"
#include "piMotionActor/MoveLimitJX.h"
#include "Core/loggingwrapper.h"
#include "piMotionActor/ConfigItem.h"

namespace Align{

DistanceManage::DistanceManage()
{
}

DistanceManage::~DistanceManage()
{
}

void DistanceManage::continueRun(bool isContinue)
{
    m_waitPause.continueRun(isContinue);
}

void DistanceManage::notifyPause(bool isPause)
{
    m_waitPause.notifyPause(isPause);
}

Result DistanceManage::eyeRelief(const EyeReliefBaseInfo &baseInfo)
{
    double acsZ;
    Result result = eyeRelief(baseInfo, acsZ);
    if (!result.success)
    {
        return result;
    }

    cv::Point3f acsCoordCurrent;
    result = AlignUtils::getAcsCoordinate(acsCoordCurrent);
    if (!result.success)
    {
        return result;
    }
    acsCoordCurrent.z = acsZ;

    result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        return result;
    }

    result = AlignUtils::acsAbsoluteMove(acsCoordCurrent, baseInfo.acsCoordLimit);
    return result;
}

Result DistanceManage::eyeRelief(const EyeReliefBaseInfo &baseInfo, double &acsZ)
{
    if (!baseInfo.judgeDataValidity()){
        return Result(false, "Eye relief base info is incomplete.");
    }

    double dueDistance = baseInfo.dueDistance;
    cv::Point3f acsCoord = baseInfo.acsCoordinate;
    CORE::CoordinateLimit acsCoordLimit = baseInfo.acsCoordLimit;

    Result result = refreshAcsBaseCoord(acsCoord);
    if (!result.success)
    {
        return result;
    }

    result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        return result;
    }

    result = AlignUtils::acsAbsoluteMove(acsCoord, acsCoordLimit);
    if (!result.success)
    {
        return result;
    }

    double keyenceToDutDis;
    result = getKeyenceToDutDistance(keyenceToDutDis, baseInfo.keyenceRange, baseInfo.keyenceZeroDistance);
    if (!result.success)
    {
        return result;
    }

    if (keyenceToDutDis == 0.0)
    {
        int loop = 0;
        while (abs(keyenceToDutDis) < 0.0001 && loop < 2)
        {
            result = waitPause(m_waitPause.recipePause());
            if (!result.success)
            {
                return result;
            }

            result = AlignUtils::acsRelativeMoveZ(-1, acsCoordLimit.min.z, acsCoordLimit.max.z);
            if (!result.success)
            {
                return result;
            }
            loop++;
            Sleep(100);
        }
    }

    if (abs(keyenceToDutDis) < 0.0001)
    {
        return Result(false, "Keyence value is invalid , value is 0.");
    }

    result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        return result;
    }
    
    double diff = dueDistance - (keyenceToDutDis - (baseInfo.keyenceRelativeMvCoord.z - baseInfo.i29RelativeMvCoordZ));
    result = AlignUtils::acsRelativeMoveZ(-diff, acsCoordLimit.min.z, acsCoordLimit.max.z);
    if (!result.success)
    {
        return result;
    }

    result = AlignUtils::getAcsCoordinate(acsCoord);
    if (!result.success)
    {
        return result;
    }
    acsZ = acsCoord.z;
    
    QString message = eyeReliefPrintMessage(baseInfo);
    return Result(true, message.toStdString());
}

Result DistanceManage::getKeyenceToDutDistance(double &distance, MLKeyenceCLType type)
{
    KeyenceInfo info = ConfigItem::instance()->getKeyenceInfo();
    MinMax keyenceRange = MinMax(info.keyenceRangeMin, info.keyenceRangeMax);
    double keyenceZeroDistance = info.keyenceZeroPos;
    return getKeyenceToDutDistance(distance, keyenceRange, keyenceZeroDistance, type);
}

Result DistanceManage::keyenceAcsByLens(cv::Point3f &keyenceAcs, const AlignBaseInfo3D &alignInfo,
                                               const EyeReliefBaseInfo &eyeReliefInfo, cv::Point3f lensAcs)
{
    //lensAcs: measure camera alignment eyebox, measure camera and eyebox distance is MV camera focus
    keyenceAcs.x = lensAcs.x + (eyeReliefInfo.keyenceRelativeMvCoord.x - alignInfo.i29RelativeMvCoord.x);
    keyenceAcs.y = lensAcs.y + (eyeReliefInfo.keyenceRelativeMvCoord.y - alignInfo.i29RelativeMvCoord.y);

    float keyenceToDutFocusZ = lensAcs.z + eyeReliefInfo.keyenceRelativeMvCoord.z;
    keyenceAcs.z = keyenceToDutFocusZ + (alignInfo.mvFocus - eyeReliefInfo.keyenceZeroDistance);
    return Result();
}

cv::Point3f DistanceManage::keyenceAcsByMv(const AlignBaseInfo3D &alignInfo, const EyeReliefBaseInfo &eyeReliefInfo, cv::Point3f mvFiducialCoord)
{
    // mvFiducialCoord: MV alignment fiducial, MV and fiducial distance is MV camera focus
    cv::Point3f keyenceAcs;
    keyenceAcs.x = mvFiducialCoord.x + eyeReliefInfo.keyenceRelativeMvCoord.x;
    keyenceAcs.y = mvFiducialCoord.y + eyeReliefInfo.keyenceRelativeMvCoord.y;

    float keyenceToDutFocusZ = mvFiducialCoord.z + eyeReliefInfo.keyenceRelativeMvCoord.z;
    keyenceAcs.z = keyenceToDutFocusZ + (alignInfo.mvFocus - eyeReliefInfo.keyenceZeroDistance);
    return keyenceAcs;
}

Result DistanceManage::fiducialSameDistance(float &distance, const EyeReliefBaseInfo &baseInfo)
{
    Result ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    distance = baseInfo.keyenceZeroDistance + baseInfo.keyenceRange.max/2.0 + 2;

    cv::Point3f acsCoordinate = baseInfo.acsCoordinate;
    acsCoordinate.z = acsCoordinate.z + baseInfo.keyenceRange.min/2.0 - 2;
    ret = MoveLimitJX::moveAbs(acsCoordinate, withCamera);
    if (!ret.success)
    {
        return ret;
    }
    
    double distanceCurrent = 0;
    for (int i = 0; i < 5; ++i)
    {
        ret = getKeyenceToDutDistance(distanceCurrent, baseInfo.keyenceRange, baseInfo.keyenceZeroDistance);
        if (!ret.success)
        {
            return ret;
        }

        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        if (abs(distanceCurrent - distance) > 0.01)
        {
            ret = MoveLimitJX::moveRel(cv::Point3f(0, 0, distanceCurrent - distance), withCamera);
            if (!ret.success)
            {
                return ret;
            }
        }
        else
        {
            break;
        }
    }

    ret = getKeyenceToDutDistance(distanceCurrent, baseInfo.keyenceRange, baseInfo.keyenceZeroDistance);
    if (!ret.success)
    {
        return ret;
    }

    //TODO: temp 
    if (abs(distance - distanceCurrent) > 0.01)
    {
        LoggingWrapper::instance()->info(
            QString("Fiducial same distance error, offsetZ=%1.").arg(distance - distanceCurrent));
        //return Result(false, QString("Fiducial same distance error, offsetZ=%1.").arg(distance - distanceCurrent).toStdString());
    }

    if (abs(distance - distanceCurrent) > 0.05)
    {
        return Result(false, QString("Fiducial same distance error, offsetZ=%1.").arg(distance - distanceCurrent).toStdString());
    }
    return Result();
}

std::vector<cv::Point3f> DistanceManage::keyenceToLens(const EyeReliefBaseInfo &baseInfo, 
    const std::vector<cv::Point3f> &keyenceCoords, float distance)
{
    std::vector<cv::Point3f> lensCoords;
    for (cv::Point3f coord : keyenceCoords)
    {
        cv::Point3f lensCoord;
        lensCoord.x = coord.x + (baseInfo.i29RelativeMvCoord.x - baseInfo.keyenceRelativeMvCoord.x);
        lensCoord.y = coord.y + (baseInfo.i29RelativeMvCoord.y - baseInfo.keyenceRelativeMvCoord.y);
        lensCoord.z = coord.z + ((distance - baseInfo.i29RelativeMvCoord.z) - baseInfo.dueDistance);
        lensCoords.push_back(lensCoord);
    }
    return lensCoords;
}

Result DistanceManage::getKeyenceToDutDistance(double &distance, MinMax keyenceRange, double keyenceZeroDistance, MLKeyenceCLType type)
{
    MLKeyenceCL *keyence = (MLKeyenceCL*)MLKeyenceCL::MakeRangeFinder(type);
    if(!keyence->IsConnected()){
        return Result(false, "Get dut Z distance error, confocal distance sensor is not connected.");
    }

    if (keyence != nullptr)
    {
        keyenceLight(type, true);
        //keyence->turnLightControl(true);
        double pos = 0;
        for (int i = 0; i < 5; ++i)
        {
            pos = keyence->GetPosition(0);
            if (pos > keyenceRange.max || pos < keyenceRange.min)
            {
                Sleep(500);
            }
            else
            {
                break;
            }
        }
       
        if (pos > keyenceRange.max || pos < keyenceRange.min)
        {
            keyenceLight(type, false);
            return Result(false, QString("Confocal distance sensor measure distance failed, pos is %1.").arg(pos).toStdString());
        }

        distance = keyenceZeroDistance - pos;
        //keyence->turnLightControl(false);
        keyenceLight(type, false);
        return Result();
    }
    else
    {
        //keyence->turnLightControl(false);
        keyenceLight(type, false);
        return Result(false, "Confocal distance sensor measure distance failed, MLKeyenceCL new error.");
    }
}

QString DistanceManage::eyeReliefPrintMessage(const EyeReliefBaseInfo &baseInfo)
{
    double curDis;
    Result result = getKeyenceToDutDistance(curDis, baseInfo.keyenceRange, baseInfo.keyenceZeroDistance);
    if (!result.success)
    {
        return "";
    }
    QString message = QString("eye-relief,due distance from i29 camera to dut is %1; real distance "
                              "from i29 camera to dut is %2-%3=%4; Confocal distance sensor measure is %5")
                          .arg(baseInfo.dueDistance)
                          .arg(curDis)
                          .arg(baseInfo.keyenceRelativeMvCoord.z - baseInfo.i29RelativeMvCoordZ)
                          .arg(curDis - (baseInfo.keyenceRelativeMvCoord.z - baseInfo.i29RelativeMvCoordZ))
                          .arg(curDis);
    return message;
}

Result DistanceManage::refreshAcsBaseCoord(cv::Point3f &acsCoord)
{
    if (acsCoord == cv::Point3f())
    {
        Result result = AlignUtils::getAcsCoordinate(acsCoord);
        if (!result.success)
        {
            return result;
        }
    }
    return Result();
}

Result DistanceManage::waitPause(bool isPause)
{
    return m_waitPause.waitPause(isPause);
}

Result DistanceManage::keyenceLight(MLKeyenceCLType type, bool isOpen)
{
    if(KEYENCE_TYPE_UP == type){
        PLCController::instance()->keyenceLightUp(isOpen);
    }else{
        PLCController::instance()->keyenceLight(isOpen);
    }
    return Result();
}

}

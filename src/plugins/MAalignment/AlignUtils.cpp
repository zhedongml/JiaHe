#include "AlignUtils.h"
#include <integratingSphere\integratingspheretcpmodel.h>
#include <PLCControl\PLCController.h>
#include "piMotionActor/MoveLimit.h"
#include "piMotionActor/DutProjectorCalibrate.h"
#include "integratingSphere/ReticleMode2D.h"

Result AlignUtils::acsAbsoluteMove(cv::Point3f &acsCoord)
{
    Result ret = MoveLimit::dutMoveAbs(acsCoord);
    if(!ret.success){
        return ret;
    }

    ML_Point3D point = Motion3DModel::getInstance(withDUT)->getPosition();
    acsCoord = cv::Point3f(point.x / UNIT_CONVERSION, point.y / UNIT_CONVERSION, point.z / UNIT_CONVERSION);
    return Result();
}

Result AlignUtils::acsAbsoluteMove(const cv::Point2f &acsCoord)
{
    ML_Point3D point = Motion3DModel::getInstance(withDUT)->getPosition();
    cv::Point3f acsCoord3D = cv::Point3f(acsCoord.x, acsCoord.y, point.z / UNIT_CONVERSION);
    return MoveLimit::dutMoveAbs(acsCoord3D);
}

Result AlignUtils::acsRelMove(const cv::Point3f &acsCoord)
{
    ML_Point3D point = Motion3DModel::getInstance(withDUT)->getPosition();
    cv::Point3f acsCoord3D = cv::Point3f(
        acsCoord.x + (point.x / UNIT_CONVERSION), 
        acsCoord.y + (point.y / UNIT_CONVERSION), 
        acsCoord.z + (point.z / UNIT_CONVERSION));
    return MoveLimit::dutMoveAbs(acsCoord3D);
}

Result AlignUtils::acsRelMove(const cv::Point2f &acsCoord)
{
    return acsRelMove(cv::Point3f(acsCoord.x, acsCoord.y, 0));
}

void AlignUtils::acsMoveStop(bool stop)
{
    if (stop)
    {
        Motion3DModel::getInstance(withDUT)->setEStop(stop);
        Motion3DModel::getInstance(withCamera)->setEStop(stop);
        Motion2DModel::getInstance(ACS2DDUT)->setEStop(stop);
        Motion2DModel::getInstance(ACS2DPro)->setEStop(stop);

        Motion3DModel::getInstance(withDUT)->Motion3DMoveStop();
        Motion3DModel::getInstance(withCamera)->Motion3DMoveStop();
        Motion2DModel::getInstance(ACS2DDUT)->Motion2DMoveStop();
        Motion2DModel::getInstance(ACS2DPro)->Motion2DMoveStop();
    }
}

Result AlignUtils::getAlignConfig(AAlignConfig &config, DUT_EYE_TYPE eyeType)
{
    MAInfo info = ConfigMAInfo::GetInstance()->ReadMAInfo();
    config.stepZ = info.stepZ;

    cv::Point3f dutPrNearCoordinate;
    Result ret = DutProjectorCalibrate::instance()->getDutPrNearCoordinate(eyeType, dutPrNearCoordinate);
    if (!ret.success)
    {
        return ret;
    }

    config.inView = cv::Point2f(dutPrNearCoordinate.x, dutPrNearCoordinate.y);
    config.outView = cv::Point2f(config.inView.x + 30, config.inView.y);

    config.distanceLimit.low = dutPrNearCoordinate.z + 1;
    config.distanceLimit.high = dutPrNearCoordinate.z + 20;
    return Result();
}

Result AlignUtils::controlLight(bool IS, bool coaxialLight)
{
    Result ret;
    ret = PLCController::instance()->coaxialLight2(false);
    if (!ret.success)
    {
        return Result(false, "Close coaxial light 2 failed.");
    }

    ret = PLCController::instance()->lightBoard(false);
    if (!ret.success)
    {
        return Result(false, "Close light Board failed.");
    }

    if (IS)
    {
        ISSolution sulution;
        ConfigMAInfo::GetInstance()->getIntergratingSphere(sulution.name, sulution.scale);
        sulution.colorFocus = sulution.name;
        sulution.currentOutputs.push_back(sulution.scale);
        ret = IntegratingSphereTCPModel::getInstance()->setSolution(sulution);
    }
    else
    {
        ret = IntegratingSphereTCPModel::getInstance()->setAllCurrentOutputZero();
    }
    if (!ret.success)
    {
        std::string msg = IS ? "Open intergrating sphere failed, " : "Close intergrating sphere failed, ";
        return Result(false, msg + ret.errorMsg);
    }

    ret = PLCController::instance()->coaxialLight(coaxialLight);
    if (!ret.success)
    {
        std::string msg = coaxialLight ? "Open coaxial light failed." : "Close coaxial light failed.";
        return Result(false, msg);
    }

    if (IS)
    {
        QString reticle = ConfigMAInfo::GetInstance()->getReticle();
        ret = ReticleMode2D::instance()->changeReticle(reticle, ReticleMode2D::instance()->getEyeType());
        if (!ret.success)
        {
            return ret;
        }
    }
    return Result();
}
#include "AlignWrapper.h"
#include "AlignUtils.h"
#include <piMotionActor\Motion2DModel.h>
#include "piMotionActor\configItem.h"
//#include "alignment.h"
#include "Core/modemanager.h"
#include "pluginsystem/Services.h"
#include "Core/loggingwrapper.h"
#include <PrjCommon/service/ml_camera.h>
#include "Alignment2D.h"
#include "Alignment3D.h"
#include <plugins/PrjCommon/metricsdata.h>
#include "CaptureFiducialImage.h"
#include "piMotionActor/MoveLimitJX.h"
#include "PLCControl/PLCController.h"

using namespace Align;

AlignWrapper::AlignWrapper(QObject *parent, CalibrateWidget *calibrateWidget)
    : IRecipeWrapper(parent), m_calibrateWidget(calibrateWidget)
{
}

AlignWrapper::~AlignWrapper()
{
}

void AlignWrapper::Invoke(QString cmd, QString param)
{
    int startTime = 0;
    int takeTime = 0;
    qWarning() << "---- takt time ---- [Alignment " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
               << "] start.";
    startTime = QDateTime::currentMSecsSinceEpoch();

    Result result;
    if (cmd == "deviceConnect")
    {
        result = deviceConnect(param);
    }
    else if (cmd == "motion3DConnect")
    {
        result = motion3DConnect(param);
    }
    else if (cmd == "eyeRelief")
    {
        result = eyeRelief(param);
    }
    else if (cmd == "dutConfig")
    {
        result = dealDutConfig(param);
    }
    else if (cmd == "calibrate")
    {
        Core::ModeManager::instance()->activateMode("CalibrateMent.CalibrateMode");
        result = startCalibrate2D(param);
        //TODO: RAM synchronous file config
        CalibrationConfig::instance()->load();
        Core::ModeManager::instance()->activateMode("Recipe.RecipeMode");
    }
    else if (cmd == "calibrateConfig")
    {
        result = calibrateConfig(param);
    }
    else if (cmd == "eyebox")
    {
        result = moveAlignEyebox(param);
    }
    else if (cmd == "MVCamera")
    {
        result = setCamera(param);
    }
    else if (cmd == "MvExpose")
    {
        result = mvExpose(param);
    }
    else if (cmd.toLower() == QString("loadInitPosition").toLower())
    {
        result = loadInitPosition(param);
    }
    else if (cmd.toLower() == QString("turnOffLights").toLower())
    {
        result = PLCController::instance()->closeLightBeforeRecipe();
    }
    else if (cmd.toLower() == QString("imageDir").toLower())
    {
        result = createImageDir(param);
    }
    else if (cmd == "setAngle")
    {
        result = setAngle(param);
    }
    else if (cmd == "pupilDecenter")
    {
        result = pupilDecenter(param);
    }
    else
    {
        result = Result(false, QString("\"%1\" cmd is mismatch, please check the execution fields.").arg(cmd).toStdString());
    }

    takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "---- takt time ---- [Alignment " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
               << "] end. times:" << takeTime << " ms...";

    if (!result.success)
    {
        QString message =
            QString("Recipe [%1:%2] run error, %3").arg(cmd).arg(param).arg(QString::fromStdString(result.errorMsg));
        LoggingWrapper::instance()->error(message);
        emit SendStatusSignal(Wrapper_Error_fatal, QString::fromStdString(result.errorMsg));
    }
    else
    {
        emit SendStatusSignal(Wrapper_Done, "Calibration is done!");
    }
}

void AlignWrapper::notifyStop(bool isstop)
{
    m_isStop = isstop;

    if (m_calibrateWidget->isAlignWidgetRun())
    {
        return;
    }

    Alignment2D::instance()->continueRun(!isstop);
}

void AlignWrapper::notifyPause(bool isPause)
{
    if (m_calibrateWidget->isAlignWidgetRun())
    {
        return;
    }
    Alignment2D::instance()->notifyPause(isPause);
}

Result AlignWrapper::eyeRelief(QString param)
{
    int eyeType = CalibrationConfig::instance()->getDutConfig().eyeType;
    EyeReliefBaseInfo baseInfo = AlignUtils::getEyeReliefBaseInfo(eyeType);
    Align::DistanceManage().eyeRelief(baseInfo);
    return Result();
}

Result AlignWrapper::startCalibrate(QString param)
{
    if (m_calibrateWidget->isAlignWidgetRun())
    {
        return Result(false, "Calibration is running, please stop and start again.");
    }

    int eyeType = CalibrationConfig::instance()->getDutConfig().eyeType;
    AlignConfig alignConfig = AnalyzeRecipe().analyzCalibrateParam(param);

    // TODO: select light: coaxialLight or lightBoard
    Result result = AlignUtils::fiducialLight(true);
    if (!result.success)
    {
        return result;
    }

    if (alignConfig.fiducialCount == 1)
    {
        AlignBaseInfoOneFiducialTilt alignInfo = AlignUtils::getAlignBaseInfoOneFiducialTilt(eyeType);
      //  result = Alignment::instance()->alignEyeboxAcsByOneFiducialXYZ(alignInfo, alignConfig);
    }
    else
    {
        AlignBaseInfoTilt alignInfo = AlignUtils::getAlignBaseInfoTilt(eyeType);
       // result = Alignment::instance()->alignEyeboxAcsXYZ(alignInfo, alignConfig);
    }
    if (!result.success)
    {
        return result;
    }

    result = AlignUtils::fiducialLight(false);
    if (!result.success)
    {
        return result;
    }
    return result;
}

Result AlignWrapper::startCalibrate3D(QString param)
{
    if (m_calibrateWidget->isAlignWidgetRun())
    {
        return Result(false, "Calibration is running, please stop and start again.");
    }

    AlignConfig alignConfig = AnalyzeRecipe().analyzCalibrateParam(param);

    AlignBaseInfo3D alignInfo;
    Result ret = AlignUtils::getAlignBaseInfo3D(alignInfo);
    if (!ret.success)
    {
        return ret;
    }

    EyeReliefBaseInfo eyeReliefInfo = AlignUtils::getEyeReliefBaseInfo();

    Result result = Alignment3D::instance()->alignEyebox(&alignInfo, eyeReliefInfo, alignConfig);
    return result;
}

Result AlignWrapper::startCalibrate2D(QString param)
{
    if (m_calibrateWidget->isAlignWidgetRun())
    {
        return Result(false, "Calibration is running, please stop and start again.");
    }

    AlignConfig alignConfig = AnalyzeRecipe().analyzCalibrateParam(param);
    AlignBaseInfo2D alignInfo;
    Result ret = AlignUtils::getAlignBaseInfo2D(alignInfo);
    if (!ret.success)
    {
        return ret;
    }

    ret = Alignment2D::instance()->alignEyebox(&alignInfo, alignConfig);
    return ret;
}

Result AlignWrapper::moveAlignEyebox(QString param)
{
    bool moveout = false;
    int index = 0;
    Result ret = AnalyzeRecipe().analyzEeyeboxParam(param, index, moveout);
    if (!ret.success)
    {
        return ret;
    }

    return Alignment2D::instance()->moveAlignEyebox(index - 1);
}

Result AlignWrapper::dealDutConfig(QString param)
{
    return AnalyzeRecipe().analyzDutConfigParam(param, CalibrationConfig::instance()->getDutConfigReference());
}

Result AlignWrapper::loadInitPosition(QString param)
{
    return MoveLimitJX::moveInit();
}

Result AlignWrapper::deviceConnect(QString param)
{
    MetricsData::instance()->setIsConnectErrorShow(false);
    return m_calibrateWidget->deviceConnect();
    MetricsData::instance()->setIsConnectErrorShow(true);
}

Result AlignWrapper::calibrateConfig(QString param)
{
    return AnalyzeRecipe().analyzeCalibrateConfig(param, CalibrationConfig::instance()->getCalibrateConfig());
}

Result AlignWrapper::setCamera(QString param)
{
    QHash<QString, QVariant> props;
    props.insert("mv", "vimba");
    MLCamera *pCamera = ExtensionSystem::Internal::ServicesManger::getService<MLCamera>("CORE::MLCamera", props);

    QStringList list = param.split("/");
    if (list.size() < 1)
    {
        return Result(false, QString("Analyze MV camera config param error, param is %1.").arg(param).toStdString());
    }

    bool autoET = false;
    int etTime = 0;
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "autoExposure")
            {
                autoET = val.toInt();
                if (autoET)
                {
                    pCamera->SetMLExposureAuto();
                }
            }
            else if (key == "exposureTime")
            {
                etTime = val.toInt();
            }
        }
    }

    if (!autoET && etTime > 0)
    {
        if (pCamera->IsOpened())
        {
            pCamera->SetExposureTime(etTime *1000);
        }
    }
    return Result();
}

Result AlignWrapper::mvExpose(QString param)
{
    return AnalyzeRecipe().analyzMvExposeParam(param);
}

Result AlignWrapper::createImageDir(QString param)
{
    QString fullPath;
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "RootDir")
            {
                fullPath = val;
                QDir dir(fullPath);
                if (!dir.exists())
                {
                    if (!dir.mkpath(fullPath))
                    {
                        return Result(false, QString("Create align image dir failed, ").arg(fullPath).toStdString());
                    }
                }
            }
        }
    }

    MetricsData::instance()->setAlignImageDir(fullPath);
    return Result();
}

Result AlignWrapper::motion3DConnect(QString param)
{
    return Result();
    //return m_calibrateWidget->motion3DConnect();
}

Result AlignWrapper::setAngle(QString param)
{
    Result ret;
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed().toLower();
            QString val = paramPair[1].trimmed();
            QStringList list = val.split(",");
            if (list.size() < 2) {
                return Result(false, "Param is error.");
            }
            cv::Point2f coord = cv::Point2f(list.at(0).toFloat(), list.at(1).toFloat());

            if (key == QString("projectorAngleXY").toLower())
            {
                ret = judgeAngleAndMoveInitPos(ACS2DPro, coord);
                if (!ret.success)
                {
                    return ret;
                }

                if (m_isStop)
                {
                    return Result(false, "Manual stop.");
                }

                ret = MoveLimitJX::moveAbs2DTilt(coord, ACS2DPro);
                if (!ret.success)
                {
                    return ret;
                }
            }
        }
    }
    return ret;
}

Result AlignWrapper::judgeAngleAndMoveInitPos(motion2DType type, cv::Point2f coordTarget)
{
    cv::Point2f currCoord;
    Result ret = MoveLimitJX::instance()->getCoordinate2DTilt(currCoord, type);
    if (!ret.success)
    {
        return ret;
    }

    if (abs(currCoord.x - coordTarget.x) > 0.1 || abs(currCoord.y - coordTarget.y) > 0.1)
    {
        ret = MoveLimitJX::instance()->moveAbsToSafe(withCamera);
        if (!ret.success)
        {
            return ret;
        }

        ret = MoveLimitJX::instance()->moveDutAbsToSafe();
        if (!ret.success)
        {
            return ret;
        }
    }

    return Result();
}

Result AlignWrapper::pupilDecenter(QString param)
{
    Result ret;
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "relative") {
                QStringList posStr = val.split(",");
                if (posStr.size() < 3)
                {
                    return Result(false, "Move coordinate parameter is error.");
                }

                cv::Point3f coord;
                coord.x = posStr[0].toDouble();
                coord.y = posStr[1].toDouble();
                coord.z = posStr[2].toDouble();
                ret = Alignment2D::instance()->relativeMoveByWafer(coord);
            }
        }
    }

    return ret;
}


#include "MAalignmentWrapper.h"
#include "piMotionActor\ConfigMAInfo.h"
#include "piMotionActor\configItem.h"
#include <piMotionActor/Motion3DModel.h>
#include "MAalignmentLogic.h"
#include <Core/modemanager.h>
#include "AlignUtils.h"
#include "piMotionActor/MoveLimit.h"
#include "piMotionActor/calibrationconfig.h"
#include "Core/loggingwrapper.h"

MAalignmentWrapper::MAalignmentWrapper(QObject *parent, aligmentWidget *alignWidget, SLBWidget* slbWidget)
    : IRecipeWrapper(parent), m_alignWidget(alignWidget), m_slbWidget(slbWidget)
{
}

MAalignmentWrapper::~MAalignmentWrapper()
{
}

void MAalignmentWrapper::Invoke(QString cmd, QString param)
{
    int startTime = 0;
    int takeTime = 0;
    qWarning() << "---- takt time ---- [MAalignment " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
               << "] start.";
    startTime = QDateTime::currentMSecsSinceEpoch();
    Result result;
    if (cmd.toLower() == QString("deviceConnect").toLower())
    {
        result = m_alignWidget->deviceConnectAll();
    }
    else if(cmd.toLower() == QString("calibrate").toLower()){
        Core::ModeManager::activateMode("MAalignment.MAalignmentMode");
        result = startCalibrate(param);
        Core::ModeManager::activateMode("Recipe.RecipeMode");
    }
    else if (cmd == "MtMotion")
    {
        result = MTMotionMove(param);
    }
    else if (cmd == "setAngle")
    {
        result = setAngle(param);
    }
    else if (cmd == "deviceCheck")
    {
        result = slbTips(param);
    }
    else if (cmd == "alignmentSLB")
    {
        result = alignmentSLB(param);
    }
    else
    {
        result = Result(false, QString("\"%1\" cmd is mismatch, please check the execution fields.").arg(cmd).toStdString());
    }

    takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "---- takt time ---- [MAalignment " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
               << "] end. times:" << takeTime << " ms...";

    if (!result.success)
    {
        QString message = QString("Recipe [%1:%2] run error, %3").arg(cmd).arg(param).arg(QString::fromStdString(result.errorMsg));
        LoggingWrapper::instance()->error(message);

        if (result.errorCode == CORE::Wrapper_Warning)
        {
            emit SendStatusSignal(CORE::Wrapper_Warning, QString::fromStdString(result.errorMsg));
        }
        else
        {
            emit SendStatusSignal(CORE::Wrapper_Error_fatal, QString::fromStdString(result.errorMsg));
        }
    }
    else
    {
        emit SendStatusSignal(Wrapper_Done, "MAalignment is done!");
    }
}

void MAalignmentWrapper::notifyStop(bool isstop)
{
    m_isStop = isstop;

    Motion3DModel::getInstance(withCamera)->Motion3DMoveStop();
    Motion3DModel::getInstance(withDUT)->Motion3DMoveStop();
    Motion2DModel::getInstance(ACS2DDUT)->Motion2DMoveStop();
    Motion2DModel::getInstance(ACS2DPro)->Motion2DMoveStop();

    if(m_alignWidget->isAlignWidgetRun()){
        return;
    }
    MAalignmentLogic::instance()->continueRun(!isstop);
}

void MAalignmentWrapper::notifyPause(bool isPause)
{
    if (m_alignWidget->isAlignWidgetRun())
    {
        return;
    }
    MAalignmentLogic::instance()->notifyPause(isPause);
}

Result MAalignmentWrapper::MTMotionMove(QString param)
{
    QStringList list = param.split("/");
    if (list.size() < 1)
    {
        return Result(false, QString("Analyze MV camera config param error, param is %1.").arg(param).toStdString());
    }
    for each (QString paramstr in list)
    {
    }
    return Result();
}

Result MAalignmentWrapper::startCalibrate(QString param)
{
    if (m_alignWidget->isAlignWidgetRun())
    {
        return Result(false, "Input grating calibration is running, please stop and start again.");
    }

    DUT_EYE_TYPE eyeType = (DUT_EYE_TYPE)CalibrationConfig::instance()->getDutConfig().eyeType;
    bool autoMode = false;
    std::string fiducialType = AAlignConfig().fiducialTypeStr;

    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed().toLower();
            QString val = paramPair[1].trimmed();
            if (key == QString("autoMode").toLower())
            {
                autoMode = val.toInt() > 0; 
            }
            else if (key == QString("dutEyeType").toLower())
            {
                eyeType = val.toLower().contains("left") ? DUT_EYE_LEFT : DUT_EYE_RIGHT;
            }
            else if (key == QString("fiducialType").toLower())
            {
                fiducialType = val.toStdString();
            }
        }
    }

    AAlignConfig alignConfig;
    Result ret = AlignUtils::getAlignConfig(alignConfig, eyeType);
    if (!ret.success)
    {
        return ret;
    }
    alignConfig.autoMode = autoMode;
    alignConfig.fiducialTypeStr = fiducialType;

    ret = MAalignmentLogic::instance()->calibrate(alignConfig);
    return ret;
}

Result MAalignmentWrapper::setAngle(QString param)
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
            if (list.size() < 2){
                return Result(false, "Param is error.");
            }
            cv::Point2f coord = cv::Point2f(list.at(0).toFloat(), list.at(1).toFloat());

            if (key == QString("dutAngleXY").toLower())
            {
                ret = judgeAngleAndMoveInitPos(ACS2DDUT, coord);
                if (!ret.success)
                {
                    return ret;
                }

                if (m_isStop)
                {
                    return Result(false, "Manual stop.");
                }

                ret = MoveLimit::moveAbs2DTilt(coord, ACS2DDUT);
                if (!ret.success)
                {
                    return ret;
                }
            }
            else if (key == QString("projectorAngleXY").toLower())
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

                ret = MoveLimit::moveAbs2DTilt(coord, ACS2DPro);
                if (!ret.success)
                {
                    return ret;
                }
            }
        }
    }
    return ret;
}

Result MAalignmentWrapper::judgeAngleAndMoveInitPos(motion2DType type, cv::Point2f coordTarget)
{
    cv::Point2f currCoord;
    Result ret = MoveLimit::instance()->getCoordinate2DTilt(type, currCoord);
    if (!ret.success)
    {
        return ret;
    }

    if (abs(currCoord.x - coordTarget.x) > 0.1 || abs(currCoord.y - coordTarget.y) > 0.1)
    {
        ret = MoveLimit::instance()->moveAbsToSafe(withCamera);
        if (!ret.success)
        {
            return ret;
        }

        ret = MoveLimit::instance()->moveAbsToSafe(withDUT);
        if (!ret.success)
        {
            return ret;
        }
    }

    return Result();
}

Result MAalignmentWrapper::slbTips(QString param)
{
    QString message = "Check whether the projector is rotated to the SLB state.";
    Result ret = Result(false, message.toStdString(), CORE::Wrapper_Warning);
    ret.errorCode = CORE::Wrapper_Warning;
    return ret;
}

Result MAalignmentWrapper::alignmentSLB(QString param)
{
    Result ret = m_slbWidget->alignSLB();
    return ret;
}

#include "motion3dcontrolwrapper.h"
#include "MoveLimit.h"
#include "Core/loggingwrapper.h"

Motion3DControlWrapper::Motion3DControlWrapper(QObject *parent) : IRecipeWrapper(parent)
{
    //setModel(Motion3DModel::getInstance(withCamera));
}
void Motion3DControlWrapper::setModel(Motion3DModel *model)
{
    m_Model = model;
}
void Motion3DControlWrapper::Invoke(QString cmd, QString param)
{
    try{
        Result ret;
        if (cmd.toLower() == QString("DutMove").toLower())
        {
            ret = dutMove(param);
        }
        else if (cmd.toLower() == QString("MeasureCameraMove").toLower())
        {
            ret = measureMove(param);
        }
        else
        {
            ret = Result(false,
                         QString("\"%1\" cmd is mismatch, please check the execution fields.").arg(cmd).toStdString());
        }

        if (!ret.success)
        {
            QString message =
                QString("Recipe [%1:%2] run error, %3").arg(cmd).arg(param).arg(QString::fromStdString(ret.errorMsg));
            LoggingWrapper::instance()->error(message);
            if (ret.errorCode == CORE::Wrapper_Warning)
            {
                emit SendStatusSignal(CORE::Wrapper_Warning, QString::fromStdString(ret.errorMsg));
            }
            else
            {
                emit SendStatusSignal(CORE::Wrapper_Error_fatal, QString::fromStdString(ret.errorMsg));
            }

        }
        else
        {
            emit SendStatusSignal(CORE::Wrapper_Done, QString::fromStdString(ret.errorMsg));
        }
    }
    catch (std::exception e)
    {
        LoggingWrapper::instance()->error("Motion3D recipe step error, " + QString::fromStdString(e.what()));
        emit SendStatusSignal(CORE::Wrapper_Error_fatal, "Motion3D recipe step error, " + QString::fromStdString(e.what()));
    }
}

Motion3DControlWrapper::~Motion3DControlWrapper()
{
}

Result Motion3DControlWrapper::dutMove(QString param)
{
    QStringList parmsLst = param.split("/");
    if (parmsLst.size() < 3){
        return Result(false, "Parameter is error.");
    }

    QString moveType = parmsLst.at(1).trimmed();
    QStringList posStr = parmsLst.at(2).split(",");
    if (posStr.size() < 3){
        return Result(false, "Move coordinate parameter is error.");
    }

    cv::Point3f coord;
    coord.x = posStr[0].toDouble();
    coord.y = posStr[1].toDouble();
    coord.z = posStr[2].toDouble();

    if (moveType.toLower() == "absolute"){
        return MoveLimit::moveAbs(coord, withDUT);
    }
    else if (moveType.toLower() == "relative")
    {
        return MoveLimit::moveRel(coord, withDUT);
    }
    return Result();
}

Result Motion3DControlWrapper::measureMove(QString param)
{
    QStringList parmsLst = param.split("/");
    if (parmsLst.size() < 3)
    {
        return Result(false, "Parameter is error.");
    }

    QString moveType = parmsLst.at(1).trimmed();
    QStringList posStr = parmsLst.at(2).split(",");
    if (posStr.size() < 3)
    {
        return Result(false, "Move coordinate parameter is error.");
    }

    cv::Point3f coord;
    coord.x = posStr[0].toDouble();
    coord.y = posStr[1].toDouble();
    coord.z = posStr[2].toDouble();

    if (moveType.toLower() == "absolute")
    {
        return MoveLimit::moveAbs(coord, withCamera);
    }
    else if (moveType.toLower() == "relative")
    {
        return MoveLimit::moveRel(coord, withCamera);
    }
    return Result();
}



#include "DecoderWrapper.h"
#include "Core/loggingwrapper.h"
#include <QDateTime>
#include "PrjCommon/Session.h"
#include "DecoderModel.h"
#include <QDebug>

DecoderWrapper::DecoderWrapper(QObject *parent)
    : IRecipeWrapper(parent)
{
}

DecoderWrapper::~DecoderWrapper()
{
}

void DecoderWrapper::Invoke(QString cmd, QString param)
{
    int startTime = 0;
    int takeTime = 0;
    qWarning() << "---- takt time ---- [Alignment " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
               << "] start.";
    startTime = QDateTime::currentMSecsSinceEpoch();

    Result result;
    if (cmd == "ReadSN")
    {
        QString sn;
        result = DecoderModel::instance().queryBarcode(sn);
        if(result.success){
            Session::getInstance()->save(Session::getInstance()->getLastKey(), sn);
        }
    }    
    else
    {
        result = Result(false, QString("\"%1\" cmd is mismatch, please check the execution fields.").arg(cmd).toStdString());
    }

    takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "---- takt time ---- [Decoder " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
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
        emit SendStatusSignal(Wrapper_Done, "Decoder is done!");
    }
}

void DecoderWrapper::notifyStop(bool isstop)
{
}

void DecoderWrapper::notifyPause(bool isPause)
{
}
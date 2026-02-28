#include "IntegratingsphereWrapper.h"
#include "ReticleMode.h"
#include "PrjCommon/metricsdata.h"
#include "ReticleMode2D.h"
#include "Core/loggingwrapper.h"

IntegratingsphereWrapper::IntegratingsphereWrapper(QObject *parent)
	: IRecipeWrapper(parent)
{
}

IntegratingsphereWrapper::~IntegratingsphereWrapper()
{
}

void IntegratingsphereWrapper::Invoke(QString cmd, QString param)
{
    int startTime = 0;
    int takeTime = 0;
    qWarning() << "---- takt time ---- [Alignment " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
        << "] start.";
    startTime = QDateTime::currentMSecsSinceEpoch();

    Result result;
    if (cmd == "setSolution")
    {
        result = setSolution(param);
    }
    else if (cmd == "reticleChange")
    {
        result = reticleChange(param);
    }
    else if (cmd == "reticleConnect")
    {
        result = reticleConnect(param);
    }
    else if (cmd == "integratingsphereConnect")
    {
        result = integratingsphereConnect(param);
    }
    else if (cmd == "getlSLumi")
    {
        result = setISPluminance(param);
    }
    else
    {
        result = Result(false, QString("\"%1\" cmd is mismatch, please check the execution fields.").arg(cmd).toStdString());
    }

    takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "---- takt time ---- [Integratingsphere-Reticle " << cmd.toStdString().c_str() << ":" << param.toStdString().c_str()
        << "] end. times:" << takeTime << " ms...";

    if (!result.success)
    {
        QString message = QString("Recipe [%1:%2] run error, %3").arg(cmd).arg(param).arg(QString::fromStdString(result.errorMsg));
        LoggingWrapper::instance()->error(message);
        emit SendStatusSignal(Wrapper_Error_fatal, QString::fromStdString(result.errorMsg));
        m_Status = CORE::Wrapper_Error_fatal;
    }
    else
    {
        emit SendStatusSignal(Wrapper_Done, QString::fromStdString(result.errorMsg));
        m_Status = CORE::Wrapper_Done;
    }
}

void IntegratingsphereWrapper::notifyStop(bool isstop)
{
    if(isstop){
        IntegratingSphereTCPModel::getInstance()->setAllCurrentOutputZero();
    }
}

Result IntegratingsphereWrapper::setSolution(QString param)
{
    //ISSolution solution;
    //Result ret = analyzeSolution(param, solution);
    //if (!ret.success){
    //    return ret;
    //}

    QStringList list = param.split("/");
    if (list.length() < 3)
    {
        return Result(false, "Wrong parameter.");
    }

    QString ISColor;
    QList<QString> ISValues;
    for (QString str : list)
    {
        QStringList paramPair = str.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "name")
            {
                ISColor = val;
            }
            else if (key == "scale")
            {
                ISValues = val.split(",");
            }
        }
    }

    QList<QString> colors;
    colors << "R"<< "G"<< "B"<< "W";
    if (!colors.contains(ISColor.toUpper()))
    {
        if (ISValues.size() == 1 && ISValues.at(0) == "0"){
            QMap<QString, float> ISValueMap;
            ISValueMap["R"] = 0.0;
            return IntegratingSphereTCPModel::getInstance()->setCurrentOutput("R", ISValueMap);
        }else{
            return IntegratingSphereTCPModel::getInstance()->setSolution(ISColor);
        }
    }
    else
    {
        if (ISValues.size() == 0)
        {
            return Result(false, "Labsphere scale is missing.");
        }

        if (ISColor.toUpper() == "W" && ISValues.size() < 3)
        {
            return Result(false, "Labsphere W color scale is incomplete.");
        }

        QMap<QString, float> ISValueMap;
        if (ISColor.toUpper() == "R")
        {
            ISValueMap["R"] = ISValues.at(0).toFloat();
        }
        else if (ISColor.toUpper() == "G")
        {
            ISValueMap["G"] = ISValues.at(0).toFloat();
        }
        else if (ISColor.toUpper() == "B")
        {
            ISValueMap["B"] = ISValues.at(0).toFloat();
        }
        else if (ISColor.toUpper() == "W")
        {
            ISValueMap["R"] = ISValues.at(0).toFloat();
            ISValueMap["G"] = ISValues.at(1).toFloat();
            ISValueMap["B"] = ISValues.at(2).toFloat();
        }

        return IntegratingSphereTCPModel::getInstance()->setCurrentOutput(ISColor, ISValueMap);
    } 
}

Result IntegratingsphereWrapper::reticleChange(QString param)
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
            if (key == "pattern") {
                ret = ReticleMode::instance()->changeReticle(val);
            }
        }
    }

    return ret;
}

Result IntegratingsphereWrapper::reticleConnect(QString param)
{
    Result ret = ReticleMode::instance()->connect();
    return ret;
}

Result IntegratingsphereWrapper::reticleConnect2D(QString param)
{
    int eyeType = -1;
    QStringList list = param.split("/");
    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "Type")
            {
                eyeType = 0;
                if (val.toLower() != "left")
                {
                    eyeType = 1;
                }
            }
        }
    }

    Result ret = ReticleMode2D::instance()->connect();
    if (!ret.success)
    {
        return ret;
    }

    if (eyeType >= 0)
    {
        ReticleMode2D::instance()->setEyeType(eyeType);
    }
    return Result();
}

Result IntegratingsphereWrapper::analyzeSolution(QString param, ISSolution &solution)
{
    QStringList list = param.split("/");
    if (list.length() < 3)
    {
        return Result(false, "Wrong parameter.");
    }

    for(QString str: list){
        QStringList paramPair = str.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "index")
            {
                solution.index = val.toInt();
            }
            else if (key == "name")
            {
                solution.name = val;
            }
            else if (key == "scale")
            {
                solution.scale = val.toDouble();
            }
        }
    }
    return Result();
}

Result IntegratingsphereWrapper::setISPluminance(QString param)
{
    QString color = "";
    double luminance = -1;
    QStringList list = param.split("/");
    for (QString str : list)
    {
        QStringList paramPair = str.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();

            if (key == "color")
            {
                color = val;
            }
            else if (key == "Lumi")
            {
                luminance = val.toDouble();
            }
        }
    }

    MetricsData::instance()->setISPluminanceData(color.toLower(), luminance);
    return Result();
}

Result IntegratingsphereWrapper::integratingsphereConnect(QString param)
{
    return IntegratingSphereTCPModel::getInstance()->Connect();
}
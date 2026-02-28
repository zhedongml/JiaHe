#include "Motion2DControlWrapper.h"
Motion2DControlWrapper::Motion2DControlWrapper(QObject *parent) : IRecipeWrapper(parent)
{
}

Motion2DControlWrapper::~Motion2DControlWrapper()
{
}

void Motion2DControlWrapper::setModel(Motion2DModel *model)
{
    m_Model = model;
}

void Motion2DControlWrapper::Invoke(QString cmd, QString param)
{
    QStringList list = param.split("/");

    QString parameter = list.last();
    if (cmd == "Tilt")
    {
        moveMotionToPos(param);
    }
}

void Motion2DControlWrapper::moveMotionToPos(QString param)
{
    QStringList parmsLst = param.split("/");
    int module;
    if (parmsLst.size() > 3)
    {
        QString module = parmsLst.at(1).trimmed();
        int moduleType = switchModule(module);
        QString moveType = parmsLst.at(2).trimmed();
        QStringList posStr = parmsLst.at(3).split(",");
        double x = posStr[0].toDouble();
        double y = posStr[1].toDouble();
        setModel(Motion2DModel::getInstance(moduleType));
        toMove(moduleType,moveType, x, y);
    }
    else
    {
        m_Status = CORE::Wrapper_Error_fatal;
        //emit SendStatusSignal(m_Status, "error parameter count");
    }
}

void Motion2DControlWrapper::toMove(int moduleType,QString moveType, double x, double y)
{
    if (moveType == "AbsMove")
    {
        Result res=m_Model->getInstance(moduleType)->Motion2DMoveAbs(x , y );
        if (!res.success)
        {
            std::string err = res.errorMsg;
            QString errMsg = QString::fromStdString(err);
            m_Status = CORE::Wrapper_Error_fatal;
            switch (moduleType)
            {
            case 0:
                errMsg = "With DUT tilt " + errMsg;
                break;
            case 1:
                errMsg = "With Projector tilt " + errMsg;
                break;
            }
            //QString msg = motion2DType(moduleType);
            trigerSignal(m_Status, errMsg);
        }

    }
    else if (moveType == "RelMove")
    {
        Result res = m_Model->getInstance(moduleType)->Motion2DMoveRel(x , y );
        if (!res.success)
        {
            std::string err =res.errorMsg;
            QString errMsg = QString::fromStdString(err);
            m_Status = CORE::Wrapper_Error_fatal;
            switch (moduleType)
            {
            case 0:
                errMsg = "With DUT tilt " + errMsg;
            case 1:
                errMsg = "With Projector tilt " + errMsg;
            }
            //QString msg = motion2DType(moduleType);
            trigerSignal(m_Status, /*msg+*/errMsg);
        }
    }
}

int Motion2DControlWrapper::switchModule(QString module)
{
    if (module == "ACS2DPro")
        return 1;
    else
        return 0;
}

void Motion2DControlWrapper::trigerSignal(CORE::WrapperStatus status, QString msg)
{
    emit SendStatusSignal(status, msg);
}

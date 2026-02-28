#pragma once

#include <QObject>
#include "Motion2DModel.h"
#include "PrjCommon/irecipewrapper.h"

class Motion2DControlWrapper : public IRecipeWrapper
{
	Q_OBJECT

public:
	Motion2DControlWrapper(QObject *parent);
	~Motion2DControlWrapper();
    void setModel(Motion2DModel *model);
    void Invoke(QString cmd, QString param);
    void moveMotionToPos(QString param);
    void toMove(int moduleType, QString moveType, double x, double y);
    int switchModule(QString module);
    void trigerSignal(CORE::WrapperStatus status, QString msg);

private:
    Motion2DModel *m_Model;
    CORE::WrapperStatus m_Status;
};

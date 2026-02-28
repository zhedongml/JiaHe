#pragma once

#include <QObject>
#include "PrjCommon/irecipewrapper.h"
#include "Motion3DModel.h"
#include "Result.h"

class Motion3DControlWrapper : public IRecipeWrapper
{
	Q_OBJECT

public:
	Motion3DControlWrapper(QObject *parent);
	~Motion3DControlWrapper();

public:
    void setModel(Motion3DModel* model);
    void Invoke(QString cmd, QString param);

private:
    Result dutMove(QString param);
    Result measureMove(QString param);

private:
    Motion3DModel* m_Model;
  CORE::WrapperStatus m_Status;
};

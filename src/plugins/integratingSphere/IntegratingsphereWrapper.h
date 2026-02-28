#pragma once
#include "PrjCommon/irecipewrapper.h"
#include "PrjCommon/service/ml_motion.h"
#include "integratingspheretcpmodel.h"

class IntegratingsphereWrapper : public IRecipeWrapper
{
	Q_OBJECT

public:
	IntegratingsphereWrapper(QObject *parent);
	~IntegratingsphereWrapper();
    void Invoke(QString cmd, QString param) override;
    void notifyStop(bool isstop) override;

private:
    Result setSolution(QString param);
	Result reticleChange(QString param);
    Result reticleConnect(QString param);
	Result reticleConnect2D(QString param);
    Result setISPluminance(QString param);
	Result integratingsphereConnect(QString param);

	Result analyzeSolution(QString param, ISSolution &solution);
private:
	CORE::WrapperStatus m_Status;
};

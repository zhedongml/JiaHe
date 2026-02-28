#pragma once
#include "Core/IAction.h"
#include "dutPrCalibrateWidget.h"

using namespace Core;

class InitCalibrateCfgAction : public IAction
{
	Q_OBJECT
public:
	InitCalibrateCfgAction(QString id, QString parentId, QObject* parent = nullptr);
	~InitCalibrateCfgAction();

private slots:
	void triggeredSlot();

private:
	dutPrCalibrateWidget* m_calibrateWidget = nullptr;
};



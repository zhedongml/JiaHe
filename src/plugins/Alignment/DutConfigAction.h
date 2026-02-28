#pragma once
#include "Core/IAction.h"
#include "FiducialConfigWidget.h"

using namespace Core;

class DutConfigAction: public IAction
{
	Q_OBJECT
public:
	DutConfigAction(QString id, QString parentId, QObject *parent = nullptr);
	~DutConfigAction();

private slots:
	void triggeredSlot();

private:
	FiducialConfigWidget* m_dutConfigWidget = nullptr;
};


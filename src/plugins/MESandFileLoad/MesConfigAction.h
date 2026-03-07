#pragma once
#include "Core/IAction.h"
#include "QtWidgetsClass.h"

using namespace Core;

class MesConfigAction : public IAction
{
	Q_OBJECT
public:
	MesConfigAction(QString id, QObject* parent = nullptr);
	~MesConfigAction();

private slots:
	void triggeredSlot();
	void updateUserLevel();

private:
	QtWidgetsClass* m_widget = nullptr;

};


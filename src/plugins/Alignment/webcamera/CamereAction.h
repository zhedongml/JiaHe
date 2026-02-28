#pragma once
#include "Core/IAction.h"
#include "qtcamera.h"

using namespace Core;

class CamereAction: public IAction
{
	Q_OBJECT
public:
	CamereAction(QString id, QString parentId, QObject *parent = nullptr);
	~CamereAction();

private slots:
	void triggeredSlot();

private:
	Camera* m_camera = nullptr;
};


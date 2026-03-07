#pragma once

#include "core_global.h"
#include <QObject>
#include <QAction>

namespace Core {

	class CORE_EXPORT IAction : public QAction
	{
		Q_OBJECT

	public:
		IAction(QString id, QString parentId, QObject* parent = nullptr);
		IAction(QString id, QString parentId, QString groupId, QObject* parent = nullptr);
		~IAction();
	};
}

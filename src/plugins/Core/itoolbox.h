#pragma once
#include "core_global.h"
#include <QObject>
#include <QWidget>
namespace Core {

	class CORE_EXPORT IToolBox : public QWidget
	{
		Q_OBJECT

	public:
		IToolBox(QString objName="",QObject * parent = nullptr);
		~IToolBox();
	};
}
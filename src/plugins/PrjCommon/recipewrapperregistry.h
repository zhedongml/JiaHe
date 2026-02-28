#pragma once

#include <QObject>
#include "prjcommon_global.h"
#include "irecipewrapper.h"
#include <QMap>
class PRJCOMMON_EXPORT RecipeWrapperRegistry : public QObject
{
	Q_OBJECT

public:
	~RecipeWrapperRegistry();
	static RecipeWrapperRegistry* Instance();
	void regist(QString wrapperName,IRecipeWrapper* wrapper);
	void registoryResultWidget(QString name,QWidget* resultWidget);
	IRecipeWrapper* getWrapper(QString wrapperName);
	void notifyStopStatus( bool isstop);
	void notifyPauseStatus(bool issPause);
	QMap<QString, QWidget*> getresultWidgets();
private :
	RecipeWrapperRegistry();
	static RecipeWrapperRegistry* self;
	int testval = -1;
	QMap<QString, IRecipeWrapper*> repository;
	QMap<QString, QWidget*> resultWidgetRepository;
};

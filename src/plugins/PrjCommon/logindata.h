#pragma once
#include <QObject>
#include "prjcommon_global.h"
#include <QList>

enum USERLEVEL {
	OPERATOR = 0,
	ENGINEER = 1,
	OTHER = 3
};
class PRJCOMMON_EXPORT LoginData : public QObject
{
	Q_OBJECT
public:
	~LoginData();
	static LoginData* instance();
	static LoginData* self;
	USERLEVEL judgeIsOperator(QString userName);
	USERLEVEL getUserLevel();
	QString getRecipeFileContent();
    QString getRecipeFileName();
    void setUserName(QString name);
    QString getUserName();
    QString getUserLevelName();

  private:
	 LoginData();
     USERLEVEL useLevel = ENGINEER;
     QString username;

};
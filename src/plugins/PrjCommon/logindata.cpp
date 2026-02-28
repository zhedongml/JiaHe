#include "logindata.h"
#include "configdatabase.h"

LoginData *LoginData::self = nullptr;
LoginData::LoginData()
	: QObject()
{
}

LoginData::~LoginData()
{
	self = nullptr;
}
LoginData *LoginData::instance()
{
	if (self == nullptr)
	{
		self = new LoginData();
	}
	return self;
}

USERLEVEL LoginData::judgeIsOperator(QString userName)
{
    //TODO:
    return USERLEVEL::ENGINEER;

	if (userName == "0")
	{
		this->useLevel = USERLEVEL::OPERATOR;
		return USERLEVEL::OPERATOR;
	}

	else if (userName == "1")
	{
		this->useLevel = USERLEVEL::ENGINEER;
		return USERLEVEL::ENGINEER;
	}else{
        this->useLevel = USERLEVEL::ENGINEER;
        return USERLEVEL::ENGINEER;
	}
}

USERLEVEL LoginData::getUserLevel()
{
    //TODO:
    return ENGINEER;

	return useLevel;
}

QString LoginData::getRecipeFileContent()
{
	return ConfigDataBase::getInstance()->readRecipeFile();
}
QString LoginData::getRecipeFileName()
{
    return ConfigDataBase::getInstance()->getRecipeFileName();
}

void LoginData::setUserName(QString name)
{
    username = name;
}

QString LoginData::getUserName()
{
    return username;
}

QString LoginData::getUserLevelName()
{
    // TODO:
    return "Engineer";

    QString name = "Operator";
    switch (useLevel)
    {
    case OPERATOR:
        name = "Operator";
        break;
    case ENGINEER:
        name = "Engineer";
        break;
    case OTHER:
        name = "Operator";
        break;
    default:
        break;
    }

    return name;
}

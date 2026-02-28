#include "Session.h"

 Session* Session::s_Instance = nullptr;

Session* Session::getInstance()
{
	return s_Instance = (s_Instance != nullptr) ? s_Instance : new Session();
}

void Session::save(const QString& key, const QString& value)
{
	m_CustomVariables[key] = value;
}

void Session::loadReservedWords()
{
	m_ReservedWords["$datetime$"] = &Session::getDateTime;
	m_ReservedWords["$date$"] = &Session::getDate;
}

QMap<QString, QString> Session::getData()
{
	return m_CustomVariables;
}

 QString Session::get(const QString& key)
{
	if (m_ReservedWords.contains(key))
	{
	    return (this->*m_ReservedWords[key])();
	}
	else if (m_CustomVariables.contains(key))
	{
		return m_CustomVariables[key];
	}
	else
	{
		return "";
	}
}

inline  QString Session::getDate()
{ 
	QDate current_date = QDate::currentDate();
	QString currentDate = current_date.toString("yyyyMMdd");
	return currentDate;
	
}
inline  QString Session::getDateTime() {
	return QDateTime().currentDateTime().toString("yyyyMMddTHHmmss");
}

Session::~Session()
{
	if (s_Instance != nullptr)
	{
		delete s_Instance;
	}
}

void Session::setLastKey(const QString& key)
{
	m_lastKey = key;
}

QString Session::getLastKey()
{
	return m_lastKey;
}


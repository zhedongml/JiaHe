#pragma once
#include "core_global.h"
#include <QObject>
#include "outputwindow.h"
#include <QMutex>

class CORE_EXPORT LoggingWrapper
{
public:
	static LoggingWrapper* instance();
	~LoggingWrapper();
    void setAppOutPutPane(Core::OutputWindow *pane);
	void info(QString message, bool isDisplay = true);
	void debug(QString message, bool isDisplay = true);
	void warn(QString message, bool isDisplay = true);
	void error(QString message, bool isDisplay = true);
private:
	LoggingWrapper();
	Core::OutputWindow*outputpane = nullptr;
	static LoggingWrapper* self;
	static QMutex m_mutex;
};

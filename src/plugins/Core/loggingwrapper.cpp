#include "loggingwrapper.h"
#include <QDebug>
#include "LogPlus.h"

LoggingWrapper* LoggingWrapper::self = nullptr;
QMutex LoggingWrapper::m_mutex;

LoggingWrapper* LoggingWrapper::instance()
{
    if (!self) {
        QMutexLocker locker(&m_mutex);
        if (!self) {
            self = new LoggingWrapper;
          
        }
    }
    return self;
}
LoggingWrapper::LoggingWrapper()
{
 
}

LoggingWrapper::~LoggingWrapper()
{
}

void LoggingWrapper::setAppOutPutPane(Core::OutputWindow* pane)
{
	outputpane = pane;
}

void LoggingWrapper::info(QString message, bool isDisplay)
{
    if (isDisplay)
    {
        if (outputpane != nullptr) {
            outputpane->appendText(message, 2);
        }
    }
    std::string cmessage = message.toStdString();
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, cmessage.c_str());
    //qWarning() << "[INFO]" << message;
}

void LoggingWrapper::debug(QString message, bool isDisplay)
{
    if (isDisplay)
    {
        if (outputpane != nullptr) {
            outputpane->appendText(message, 0);
        }
    }
    std::string cmessage = message.toStdString();
    LOG4CPLUS_DEBUG(LogPlus::getInstance()->logger, cmessage.c_str());
    //qWarning() << "[DEBUG]" << message;
}

void LoggingWrapper::warn(QString message, bool isDisplay)
{
    if (isDisplay)
    {
        if (outputpane != nullptr) {
            outputpane->appendText(message, 3);
        }
    }
    std::string cmessage = message.toStdString();
    LOG4CPLUS_WARN(LogPlus::getInstance()->logger, cmessage.c_str());
    //qWarning() << "[WARN]" << message;
}

void LoggingWrapper::error(QString message, bool isDisplay)
{
    if (isDisplay)
    {
        if (outputpane != nullptr) {
            outputpane->appendText(message, 1);
        }
    }
    std::string cmessage = message.toStdString();
    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, cmessage.c_str());
    //qWarning() << "[ERROR]" << message;
}

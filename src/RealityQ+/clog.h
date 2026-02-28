#pragma once
#include <QDateTime> 
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMutex>
#include <windows.h>

#define _TIME_ qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))

// Print log
static struct LogWrap {
    LogWrap::LogWrap() { reset(); }
    LogWrap::~LogWrap() { reset(); }
    void reset() {
        isInit = false;
        level = "";
        date = "";
        if (ts) {
            delete ts;
            ts = nullptr;
        }
        if (file) {
            delete file;
            file = nullptr;
        }
    }
    bool isInit;
    QMutex locker;
    QString level;
    QString date;
    QFile* file;
    QTextStream* ts;
} s_logWrap;

class CLog
{
public:
    static void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
    {
        QString formatMsg;
        switch (type) {
        case QtDebugMsg:
            formatMsg = "DEBUG";
            break;
        case QtWarningMsg:
            formatMsg = "WARNING";
            break;
        case QtCriticalMsg:
            formatMsg = "CRITICAL";
            break;
        case QtFatalMsg:
            formatMsg = "FATAL";
            break;
        default:
            return;
        }

        QString curDate = QDate::currentDate().toString("yyyyMMdd");
        if (curDate != s_logWrap.date) {
            s_logWrap.reset();
        }

        if (!s_logWrap.isInit) {
            s_logWrap.isInit = true;
            QDir().mkpath("logFile");
            s_logWrap.level = "debug";
            s_logWrap.date = QDate::currentDate().toString("yyyyMMdd");
            s_logWrap.file = new QFile(QString("logFile\\log%1.log").arg(s_logWrap.date));
            if (s_logWrap.file->open(QIODevice::WriteOnly | QIODevice::Append)) {
                s_logWrap.ts = new QTextStream(s_logWrap.file);
            }
        }

        if (s_logWrap.ts) {
            QString text = _TIME_ + QString(" %1 [%2:%3:%4] %5\n")
                .arg(formatMsg)
                .arg(QLatin1String(context.file))
                .arg(QLatin1String(context.function))
                .arg(context.line)
                .arg(msg);
            std::wstring wstr = QString::fromUtf8(text.toUtf8()).toStdWString();
            OutputDebugStringW(wstr.c_str());
            QMutexLocker locker(&s_logWrap.locker);
            (*s_logWrap.ts << text).flush();
        }
    }
};
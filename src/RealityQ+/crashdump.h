#pragma once
#include <QApplication>
#include <QString>
#include <QDir>
#include <QDateTime>

#if defined(Q_OS_WIN32)   
#include <windows.h>
#endif

class CrashDump
{
public:
#if defined(Q_OS_WIN32) // 		#if defined(_MSC_VER) && _MSC_VER >= 1400
	static LONG WINAPI  crashStackCallback(struct _EXCEPTION_POINTERS* exceptionInfo);
#endif

};


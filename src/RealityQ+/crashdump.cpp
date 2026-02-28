#include "crashdump.h"
#if defined(Q_OS_WIN32)
#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")

LONG WINAPI CrashDump::crashStackCallback(struct _EXCEPTION_POINTERS* exceptionInfo) {
	QString savePath = QCoreApplication::applicationDirPath() + "/dump/";
	QDir dir(savePath);
	if (!dir.exists() && !dir.mkpath(savePath)) {
		//app->exit(E_UNEXPECTED);
		return EXCEPTION_EXECUTE_HANDLER;
	}
	savePath.append(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"));
	savePath.append(".dmp");

	HANDLE dump = CreateFileW(savePath.toStdWString().c_str(), GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == dump) {
		//app->exit(E_UNEXPECTED);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	MINIDUMP_EXCEPTION_INFORMATION miniDumpExceptionInfo;
	miniDumpExceptionInfo.ExceptionPointers = exceptionInfo;
	miniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
	miniDumpExceptionInfo.ClientPointers = TRUE;
	DWORD idProcess = GetCurrentProcessId();
	MiniDumpWriteDump(GetCurrentProcess(), idProcess, dump,
		MiniDumpWithFullMemory, &miniDumpExceptionInfo, NULL, NULL);

	CloseHandle(dump);

	return EXCEPTION_EXECUTE_HANDLER;
}
#endif
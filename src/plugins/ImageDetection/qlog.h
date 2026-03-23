
#ifndef QLOG_H      //条件编译，只被编译一次

#define QLOG_H


#include <stdio.h>

#include <stdarg.h>

#ifdef __cplusplus  //C/C++混合编程

extern "C" {         //声明全局变量

#endif

	class QLog
	{
	public:
		
		~QLog();

		QLog(const QLog&) = delete;

		QLog& operator=(const QLog& log) = delete;

		static QLog& GetInstance();

		int log_init(const char*);

		void log_exit();

		int qlog(const char* msg, ...);

	private:
		QLog();

		FILE* logfile = 0;

		char buff[4096];
	};







#ifdef __cplusplus

}

#endif





#endif // QLOG_H

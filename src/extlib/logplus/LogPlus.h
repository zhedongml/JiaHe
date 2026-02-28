#pragma once
#include <iostream>
#include <string>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h> 
#include <log4cplus/layout.h> 
#include <log4cplus/loggingmacros.h> 
#include <log4cplus/helpers/stringhelper.h> 
#if defined(CORE_LIBRARY)
#  define CORE_EXPORT __declspec(dllexport)
#elif defined(CORE_STATIC_LIBRARY) // Abuse single files for manual tests
#  define CORE_EXPORT
#else
#  define CORE_EXPORT __declspec(dllimport)
#endif
#define MY_LOG_FILE_PATH "config\\logconfig.properites"

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

class CORE_EXPORT LogPlus
{
public:
	static LogPlus * getInstance(const char *strConfigFile = MY_LOG_FILE_PATH);
    log4cplus::Logger logger;
private:
	explicit LogPlus(const char *strConfigFile);
	~LogPlus();
	LogPlus(const LogPlus &) = delete;
	LogPlus &operator=(const LogPlus &) = delete;

};


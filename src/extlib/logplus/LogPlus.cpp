
#include "LogPlus.h"

//#ifdef _DEBUG
//#ifdef _M_X64
//#pragma comment(lib,"..\\..\\extlib\\logplus\\lib\\x64\\bin.Debug_Unicode\\log4cplusUD.lib")
//#else
//#pragma comment(lib,"..\\..\\extlib\\logplus\\lib\\win32\\bin.Debug_Unicode\\log4cplusUD.lib")
//#endif
//#else
//#ifdef _M_X64
//#pragma comment(lib,"..\\..\\extlib\\logplus\\lib\\x64\\bin.Release_Unicode\\log4cplusU.lib")
//#else
//#pragma comment(lib,"..\\extlib\\logplus\\lib\\win32\\bin.Release_Unicode\\log4cplusU.lib")
//#endif
//#endif  //_DEBUG



LogPlus * LogPlus::getInstance(const char *strConfigFile)
{
	static LogPlus log(strConfigFile);
	return &log;
}

LogPlus::LogPlus(const char *strConfigFile)
{
	log4cplus::initialize();
	PropertyConfigurator::doConfigure(LOG4CPLUS_C_STR_TO_TSTRING(strConfigFile));
	logger = Logger::getRoot();
}


LogPlus::~LogPlus()
{
}

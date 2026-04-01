#pragma once
#include <string>

#ifdef FILEUTILS_DLL_EXPORTS
#define FILEUTILS_API __declspec(dllexport)   
#else
#define FILEUTILS_API __declspec(dllimport)   // 外部使用 DLL 时导入
#endif

FILEUTILS_API void createDirIfNotExists(const std::string & path); 

#ifndef CORE_CONFIGFILE_H_
#define CORE_CONFIGFILE_H_

#pragma once
#include <fstream>
#include <iostream>
#include "../prjcommon_global.h"

using namespace std;

#define MAX_LINE_LENGTH 300

namespace CORE
{
class PRJCOMMON_EXPORT CConfigFile
{
public:         
    CConfigFile();
    CConfigFile(const char* config_file);
    ~CConfigFile();


private:      
    fstream file_stream_;
    short         is_open_;    
    enum
    {
        kIsComment,      
        kIsSession,      
        kIsValue,           
        kNotFound           
    };

private:      
      
    short GetSession(const char* str,const char* session_name);

    short GetContent(const char* str,const char* value_name,std::string& content);


public:         
      
    short    Open(const char* config_file);
    
    void     Close();
    
    short    GetValue(const char* session,const char* value_name, std::string& content);
    
    short    IsOpen() { return is_open_; };

};
}

#endif //CORE_CONFIGFILE_H_
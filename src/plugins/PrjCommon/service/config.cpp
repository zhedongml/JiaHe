#include "Config.h"
#include <string.h>;

namespace CORE
{
CConfigFile::CConfigFile()
{
    is_open_ = -1;
}

CConfigFile::CConfigFile(const char* config_file)
{
    is_open_ = -1;
    Open(config_file);
}

CConfigFile::~CConfigFile()
{
    if (is_open_ == 0)
        file_stream_.close();
}

//return: 0 - open file successfully; -1 - open file failed
short    CConfigFile::Open(const char* config_file)
{

    if (is_open_ == 0)
    {
        file_stream_.close();
        is_open_ == -1;
    }
    
    file_stream_.open(config_file, ios::in);
    if (!file_stream_)
        return -1;

    is_open_ = 0;
    return 0;
}

void CConfigFile::Close()
{
    if (is_open_ == 0)
        file_stream_.close();
}

//----------------------------------------------------------------------------
// function£º short    CConfigFile::GetValue(const char* session,const char* value_name, std::string& content)
// feature£º get the value of the specified variable from the configuration file
// parameters£º const char *session     The name of the section in which the variable resides
//              const char *value_name      variable name
//              string& content   The return reference to the value of the variable
// return£º 0   get the value
//          -1  did not get the value
//----------------------------------------------------------------------------
short    CConfigFile::GetValue(const char* session,const char* value_name, std::string& content)
{
    char                tmp_str[MAX_LINE_LENGTH];
    short               flag;
    short               ret;

    file_stream_.seekg(0, ios::beg);
    //find the session
    flag = -1;
    while (!file_stream_.eof())
    {
        file_stream_.getline(tmp_str, MAX_LINE_LENGTH);
        if (GetSession(tmp_str, session) == 0)
        {
            flag = 0;
            break;
        }
    }

    if (flag == -1)
        return -1;

    //get the value
    flag = -1;
    while (!file_stream_.eof())
    {
        file_stream_.getline(tmp_str, MAX_LINE_LENGTH);
        ret = GetContent(tmp_str, value_name, content);
        if ((ret == kIsComment) || (ret == kNotFound))
            continue;
        else if (ret == kIsSession)
            break;
        flag = 0;
        break;
    }

    return flag;
}

//----------------------------------------------------------------------------
// function£º short CConfigFile::GetSession(const char* str,const char* session_name)
// feature£º  get the name of a section (the string in []) in the specified string
// parameters£º const char *str Specified string
//              const char *session_name the specified section name
// return£º 0    find the specified section name
//          -1   the specified section name was not found
//----------------------------------------------------------------------------
short CConfigFile::GetSession(const char* str,const char* session_name)
{
    char tmp_str[50];
    int i = 0;
    int j = 0;

    while (str[i] == ' ') i++;   // skip spaces

    if (str[i] != '[')
        return -1;    // it is not a session

    i++; // skip '['

    while (str[i] == ' ') i++; //skip spaces

  //get the session
    while (str[i] != ' ' && str[i] != ']')
    {
        tmp_str[j] = str[i];
        i++;
        j++;
    }
    tmp_str[j] = 0;

    if (strcmp(tmp_str, session_name) != 0)
        return -1; // Is not the specified section name

    return 0;
}

//----------------------------------------------------------------------------
// function£º short CConfigFile::GetContent(const char* str,const char* value_name,std::string& content)
// feature£º  get the value of the variable from the specified string
// parameters£º const char* str         specified string
//              const char* value_name  variable name
//              std::string& content    The return reference to the value of the variable
// return£º IS_VALUE    get the value of variable
//          IS_COMMENT  is a comment
//          IS_SESSION  is a section
//          NOT_FOUND   the specified variable value was not found
//----------------------------------------------------------------------------
short CConfigFile::GetContent(const char* str,const char* value_name,std::string& content)
{
    char tmp_value_name[50];
    char tem_str[50];
    int i = 0;
    int j = 0;

    while (str[i] == ' ') i++; // skip spaces

    if (str[i] == '#') return kIsComment;    // Is a comment
    if (str[i] == '[') return kIsSession;  // Is a section
    if (str[i] == 0) return kNotFound;     // End of line not found

      // get the value name
    while (str[i] != ' ' && str[i] != '\t' && str[i] != '=' && str[i] != 0)
    {
        tmp_value_name[j] = str[i];
        i++;
        j++;
    }
    tmp_value_name[j] = 0;

    if (strcmp(tmp_value_name, value_name) != 0)
        return kNotFound;    // Is not a specified variable

    while (str[i] == ' ' || str[i] == '\t' || str[i] == '=') i++; //skip spaces and '='

  // get the variable value
    j = 0;
    while (str[i] > ' ')
    {
        tem_str[j] = str[i];
        i++;
        j++;
    }
    tem_str[j] = 0;
    content = tem_str;

    return kIsValue;
}
}
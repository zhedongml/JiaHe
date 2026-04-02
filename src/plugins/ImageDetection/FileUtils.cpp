#include "FileUtils.h"
#include <direct.h>
#include <io.h>

void createDirIfNotExists(const std::string& path)
{
    if (_access(path.c_str(), 0) == 0)
        return;

    std::string current;

    for (size_t i = 0; i < path.size(); ++i)
    {
        current += path[i];

        if (path[i] == '/' || path[i] == '\\')
        {
            if (_access(current.c_str(), 0) != 0)
            {
                (void)_mkdir(current.c_str());
            }
        }
    }

    if (_access(current.c_str(), 0) != 0)
    {
        (void)_mkdir(current.c_str());
    }
}
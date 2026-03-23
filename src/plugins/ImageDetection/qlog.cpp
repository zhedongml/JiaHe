#include "qlog.h"




QLog::QLog()
{
}

QLog::~QLog()
{
}



QLog& QLog::GetInstance()
{
    static QLog instance;
    return instance;
}

int QLog::log_init(const char* name)

{
    if (logfile)
        return 1;

    errno_t err = fopen_s(&logfile, name, "w");

    if (!logfile)

        return -1;

    return 0;

}



int QLog::qlog(const char* msg, ...)

{
    if (!logfile)

        return -1;

    va_list ap;



    va_start(ap, msg);

    vsnprintf(buff, 4096, msg, ap);

    va_end(ap);



    fprintf(logfile, "%s", buff);

    fflush(logfile);

    return 0;

}



void QLog::log_exit()

{

    fclose(logfile);

}



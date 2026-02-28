// Copyright 2023 MLOptic
#pragma once
#include <iostream>
#include <exception>
#include <string>
#include <list>
// #include "ml_addinmanger_global.h"

#ifdef EXPORTING_ADDINMANGER
#define ADDINMANGER_API __declspec(dllexport)
#else
#define ADDINMANGER_API __declspec(dllimport)
#endif

typedef enum exceptionType {
    none,
    timeout,
    deviceAlarm,
    error,
    serialPortAbnormality,
    unknownException,
    uninitialized,
    axisError
} exceptionType;

typedef enum routineType {
    unknown,
    isOk_,
    isError,
    isConnecting,
    notConnected,
    isDisconnected,
    isMoving,
    isStationary,
} routineType;

class pluginException : public std::exception {
 public:
    /*
     *  exception handling
     */
    ADDINMANGER_API pluginException(exceptionType exceptionType,
                                    const char* msg = "");
    exceptionType getExceptionType();
    std::string getExceptionMsg();

    /*
     *  Routine handling
     */
    ADDINMANGER_API pluginException();
    void setRoutineType(routineType routineType);
    void setRoutineMsg(std::string msg);
    void setStatusFlag(bool statusFlag);
    routineType getRoutineType();
    std::list<std::string> getRoutineMsg();
    bool getStatusFlag();

 private:
    std::string getExceptionByType(exceptionType exceptionType,
                                   std::string msg);

 private:
    exceptionType m_exceptionType = exceptionType::none;
    std::string m_exceptionMsg = "";

    routineType m_routineType = routineType::unknown;
    std::list<std::string> m_routineMsgList;
    bool m_statusFlag = false;
};

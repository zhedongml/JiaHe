#pragma once
#include <string>

struct Result
{
    bool success;
    std::string errorMsg;
    int errorCode; // 0 (running error); 1 (manual stop)

  public:
    Result()
    {
        success = true;
        errorMsg = "";
        errorCode = 0;
    }
    Result(bool success, std::string errorMsg, int errorCode = 0)
    {
        this->success = success;
        this->errorMsg = errorMsg;
        this->errorCode = errorCode;
    }

    ~Result()
    {
    }
};
#pragma once
#include <QSharedPointer>
#include <memory>
#include "Result.h"

namespace PrjCommon {
    class TaskCallback
    {
    public:
        virtual void notifyResult(Result ret) = 0;
    };

    class TaskBase {
    public:
        TaskBase(int maxRetry = 0) : m_retryCount(0), m_maxRetry(maxRetry) {}
        virtual ~TaskBase() {}

        virtual Result execute() = 0;
        virtual QString taskInfo() { return QString(); }
        virtual void setStop(std::shared_ptr<std::atomic<bool>> stopFlag){ m_stopFlag = stopFlag;};

        int retryCount() const { return m_retryCount; }
        bool shouldRetry() const { return m_retryCount < m_maxRetry; }
        void incrementRetry() { ++m_retryCount; }

        virtual void onSuccess() {} 
        virtual void onFailure() {}

    protected:
        int m_retryCount;
        int m_maxRetry;
        std::shared_ptr<std::atomic<bool>> m_stopFlag;
    };

    struct PriorityTask {
        int priority = 0;
        QSharedPointer<TaskBase> task;

        // 用于 std::priority_queue 的比较：越大越优先
        bool operator<(const PriorityTask& other) const {
            return priority < other.priority;  // 大顶堆：priority 越大越优先
        }
    };
}
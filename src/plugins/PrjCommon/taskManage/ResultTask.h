#pragma once
#include "TaskBase.h"

namespace PrjCommon {
    template<typename T>
    class ResultTask : public TaskBase {
    public:
        ResultTask(std::function<T()> func, int maxRetry = 0)
            : TaskBase(maxRetry), m_func(func) {}

        Result execute() override {
            try {
                T result = m_func();
                m_promise.set_value(result);
                return Result();
            }
            catch (...) {
                if (!shouldRetry())
                    m_promise.set_exception(std::current_exception());
                return Result(false, "ResultTask exception.");
            }
        }

        std::future<T> future() {
            return m_promise.get_future();
        }

    private:
        std::function<T()> m_func;
        std::promise<T> m_promise;
    };
}


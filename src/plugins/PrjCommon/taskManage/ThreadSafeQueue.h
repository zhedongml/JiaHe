#pragma once
#include <QSharedPointer>
#include <QMutexLocker>
#include <QMutex>
#include <queue> 
#include <QWaitCondition>
#include "Result.h"

namespace PrjCommon {

    template <typename T>
    class ThreadSafeQueue {
    public:
        Result enqueue(const T& value) {
            QMutexLocker locker(&m_mutex);
            if (m_stopped) return Result(false, "Queue enqueue is stop.");

            if (m_maxSize > 0 && m_queue.size() >= m_maxSize)
                m_condition.wait(&m_mutex);

            if (m_stopped) return Result(false, "Queue enqueue is stop.");

            m_queue.push(value);
            m_condition.wakeOne();
            ++m_waitCount;
            return Result();
        }

        Result dequeue(T& task) 
        {
            QMutexLocker locker(&m_mutex);
            while (m_queue.empty() && !m_stopped) {
                m_condition.wait(&m_mutex);
            }

            if (m_stopped) return Result(false, "Queue dequeue is stop.");

            task = m_queue.front();
            m_queue.pop();
            m_condition.wakeAll();
            return Result();
        }

        void setMaxSize(int size) {
            QMutexLocker locker(&m_mutex);
            m_maxSize = size;
        }

        void start() {
            QMutexLocker locker(&m_mutex);
            m_stopped = false;
            m_waitCount = 0;
        }

        void stop() {
            QMutexLocker locker(&m_mutex);
            m_stopped = true;
            m_condition.wakeAll();
        }

        bool isStopped() const {
            QMutexLocker locker(&m_mutex);
            return m_stopped;
        }

        int waitCount(){
            QMutexLocker locker(&m_mutex);
            return m_waitCount;
        }

        int finishCount() {
            QMutexLocker locker(&m_mutex);
            return m_finishCount;
        }

        void taskEnd(Result ret) {
            QMutexLocker locker(&m_mutex);
            if (!ret.success) {
                m_errorList.push_back(ret);
            }
            --m_waitCount;
            ++m_finishCount;
        }

        QList<Result> taskErrorResult()
        {
            QMutexLocker locker(&m_mutex);
            return m_errorList;
        }

        void clearResult()
        {
            QMutexLocker locker(&m_mutex);
            m_waitCount = 0; 
            m_errorList.clear();
            m_finishCount = 0;
        }

    private:
        std::queue<T> m_queue;
        QMutex m_mutex;
        QWaitCondition m_condition;
        int m_maxSize = 0; // 0 = unlimited

        bool m_stopped = false;
        int m_waitCount = 0;
        int m_finishCount = 0;
        QList<Result> m_errorList;
    };
}

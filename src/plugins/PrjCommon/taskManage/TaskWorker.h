#pragma once
#include <QObject>
#include <QThreadPool>
#include <QtConcurrent>
#include "ThreadSafeQueue.h"
#include "TaskBase.h"
#include "TaskRunner.h"
#include <QFutureWatcher>

namespace PrjCommon {
    class TaskWorker
    {
    public:
        TaskWorker(ThreadSafeQueue<QSharedPointer<TaskBase>>* queue, QThreadPool* pool, std::shared_ptr<std::atomic<bool>> stopFlag)
            : m_taskQueue(queue), threadPool(pool), m_running(true), m_stopFlag(stopFlag)
        {
        }

        void start(TaskCallback* callback) {
            m_future = QtConcurrent::run([this, callback]() {
                while (m_running.load()) {
                    QSharedPointer<TaskBase> task;
                    Result ret = m_taskQueue->dequeue(task);
                    if(!m_running.load())break;

                    if (ret.success) {
                        threadPool->start(new TaskRunner(task, m_taskQueue, callback, m_stopFlag));
                    }

                    QThread::msleep(10);
                }
                });
        }

        void stop() {
            m_running.store(false);
            if (m_future.isRunning()) m_future.waitForFinished();
        }

    private:
        ThreadSafeQueue<QSharedPointer<TaskBase>>* m_taskQueue;
        QThreadPool* threadPool;
        std::atomic_bool m_running;
        std::shared_ptr<std::atomic<bool>> m_stopFlag;

        QFuture<void> m_future;
    };
}
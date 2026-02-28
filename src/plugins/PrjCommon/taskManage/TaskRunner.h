#pragma once
#include <QRunnable>
#include <QSharedPointer>
#include "TaskBase.h"
#include "ThreadSafeQueue.h"
#include <QCoreApplication>

namespace PrjCommon {
    class TaskRunner : public QRunnable {
    public:
        TaskRunner(QSharedPointer<TaskBase> taskT,
            ThreadSafeQueue<QSharedPointer<TaskBase>>* q,
            TaskCallback* callback, std::shared_ptr<std::atomic<bool>> stopFlag)
            : m_task(taskT), m_queue(q), m_callback(callback), m_stopFlag(stopFlag)
        {
            setAutoDelete(true);
            m_task->setStop(stopFlag);
        }

        void run() override {
            if (m_stopFlag->load()) {
                if(m_callback){
                    m_callback->notifyResult(Result(false, "Manual stop task, " + m_task->taskInfo().toStdString()));
                }
                return;
            }

            Result ok = m_task->execute();
            if (ok.success) {
                QMetaObject::invokeMethod(QCoreApplication::instance(), [t = m_task]() {
                    t->onSuccess();
                    }, Qt::QueuedConnection);
            }
            else if (m_task->shouldRetry()) {
                m_task->incrementRetry();
                m_queue->enqueue(m_task);
            }
            else {
                QMetaObject::invokeMethod(QCoreApplication::instance(), [t = m_task]() {
                    t->onFailure();
                    }, Qt::QueuedConnection);
            }

            if(m_callback){
                m_callback->notifyResult(ok);
            }
        }

    private:
        QSharedPointer<TaskBase> m_task;
        ThreadSafeQueue<QSharedPointer<TaskBase>>* m_queue;
        std::shared_ptr<std::atomic<bool>> m_stopFlag;
        TaskCallback* m_callback = nullptr;
    };
}
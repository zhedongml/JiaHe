#pragma once
#include <QObject>
#include <QThreadPool>
#include "TaskWorker.h"
#include "ThreadSafeQueue.h"
#include <QSharedPointer>
#include "ResultTask.h"
#include "Result.h"
#include <QMutex>

namespace PrjCommon {
    class TaskManager : public TaskCallback
    {
    public:
        TaskManager(int threadNumber = 0)
        {
            m_threadPool = new QThreadPool();
            if (threadNumber == 0) {
                threadNumber = QThread::idealThreadCount() / 2;
            }
            m_threadPool->setMaxThreadCount(threadNumber);

            m_queue.setMaxSize(100);
            m_stopFlag = std::make_shared<std::atomic<bool>>(false);

            m_worker = new TaskWorker(&m_queue, m_threadPool, m_stopFlag);
            m_worker->start(this);
        }

        ~TaskManager()
        {
            if (m_worker) { delete m_worker; m_worker = nullptr; }
            if (m_threadPool) { delete m_threadPool; m_threadPool = nullptr; }
        }

        Result submit(QSharedPointer<TaskBase> task, int priority = 0)
        {
            if (!m_stopFlag->load()) {
                Result ret = m_queue.enqueue(task);
                return ret;
            }
            else
            {
                return Result(false, "Task manager submit is stop.");
            }
        }

        template<typename T>
        std::future<T> submitWithResult(std::function<T()> func, int priority = 0, int retry = 0) {
            auto task = QSharedPointer<ResultTask<T>>::create(func, retry);
            submit(task, priority);
            return task->future();
        }

        void stopThread()
        {
            m_stopFlag->store(true);
            m_queue.stop();
            if (m_worker) m_worker->stop();
            if (m_threadPool) m_threadPool->waitForDone();
        }

        void stopTask()
        {
            m_stopFlag->store(true);
            m_queue.stop();
            m_threadPool->waitForDone();

            m_stopFlag->store(false);
            m_queue.start();
        }

        void notifyResult(Result ret) override {
            m_queue.taskEnd(ret);
        }

        Result waitEnd(const QString &taskName = "All tasks")
        {
            int waitTime = 0;
            while (m_queue.waitCount()) {
                if (waitTime++ > 100000) {
                    stopTask();
                    QString msg = resultToStr(m_queue.taskErrorResult());
                    m_queue.clearResult();
                    return Result(false, "Task wait is over time, manual stop all tasks." + msg.toStdString());
                }

                QThread::msleep(100);
            }

            bool flag = m_threadPool->waitForDone(10000);
            if (!flag)
            {
                stopTask();
                QString msg = resultToStr(m_queue.taskErrorResult());
                m_queue.clearResult();
                return Result(false, "Task wait for done is over time, manual stop all tasks." + msg.toStdString());
            }

            QString msg = resultToStr(m_queue.taskErrorResult());
            if(!msg.isEmpty()) {
                flag = false;
                msg = QString("%1 were failed executed. Finish tasks number is %2. %3").arg(taskName).arg(m_queue.finishCount()).arg(msg);
            }

            m_queue.clearResult();
            if (!flag) {
                return Result(false, msg.toStdString());
            }
            msg = QString("%1 were successfully executed. Finish tasks number is %2.").arg(taskName).arg(m_queue.finishCount());
            return Result(true, msg.toStdString());
        }

    private:
        QString resultToStr(const QList<Result>& retList)
        {
            if (retList.size() == 0) {
                return QString();
            }

            QString msg = QString("Result is error, number of errors is %1, ").arg(retList.size());
            for (int i = 0; i < retList.size(); ++i) {
                Result ret = retList[i];
                msg += QString("\nError_%1: %2").arg(i + 1).arg(QString::fromStdString(ret.errorMsg));
            }
            return msg;
        }

    private:
        ThreadSafeQueue<QSharedPointer<TaskBase>> m_queue;
        QThreadPool* m_threadPool = nullptr;
        TaskWorker* m_worker = nullptr;

        std::shared_ptr<std::atomic<bool>> m_stopFlag;
    };
}

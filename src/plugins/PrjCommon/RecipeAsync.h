#pragma once
#include <QFutureSynchronizer>
#include "Result.h"
#include <QObject>
#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrent>
#include "prjcommon_global.h"

class PRJCOMMON_EXPORT TrackableSynchronizer : public QObject, public QFutureSynchronizer<Result>
{
    Q_OBJECT

public:
    void addFuture(const QFuture<Result>& future, int taskId = 0) {
        static int mm = 0;
        taskId = ++mm;

        qCritical() << "taskId" << taskId << QDateTime::currentDateTime().toString("yyyyMMdd_hh:mm:ss.zzz");

        QFutureSynchronizer::addFuture(future);
        QFutureWatcher<Result>* watcher = new QFutureWatcher<Result>();
        connect(watcher, &QFutureWatcher<Result>::finished, [this, watcher, taskId]() {
            Result result = watcher->future().result();
            if (!result.success && !result.errorCode)
            {
                qCritical() << QString::fromStdString(result.errorMsg);
            }

            emit this->taskCompleted(taskId, watcher->future().result());
            watcher->deleteLater();
            });
        watcher->setFuture(future);
    }
signals:
    void taskCompleted(int id, Result ret);
};

class PRJCOMMON_EXPORT RecipeAsync: public QObject
{
    Q_OBJECT

public:
	static RecipeAsync& instance();
	RecipeAsync();
	~RecipeAsync();

    template <typename T, typename Class>
    Result waitRun(Class* object, T(Class::* fn)());

    template <typename T, typename Class, typename Param1, typename Arg1>
    Result waitRun(Class* object, T(Class::* fn)(Param1), const Arg1& arg1);

    template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
    Result waitRun(Class* object, T(Class::* fn)(Param1, Param2), const Arg1& arg1, const Arg2& arg2);

    template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
    Result waitRun(Class* object, T(Class::* fn)(Param1, Param2, Param3), const Arg1& arg1, const Arg2& arg2, const Arg3& arg3);

    Result waitEnd();

private:
    TrackableSynchronizer m_sync;
    QList<QFutureWatcher<Result>*> m_watchers;
    QMutex m_mutex;
};

template <typename T, typename Class>
Result RecipeAsync::waitRun(Class* object, T(Class::* fn)())
{
    QMutexLocker locker(&m_mutex);
    QFuture<Result> future = QtConcurrent::run(object, fn);
    m_sync.addFuture(future);
    return Result();
}

template <typename T, typename Class, typename Param1, typename Arg1>
Result RecipeAsync::waitRun(Class* object, T(Class::* fn)(Param1), const Arg1& arg1)
{
    QMutexLocker locker(&m_mutex);
    QFuture<Result> future = QtConcurrent::run(object, fn, arg1);
    m_sync.addFuture(future);
    return Result();
}

template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
Result RecipeAsync::waitRun(Class* object, T(Class::* fn)(Param1, Param2), const Arg1& arg1, const Arg2& arg2)
{
    QMutexLocker locker(&m_mutex);
    QFuture<Result> future = QtConcurrent::run(object, fn, arg1, arg2);
    m_sync.addFuture(future);
    return Result();
}

template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3,
    typename Arg3>
    Result RecipeAsync::waitRun(Class* object, T(Class::* fn)(Param1, Param2, Param3), const Arg1& arg1, const Arg2& arg2,
        const Arg3& arg3)
{
    QMutexLocker locker(&m_mutex);
    QFuture<Result> future = QtConcurrent::run(object, fn, arg1, arg2, arg3);
    m_sync.addFuture(future);
    return Result();
}


#pragma once
#include <QWaitCondition>
#include <QMutex>
#include <QThread>
#include "Result.h"

class WaitPause
{
  public:
    WaitPause();
    ~WaitPause();

    Result waitPause(bool isPause);
    void notifyPause(bool isPause);

    // continue or stop
    void continueRun(bool isContinue);
    bool recipePause();
    bool isStop();

  private:
    QWaitCondition m_waitCondition;
    QMutex m_mutex;
    bool m_isStop;
    bool m_recipePause = false;
};

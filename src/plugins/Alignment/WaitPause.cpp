#include "WaitPause.h"
#include "AlignUtils.h"

WaitPause::WaitPause(): m_isStop(true), m_recipePause(false)
{
}

WaitPause::~WaitPause()
{
}


Result WaitPause::waitPause(bool isPause)
{
    m_recipePause = isPause;
    if (isPause)
    {
        if (!m_mutex.try_lock())
        {
            return Result(false, "Try pause wait failed.");
        }

        m_waitCondition.wait(&m_mutex);
        m_mutex.unlock();
        m_recipePause = false;
        //if (m_isStop)
        //{
        //    return Result(false, "Recipe manual stop.", 1);
        //}
    }

    if (m_isStop)
    {
        return Result(false, "Recipe manual stop.", 1);
    }
    return Result();
}

void WaitPause::notifyPause(bool isPause)
{
    m_recipePause = isPause;
    if (!isPause)
    {
        m_waitCondition.wakeOne();
    }
}

void WaitPause::continueRun(bool isContinue)
{
    AlignUtils::acsMoveStop(!isContinue);
    m_isStop = !isContinue;
    m_waitCondition.wakeOne();

    if (!isContinue)
    {
        QThread::msleep(500);
        AlignUtils::acsMoveStop(false);
        m_recipePause = false;
    }
}

bool WaitPause::recipePause()
{
    return m_recipePause;
}

bool WaitPause::isStop()
{
    return m_isStop;
}

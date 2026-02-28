#ifndef CLEAR_HISTORYDATA_MANAGR
#define CLEAR_HISTORYDATA_MANAGE
#include <qobject.h>
#include <qdatetime.h>
class ClearHistoryDataManage :
    public QObject
{
    Q_OBJECT
public:
    ClearHistoryDataManage();
    ~ClearHistoryDataManage();
    void CycleClearHistoryData();
public slots:
    void work(QDateTime loginTime, int saveDay);
    void StopThread(bool stop);
private:
    QDateTime m_loginTime;
    bool m_isShutDown;
    int m_saveDay;
};
#endif


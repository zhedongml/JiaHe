#include "ClearHistoryDataManage.h"
#include "TableViewDataManage.h"
#include <QThread>
#define OneDaySecs  24*3600

ClearHistoryDataManage::ClearHistoryDataManage()
{
	m_isShutDown = false;
    m_saveDay = 30;
}

ClearHistoryDataManage::~ClearHistoryDataManage()
{

}

void ClearHistoryDataManage::CycleClearHistoryData()
{
	while (!m_isShutDown)
	{
		QDateTime currentT = QDateTime::currentDateTime();
		uint st = currentT.toSecsSinceEpoch();
		uint logint = m_loginTime.toSecsSinceEpoch();
		if (st - logint >= OneDaySecs)
		{
            QString lastT = currentT.addDays(-m_saveDay).toString("yyyyMMddhhmmss");
			TableViewDataManage::DelTimeOutHistoryData(lastT);
		}

		QThread::sleep(5);
	}
}

void ClearHistoryDataManage::StopThread(bool stop)
{
	m_isShutDown = stop;
}

void ClearHistoryDataManage::work(QDateTime loginTime,int saveDay)
{
	m_loginTime = loginTime;
    m_saveDay = saveDay;
    QDateTime oldTime = m_loginTime.addDays(-m_saveDay);
	QString oldDelTime = oldTime.toString("yyyyMMddhhmmss");
	TableViewDataManage::DelTimeOutHistoryData(oldDelTime);
	CycleClearHistoryData();
}

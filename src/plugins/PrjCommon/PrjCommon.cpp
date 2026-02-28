#include "PrjCommon.h"
#include <QMutex>

namespace Core{

	PrjCommon* PrjCommon::m_instance = nullptr;

	PrjCommon::PrjCommon(QObject* parent)
        : QObject(parent)
	{

	}

    PrjCommon* PrjCommon::instance(QObject* parent)
    {
        if (!m_instance)
        {
            static QMutex mutex;
            QMutexLocker locker(&mutex);
            if (!m_instance)
            {
                m_instance = new PrjCommon(parent);
            }
        }
        return m_instance;
    }
}


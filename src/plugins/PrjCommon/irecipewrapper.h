#pragma once
#include <QObject>
#include "service/ml.h"
#include "prjcommon_global.h"

class PRJCOMMON_EXPORT IRecipeWrapper : public QObject
{
    Q_OBJECT

  public:
    IRecipeWrapper(QObject *parent);
    ~IRecipeWrapper();
    virtual void Invoke(QString cmd, QString param) = 0;
    void setSignalConnected(bool connected);
    bool getSignalConnected();
    void trigerSinal(CORE::WrapperStatus status, QString msg);
    virtual void notifyStop(bool isstop);
    virtual void notifyPause(bool isPause);
    bool getIsStop();
    bool getIsPause();

  private:
    bool signalConnected = false;
    bool isStop = false;
    bool m_isPause = false;
  signals:
    void SendStatusSignal(CORE::WrapperStatus, QString);
};

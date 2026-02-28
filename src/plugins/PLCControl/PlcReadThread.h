#pragma once
#include <QThread>
#include "PLCConfig.h"
#include <basetsd.h>
#include "Result.h"

#define GetBit(v, n) ((v) & ((UINT32)1 << (n)))
#define SetBit(v, n) ((v) |= ((UINT32)1 << (n)))
#define ClearBit(v, n) ((v) &= ~((UINT32)1 << (n)))

using namespace std;

class PlcCallback{
  public:
    virtual void updateUserLevel() = 0;
    virtual Result testing(bool flag) = 0;
    virtual void sendState(int, int) = 0;
    virtual Result interLock(bool isOpen) = 0;
};

class PLCCONTROL_EXPORT PlcReadThread : public QThread
{
  public:
    PlcReadThread(QObject *parent = nullptr);
    ~PlcReadThread();

    void setCells(const QMap<int, PLCCell> &cells);
    void setCallback(PlcCallback *plcCallback);

    bool getDoorState();
    bool getInterLockState();

    bool getState(QString name);

  protected:
    void run() override;

  private:
    bool open();
    void close();

    void startProcessing();
    void stopProcessing();

  private:
    class PLCControllerImpl;
    std::unique_ptr<PLCControllerImpl> m_pImpl;

  private:
    bool m_Stopping;
    QMap<int, PLCCell> m_Cells;

    // true:close false:open
    bool doorState = true;
    bool m_interlockDoorState = true;

    bool interlock = false;
    bool istesting = false;
    bool doorReplyState = false;

    PlcCallback *m_plcCallback;
    QMap<int, int> m_cellStateMap;
    QMap<QString, int> m_nameStateMap;
};

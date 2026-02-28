#pragma once

#include "prjcommon_global.h"
#include <QObject>
#include <QString>
#include "Result.h"

namespace Core
{
class PRJCOMMON_EXPORT PrjCommon : public QObject
{
    Q_OBJECT

public:
    static PrjCommon* instance(QObject* parent = nullptr);

private:
    PrjCommon(QObject* parent = nullptr);

signals:
    void coreAboutToOpen();
    void coreOpened();
    void coreAboutToClose();
    void systemEnvironmentChanged();
    void goLoadDUTPostionSig();
    void connectAllDevicesSig();
    //void recipeRunStartOrEnd(bool isStart);
    void InterLockRestart();
    void currentModeChanged(QString);
    // interlock true, Door true(close), state true
    void interlockDoorState(bool state);
    void historyDataShow();
    void historyimageShow(QString path);
    void historyimageUpdate();
    void updateDutConfigSignal();
    void updateLabsphereColor(QString);

    // Task thread
    void taskErrorSignal(Result ret);

private:
    static PrjCommon* m_instance;

};
}

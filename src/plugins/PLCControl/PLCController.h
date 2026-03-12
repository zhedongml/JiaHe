#pragma once

#include "PLCConfig.h"
#include "plccontrol_global.h"
#include <QThread>
#include <string>
#include "PLCConfig.h"
#include "Result.h"
#include "PlcReadThread.h"

class PLCCONTROL_EXPORT PLCController : public QObject, public PlcCallback
{
    Q_OBJECT

  signals:
    void sendStateToUI(int, int);

  public:
    static PLCController *instance(QObject *parent = nullptr);
    ~PLCController();

  public:
    bool Connect();
    void DisConnect();
    bool IsConnected();
    bool SendCommond(int index, int value);
    QMap<int, PLCCell> &GetAllPLCCells();

    Result coaxialLight(bool isOpen);
    Result coaxialLight2(bool isOpen);
    Result lightBoard(bool isOpen);
    Result lightBoardUp(bool isOpen);
    Result keyenceLight(bool isOpen);
    Result keyenceLightUp(bool isOpen);
    Result collisionControl(bool isOpen);
    Result collimatorLight(bool isOpen);

    Result projectorUp(bool up);

    Result collisionControlPreMove();
    Result closeLightBeforeRecipe();

    bool GetDoorState();
    bool GetInterlockState();

    bool GetSensorAState();
    bool GetSensorBState();
    bool GetSensorCState();
    bool GetSensorDState();

    std::string GetEmptySensorState();

  private:
    PLCController(QObject *parent = nullptr);
    Result SendCommond(const QString &name, bool isOpen);
    Result testing(bool isOpen) override;
    Result interLock(bool isOpen) override;

    void sendState(int index, int value) override;

  private slots:
    void recipeRunStartOrEnd(bool isStart);
    void updateUserLevel() override;

  private:
    class PLCControllerImpl;
    std::unique_ptr<PLCControllerImpl> m_pImpl;

  public:
    QMap<int, PLCCell> m_Cells;

  private:
    static PLCController *self;

    PlcReadThread *m_readThread;
    std::string m_emptyState = "0000";
};

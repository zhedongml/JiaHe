#include "PLCController.h"
#include "Core/icore.h"
#include <QMutex>
#include <basetsd.h>
#include "PrjCommon/logindata.h"
#include <snap7.h>
#include "PrjCommon/PrjCommon.h"
#include "Recipe2.h"
#include "Core/modemanager.h"
#include "Core/imode.h"
#include "RecipeXMLEngine/RecipeXMLEngine.h"

#define GetBit(v, n) ((v) & ((UINT32)1 << (n)))
#define SetBit(v, n) ((v) |= ((UINT32)1 << (n)))
#define ClearBit(v, n) ((v) &= ~((UINT32)1 << (n)))

PLCController *PLCController::self = nullptr;

class PLCController::PLCControllerImpl
{
  public:
    PLCControllerImpl()
        : m_DBNumber(1){

          };
    ~PLCControllerImpl()
    {
        if (IsConnected())
        {
            Disconnect();
        }
    };

    bool Connect(const char *ip, int rack, int slot)
    {
        m_pClient.ConnectTo(ip, rack, slot);
        return m_pClient.Connected();
    };
    void Disconnect()
    {
        m_pClient.Disconnect();
    };

    bool IsConnected()
    {
        return m_pClient.Connected();
    }
    void GetData(unsigned char *buffer, int start, int len)
    {
        m_pClient.DBRead(m_DBNumber, start, len, buffer);
    };

    void SetDate(unsigned char *buffer, int start, int len)
    {
        m_pClient.DBWrite(m_DBNumber, start, len, buffer);
    }

  private:
    TS7Client m_pClient;
    int m_DBNumber;
};

PLCController *PLCController::instance(QObject *parent)
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new PLCController(parent);
        }
    }
    return self;
}

PLCController::PLCController(QObject *parent) : QObject(parent), m_pImpl(new PLCControllerImpl())
{
    if (PLCConfig::GetInstance().Load())
    {
        m_Cells = PLCConfig::GetInstance().ReadInfo();
    }

    Core::IMode* mode = Core::ModeManager::instance()->findMode("Recipe.RecipeXMLMode");
    bool success = connect(static_cast<RecipeXMLEngine::Internal::RecipeXMLMode*>(mode)->GetRecipe2Plugin(), &Recipe2::updateTreeSystemIsRunDone,
        this, [this](bool value) {
            recipeRunStartOrEnd(!value);
        });
    //connect(Core::PrjCommon::instance(), SIGNAL(recipeRunStartOrEnd(bool)), this, SLOT(recipeRunStartOrEnd(bool)));
    connect(Core::PrjCommon::instance(), SIGNAL(updateUserLevel()), this, SLOT(updateUserLevel()));

    {
        m_readThread = new PlcReadThread();
        m_readThread->setCells(m_Cells);
        m_readThread->setCallback(this);
    }
}

PLCController::~PLCController()
{
    if (m_readThread != nullptr){
        delete m_readThread;
        m_readThread = nullptr;
    }

    testing(false);
}

bool PLCController::Connect()
{
    ConnectInfo info = PLCConfig::GetInstance().GetConnectInfo();
    bool res = m_pImpl->Connect(info.ip.c_str(), info.rack, info.slot);
    return res;
}

void PLCController::DisConnect()
{
    m_pImpl->Disconnect();
}

bool PLCController::IsConnected()
{
    return m_pImpl->IsConnected();
}

bool PLCController::SendCommond(int index, int value)
{
    if (!IsConnected())
    {
        bool ret = Connect();
        if (!ret){
            return false;
        }
    }

    PLCCell cell = m_Cells.value(index);
    int byte, bit;
    PLCConfig::GetInstance().getItemOffset(cell.offset, byte, bit);
    unsigned char buffer;
    m_pImpl->GetData(&buffer, byte, 1);
    if (value)
    {
        SetBit(buffer, bit);
    }
    else
    {
        ClearBit(buffer, bit);
    }

    int val = GetBit(buffer, bit);
    m_pImpl->SetDate(&buffer, byte, 1);
    return true;
}

QMap<int, PLCCell> &PLCController::GetAllPLCCells()
{
    return m_Cells;
}

Result PLCController::coaxialLight(bool isOpen)
{
    return SendCommond("Coaxia Light", isOpen);
}

Result PLCController::coaxialLight2(bool isOpen)
{
    return SendCommond("Coaxia Light2", isOpen);
}

Result PLCController::lightBoard(bool isOpen)
{
    return SendCommond("Light Board2", isOpen);
}

Result PLCController::lightBoardUp(bool isOpen)
{
    return SendCommond("Light Board", isOpen);
}

Result PLCController::keyenceLight(bool isOpen)
{
    return SendCommond("CL_3000", isOpen);
}

Result PLCController::keyenceLightUp(bool isOpen)
{
    return SendCommond("CL_3000 Above", isOpen);
}

Result PLCController::collisionControl(bool isOpen)
{
    return Result();
    return SendCommond("Stop_Sensor", !isOpen);
}

Result PLCController::collimatorLight(bool isOpen)
{
    return SendCommond("Collimator Light", isOpen);
}

Result PLCController::projectorUp(bool up)
{
    if (up)
    {
        Result res = SendCommond("Projector Down", false);
        if (!res.success)
        {
            return res;
        }

        QThread::msleep(500);
        res = SendCommond("Projector Up", true);
        if (!res.success)
        {
            return res;
        }
    }
    else
    {
        Result res = SendCommond("Projector Up", false);
        if (!res.success)
        {
            return res;
        }

        QThread::msleep(500);
        res = SendCommond("Projector Down", true);
        if (!res.success)
        {
            return res;
        }
    }
    return Result();
}

Result PLCController::collisionControlPreMove()
{
    // TODO:
    return Result();

    if (!IsConnected())
    {
        return Result(false, "Collision control error, PLC is not connected.");
    }

    bool state = false;;
    int number = 0;
    while (number++ < 10)
    {
        state = m_readThread->getState("Collision Light");
        if(state){
            break;
        }

        Result ret = collisionControl(true);
        if (!ret.success)
        {
            return ret;
        }
        QThread::msleep(200);
    }

    if (!state){
        state = m_readThread->getState("Collision Light");
    }

    if (!state)
    {
        return Result(false, "Collision light open failed.");
    }
    return Result();
}

Result PLCController::closeLightBeforeRecipe()
{
    bool isOpen = false;
    Result ret = coaxialLight(isOpen);
    if (!ret.success)
    {
        return ret;
    }
    ret = coaxialLight2(isOpen);
    if (!ret.success)
    {
        return ret;
    }
    ret = lightBoard(isOpen);
    if (!ret.success)
    {
        return ret;
    }
    ret = lightBoardUp(isOpen);
    if (!ret.success)
    {
        return ret;
    }
    ret = keyenceLight(isOpen);
    if (!ret.success)
    {
        return ret;
    }

    ret = SendCommond("Lamp", isOpen);
    if (!ret.success)
    {
        return ret;
    }

    ret = collisionControl(isOpen);
    if (!ret.success)
    {
        return ret;
    }

    ret = collimatorLight(isOpen);
    if (!ret.success)
    {
        return ret;
    }

    return Result();
}

Result PLCController::testing(bool isOpen)
{
    return SendCommond("Testing", isOpen);
}

Result PLCController::interLock(bool isOpen)
{
    return SendCommond("InterLock", isOpen);
}

void PLCController::sendState(int index, int value)
{
    emit sendStateToUI(index, value);
}

Result PLCController::SendCommond(const QString &name, bool isOpen)
{
    QMap<int, PLCCell>::Iterator iter = m_Cells.begin();
    while (iter != m_Cells.end())
    {
        if (iter.value().name.compare(name, Qt::CaseInsensitive) == 0)
        {
            bool ret = SendCommond(iter.key(), isOpen);
            return Result(ret, "send commond failed");
        }
        ++iter;
    }
    return Result(false, QString("%1 control failed.").arg(name).toStdString());
}

void PLCController::recipeRunStartOrEnd(bool isStart)
{
    if (isStart)
    {
        testing(true);
    }else{
        testing(false);
        coaxialLight(false);
    }
}

void PLCController::updateUserLevel()
{
    if (LoginData::instance()->getUserLevel() == USERLEVEL::OPERATOR){
        interLock(true);
    }else{
        interLock(false);
    }
}

bool PLCController::GetDoorState()
{
    return m_readThread->getDoorState();
}

bool PLCController::GetInterlockState()
{
    return m_readThread->getInterLockState();
}

bool PLCController::GetSensorAState()
{
    return m_readThread->getState("Sensor A");
}

bool PLCController::GetSensorBState()
{
    return m_readThread->getState("Sensor B");
}

bool PLCController::GetSensorCState()
{
    return m_readThread->getState("Sensor C");
}

bool PLCController::GetSensorDState()
{
    return m_readThread->getState("Sensor D");
}
#include "AlignPlugin.h"
#include "AlignWidget.h"
#include "AlignWrapper.h"
#include "DeviceControlWidget.h"
#include "DeviceControlWidget2D.h"
#include "CdsWidget.h"
#include "calibratewidget.h"
#include "Core/outputpane.h"
#include "PrjCommon/recipewrapperregistry.h"
#include "pluginsystem/Services.h"
#include "Core/loggingwrapper.h"
#include <QSplitter>
#include <Core/actionmanager/actioncontainer.h>
#include <Core/actionmanager/actionmanager.h>
#include <Core/coreconstants.h>
#include <Core/icore.h>
#include <Core/imode.h>
#include <Core/modemanager.h>
#include <QDesktopWidget>
//#include "DutConfigAction.h"
#include "DutAA4Recipe.h"
#include "PImotion4Recipe.h"
#include "webcamera/CamereAction.h"

BT_REGISTER_NODES(factory)
{
    DutAA::RegisterNodes(factory);
    PImotion::RegisterNodes(factory);
}

class CalibrateMode : public Core::IMode
{
  public:
    ~CalibrateMode()
    {
        if (m_widget != nullptr)
        {
            delete m_widget;
            m_widget = nullptr;
        }
    }

    CalibrateMode(QObject *parent = nullptr)
    {
        m_widget = new CalibrateWidget();
        setWidget(m_widget);
        setContext(Core::Context("CalibrateMent.MainView"));
        setDisplayName(tr("DUT AA"));
        setIcon(QIcon(":/image/calibrate.png"));
        setPriority(3);
        setId("CalibrateMent.CalibrateMode");
    }

    CalibrateWidget *getWidget()
    {
        return m_widget;
    }

  private:
    CalibrateWidget *m_widget;
};

AlignPlugin::AlignPlugin()
{
}

AlignPlugin::~AlignPlugin()
{
    if (m_calibrateMode != nullptr){
        delete m_calibrateMode;
        m_calibrateMode = nullptr;
    }
}

bool AlignPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    m_calibrateMode = new CalibrateMode(this);
    CalibrateWidget* wgt = m_calibrateMode->getWidget();
    AlignWrapper *wrapper = new AlignWrapper(this, wgt);
    DutAA::DutAA4Recipe::getInstance(wgt);
    PImotion::PImotion4Recipe::getInstance();
    RecipeWrapperRegistry::Instance()->regist("DutAA", wrapper);
    return true;
}

void AlignPlugin::extensionsInitialized()
{
    DeviceControlWidget *deviceControlWidget = new DeviceControlWidget("Motion3D Imaging");
    DeviceControlWidget* dutControlWidget = new DeviceControlWidget("Motion3D Dut");
    CdsWidget* dsWidget = new CdsWidget("Confocal Distance Sensor");

    CamereAction* actionCamera = new CamereAction(Constants::TOGGLE_WEB_CAMERA, Constants::M_TOOLS);
    //DutConfigAction *action = new DutConfigAction(Constants::TOGGLE_DUT_CONFIG, Constants::M_TOOLS);
}

CalibrateMode* AlignPlugin::getCalibrateMode()
{
    return m_calibrateMode;
}

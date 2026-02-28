#include "MAalignment.h"
#include "MAalignmentWrapper.h"
#include "Core/outputpane.h"
#include "pluginsystem/Services.h"
#include <Core/actionmanager/actioncontainer.h>
#include <Core/actionmanager/actionmanager.h>
#include <Core/coreconstants.h>
#include <Core/icore.h>
#include <Core/imode.h>
#include <Core/modemanager.h>
#include <PrjCommon/recipewrapperregistry.h>
#include <MAalignment/DeviceControlWidget.h>
#include "InitCalibrateCfgAction.h"
#include "piMotionActor/ConfigMAInfo.h"

class MAalignmentMode : public Core::IMode
{
  public:
    MAalignmentMode(aligmentWidget *widget)
    {
        m_widget = widget;
        setWidget(m_widget);
        setContext(Core::Context("MAalignment.MainView"));
        setDisplayName(tr("InputAA"));
        setIcon(QIcon(":/image/image/alig.png"));
        setPriority(4);
        setId("MAalignment.MAalignmentMode");
    }

    ~MAalignmentMode()
    {
        if (m_widget != nullptr){
            delete m_widget;
            m_widget = nullptr;
        }
    }

    aligmentWidget *getWidget()
    {
        return m_widget;
    }

  private:
    aligmentWidget *m_widget;
};
MAalignment::MAalignment()
{
}
MAalignment::~MAalignment()
{
    if (m_alignmentMode != nullptr){
        delete m_alignmentMode;
        m_alignmentMode = nullptr;
    }
}

bool MAalignment::initialize(const QStringList &arguments, QString *errorMessage)
{
    Core::Context context("MAalignment.MainView");
    m_alignWidget = new aligmentWidget();

    if(ConfigMAInfo::GetInstance()->isDisplayUI()){
        m_alignmentMode = new MAalignmentMode(m_alignWidget);
    }

    //m_slbWidget = new SLBWidget(m_alignWidget, "SLB Alignment");
    //MAalignmentWrapper *wrapper = new MAalignmentWrapper(this, m_alignWidget, m_slbWidget);
    //RecipeWrapperRegistry::Instance()->regist("InputAA", wrapper);
    //RecipeWrapperRegistry::Instance()->regist("SLB", wrapper);
    return true;
}

void MAalignment::extensionsInitialized()
{
    //ProjectorTiltControlWidget *projectorTiltControlWidget = new ProjectorTiltControlWidget("Projector");
    //DeviceControlWidget *deviceControlWidgetTiltDut = new DeviceControlWidget("Dut Tilt Station");
    DeviceControlWidget *deviceControlWidgetTiltPro = new DeviceControlWidget("Motion2D-Tilt Projector");
    DeviceControlWidget* dutTiltControlWidget = new DeviceControlWidget("Motion2D-Tilt Imaging");

    //InitCalibrateCfgAction* action = new InitCalibrateCfgAction(Constants::TOGGLE_INIT_CALIBRATE_CFG, Constants::M_TOOLS);
}
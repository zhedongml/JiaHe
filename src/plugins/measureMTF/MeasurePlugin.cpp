#include "MeasurePlugin.h"
#include <QWidget>

#include "measurewidget.h"
#include "mtfresultwidget.h"

#include "Core/icore.h"
#include "Core/imode.h"
#include "PrjCommon/logindata.h"
#include "Core/modemanager.h"
#include "Core/outputpane.h"
#include "PrjCommon/recipewrapperregistry.h"

#include "ExposureWidget.h"
#include "iqmetricconfig.h"
#include "Core/loggingwrapper.h"

#include "Focus/focuswidget.h"
#include "Cylinder/RxFilterWheelWidget.h"
#include "IQT4Recipe.h"
#include "System4Recipe.h"

#include "behaviortree_cpp_v3/bt_factory.h"
BT_REGISTER_NODES(factory)
{
    IQT::RegisterNodes(factory);
    System::RegisterNodes(factory);
}

class MeasureMode : public Core::IMode
{
  public:
    MeasureWidget *measureWidget = nullptr;
    void postInitalize()
    {
        if (measureWidget != nullptr)
        {
            measureWidget->postInitalize();
        }
    };
    MeasureMode()
    {
        measureWidget = new MeasureWidget();
        setWidget(measureWidget);
        setContext(Core::Context("MeasureMent.MainView"));
        setDisplayName(tr("Measure"));
        setIcon(QIcon(":/image/image/MTF.png"));
        setPriority(2);
        setId("MeasureMent.MeasureMode");
    }

    ~MeasureMode(){
        if (measureWidget != nullptr){
            delete measureWidget;
            measureWidget = nullptr;
        }  
    }

    MeasureWidget *getMeasureWidget()
    {
        return measureWidget;
    }
};
MeasurePlugin::MeasurePlugin()
{
    //_CrtSetBreakAlloc(159);
}

MeasurePlugin::~MeasurePlugin()
{
    if (m_measureMode != nullptr)
    {
        delete m_measureMode;
        m_measureMode = nullptr;
    }
}

bool MeasurePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Core::Context context("MeasureMent.MainView");

    //TODO: mode deal
    m_measureMode = new MeasureMode;
    //USERLEVEL lev = LoginData::instance()->getUserLevel();
    //if (lev == USERLEVEL::OPERATOR)
    //{
    //    m_measureMode->setHide(true);
    //}
    IQT::IQT4Recipe::getInstance();
    System::System4Recipe::getInstance();
    MTFResultWidget *mtfResult = MTFResultWidget::getInstance();
    RecipeWrapperRegistry::Instance()->registoryResultWidget("MTF", mtfResult);
    //MTFWrapper *wrapper = new MTFWrapper(this);
    //RecipeWrapperRegistry::Instance()->regist("MTF", wrapper);

    //Core::ModeManager::instance()->activateMode("MeasureMent.MeasureMode");
    return true;
}

void MeasurePlugin::extensionsInitialized()
{
    ExposureWidget *exposureWidget = new ExposureWidget("Colorimeter-MeasureCamera");
    if (m_measureMode != nullptr)
        m_measureMode->postInitalize();

    //IQTWrapper *iqtwrapper = new IQTWrapper(m_measureMode->getMeasureWidget(), this);
    //RecipeWrapperRegistry::Instance()->regist("IQ", iqtwrapper);

    FocusWidget* focusWgt = new FocusWidget("Colorimeter-Focus");

    RxFilterWheelWidget* rxfilter = new RxFilterWheelWidget("Colorimeter-RX");

    //MeasureSetupWidget *meaSetupTlbox = new MeasureSetupWidget("Measure Setup");
}

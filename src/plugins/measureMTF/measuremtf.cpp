#include <QWidget>

#include "mtfmeasurewidget.h"
#include "mtfresultwidget.h"

#include "coreplugin/icore.h"
#include "coreplugin/imode.h"
#include "coreplugin/logindata.h"
#include "coreplugin/minisplitter.h"
#include "coreplugin/modemanager.h"
#include "coreplugin/outputpane.h"
#include "coreplugin/recipewrapperregistry.h"

#include "CameraConfig.h"
#include "ExposureWidget.h"
#include "dpacontrolwidget.h"
#include "iqmetricconfig.h"
#include "iqtwrapper.h"
#include "loggingwrapper.h"
#include "measuremtf.h"
#include "measuresetupwidget.h"

#include "displaywrapper.h"
#include "mtfwrapper.h"
#include "serialportwrapper.h"
#include "focuswidget.h"
class MTFMeasureMode : public Core::IMode
{
  public:
    QSplitter *splitter;
    MTFmeasureWidget *mtfmeasureWidget = nullptr;
    void postInitalize()
    {
        if (mtfmeasureWidget != nullptr)
        {
            mtfmeasureWidget->postInitalize();
        }
    };
    MTFMeasureMode()
    {

        splitter = new Core::MiniSplitter;
        splitter->setOrientation(Qt::Vertical);
        mtfmeasureWidget = new MTFmeasureWidget();
        splitter->insertWidget(0, mtfmeasureWidget);
        QWidget *outputPane = new Core::OutputPanePlaceHolder("MTFMeasureMent.MTFMeasureMode", splitter);
        outputPane->setObjectName(QLatin1String("MeasureModeOutputPanePlaceHolder"));
        splitter->insertWidget(1, outputPane);
        splitter->setStretchFactor(0, 2);
        splitter->setStretchFactor(1, 0);
        setWidget(splitter);
        setContext(Core::Context("MeasureMent.MainView"));
        setDisplayName(tr("MTFMeasure"));
        setIcon(QIcon(":/image/image/MTF.png"));
        setPriority(0);
        setId("MTFMeasureMent.MTFMeasureMode");
    }

    MTFmeasureWidget *getMeasureWidget()
    {
        return mtfmeasureWidget;
    }
};
measureMTF::measureMTF()
{
    //_CrtSetBreakAlloc(159);
}

bool measureMTF::initialize(const QStringList &arguments, QString *errorMessage)
{
    Core::Context context("MeasureMent.MainView");

    //TODO: mode deal
    m_mtfMeasureMode = new MTFMeasureMode;
    USERLEVEL lev = LoginData::instance()->getUserLevel();
    if (lev == USERLEVEL::OPERATOR)
    {
        m_mtfMeasureMode->setHide(true);
    }
    MTFResultWidget *mtfResult = MTFResultWidget::getInstance();
    RecipeWrapperRegistry::Instance()->registoryResultWidget("MTF", mtfResult);
    MTFWrapper *wrapper = new MTFWrapper(this);
    RecipeWrapperRegistry::Instance()->regist("MTF", wrapper);
    return true;
}

void measureMTF::extensionsInitialized()
{
    appoutput = new MeasureMent::Internal::AppOutputPane;
    LoggingWrapper::instance()->setAppOutPutPane(appoutput);
    ExposureWidget *exposureWidget = new ExposureWidget("Exposure Widget");
    FocusWidget* focusWgt = new FocusWidget("Focus");
    //DPAControlWidget *dpaWidget = new DPAControlWidget("DPA Control");
    if (m_mtfMeasureMode != nullptr)
        m_mtfMeasureMode->postInitalize();

    IQTWrapper *iqtwrapper = new IQTWrapper(m_mtfMeasureMode->getMeasureWidget() , this);
    RecipeWrapperRegistry::Instance()->regist("IQ", iqtwrapper);

    SerialPortWrapper *serialportwrapper = new SerialPortWrapper(this);
    RecipeWrapperRegistry::Instance()->regist("DUT", serialportwrapper);

    //DisplayWrapper *displaywrapper = new DisplayWrapper(this);
    //RecipeWrapperRegistry::Instance()->regist("DISP", displaywrapper);

    MeasureSetupWidget *meaSetupTlbox = new MeasureSetupWidget("Measure Setup");
}

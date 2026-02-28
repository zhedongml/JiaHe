#include "mtfplugin.h"
#include "core/icore.h"
#include "core/imode.h"
#include "mtfchartwidget.h"
#include "mtfsettingwidget.h"
class MTFMeasureMode : public Core::IMode
{
public:
   
    void postInitalize()
    {
        
    };
    MTFMeasureMode()
    {
        setLogicWidget(new MTFSettingWidget());
        setResultWidget(new MTFChartWidget());
        setContext(Core::Context("MeasureMent.MainView"));
        setDisplayName(tr("MTFMeasure"));
        setIcon(QIcon(":/image/image/MTF.png"));
        setPriority(0);
        setId("MTFMeasureMent.MTFMeasureMode");
    }
};
MTFPlugin::MTFPlugin()
{

}

bool MTFPlugin::initialize(const QStringList& arguments, QString* errorMessage)
{
    Core::Context context("MeasureMent.MainView");
    m_mtfMeasureMode = new MTFMeasureMode;
    return true;
}

void MTFPlugin::extensionsInitialized()
{
}

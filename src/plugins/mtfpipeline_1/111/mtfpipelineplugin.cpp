#include "mtfpipelineplugin.h"
//#include "mtfMeasureWidget.h"

MTFPipeLinePlugin::MTFPipeLinePlugin()
	: IPlugin()
{
	//MtfMeasureWidget mtfwidget = new MtfMeasureWidget();
	
}

MTFPipeLinePlugin::~MTFPipeLinePlugin()
{
}

bool MTFPipeLinePlugin::initialize(const QStringList& arguments, QString* errorString)
{
	Q_UNUSED(arguments)
		Q_UNUSED(errorString)

		return true;
}

void MTFPipeLinePlugin::extensionsInitialized()
{
}

ExtensionSystem::IPlugin::ShutdownFlag MTFPipeLinePlugin::aboutToShutdown()
{
	return ShutdownFlag();
}

void MTFPipeLinePlugin::triggerAction()
{
}

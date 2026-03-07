#include "MesPlugin.h"
#include "MesWrapper.h"
#include "PrjCommon/recipewrapperregistry.h"
#include "MesTaskAsync.h"
#include "MesMode.h"
#include <QDebug>
#include "MesConfigAction.h"
#include <Core/coreconstants.h>
#include "Mes4Recipe.h"
#include "PrjCommon/CsvDequeRecorder.h"
//using namespace MesNS;

BT_REGISTER_NODES(factory)
{
	MesNS::RegisterNodes(factory);
}

MesPlugin::MesPlugin()
{
}

MesPlugin::~MesPlugin()
{
	MesTaskAsync::instance().stopThread();
}

bool MesPlugin::initialize(const QStringList& arguments, QString* errorMessage)
{
	MesNS::Mes4Recipe::getInstance();

	MesWrapper* wrapper = new MesWrapper();
	//MesConfigAction* mesAction = new  MesConfigAction(Constants::TOGGLE_MES_CONFIG);
	RecipeWrapperRegistry::Instance()->regist("MES", wrapper);
	return true;
}

void MesPlugin::extensionsInitialized()
{
	MesMode::instance();
	MesTaskAsync::instance();
	QTimer::singleShot(8000, this, []() {
		MesMode::instance().DetectIncompleteAdpTask();
		});

}

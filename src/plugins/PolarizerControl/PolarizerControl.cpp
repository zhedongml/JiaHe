#include "PolarizerControl.h"
#include "PolarizerWidget.h"
#include "Polarizer4Recipe.h"

BT_REGISTER_NODES(factory)
{
	Polarizer::RegisterNodes(factory);
}


PolarizerControl::PolarizerControl()
{
}

PolarizerControl::~PolarizerControl()
{
}

bool PolarizerControl::initialize(const QStringList& arguments, QString* errorMessage)
{
	//PolarizerWrapper* wrapper = new PolarizerWrapper(this);
	//RecipeWrapperRegistry::Instance()->regist("QrCode", wrapper);
	//PolarizerControlSN::Decoder4Recipe::getInstance();

	//PolarizerWidget* widget = new PolarizerWidget("Polarizer");
	//Polarizer::Polarizer4Recipe::getInstance();
	return true;
}

void PolarizerControl::extensionsInitialized()
{
}

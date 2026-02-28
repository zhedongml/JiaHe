#include "DUTMotionPlugin.h"
//#include "coreplugin/recipewrapperregistry.h"
//#include "DUTMotionWrapper.h"
#include "DUTMotion4Recipe.h"

BT_REGISTER_NODES(factory)
{
    DUTMotion::RegisterNodes(factory);
}

DUTMotionPlugin::DUTMotionPlugin()
{
}

DUTMotionPlugin::~DUTMotionPlugin()
{
}

bool DUTMotionPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{

    DUTMotion::DUTMotion4Recipe::getInstance();
    DUTMotionWidget *widget = new DUTMotionWidget("Motion3D-Tilt Dut");

    //DUTMotionWrapper *wrapper = new DUTMotionWrapper(this);
    //RecipeWrapperRegistry::Instance()->regist("DUTMotion", wrapper);
	

    return true;
}

void DUTMotionPlugin::extensionsInitialized()
{
}

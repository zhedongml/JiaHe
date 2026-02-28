#include <Windows.h>
#include "IntegratingSpherePlugin.h"
#include "PrjCommon/recipewrapperregistry.h"
#include "integratingsphereWrapper.h"
#include "integratingspherecontroller.h"
#include "ReticleWidget2D.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "Integratingsphere4Recipe.h"
#include "Reticle4Recipe.h"

BT_REGISTER_NODES(factory)
{
    IntegratingSphere::RegisterNodes(factory);
    Reticle::RegisterNodes(factory);
}
IntegratingSpherePlugin::IntegratingSpherePlugin()
{
}

IntegratingSpherePlugin::~IntegratingSpherePlugin()
{
}

bool IntegratingSpherePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    IntegratingSphereController *widget = new IntegratingSphereController("Integrating Sphere");
    //ReticleWidget *widgetReticle = new ReticleWidget("Reticle");
    ReticleWidget2D* widgetReticle = new ReticleWidget2D("Motion2D Reticle");
    IntegratingSphere::IntegratingSphere4Recipe::getInstance();
    Reticle::Reticle4Recipe::getInstance();
    IntegratingsphereWrapper *wrapper = new IntegratingsphereWrapper(this);
    RecipeWrapperRegistry::Instance()->regist("integratingSphere", wrapper);
    RecipeWrapperRegistry::Instance()->regist("reticle", wrapper);
    return true;
}

void IntegratingSpherePlugin::extensionsInitialized()
{
}

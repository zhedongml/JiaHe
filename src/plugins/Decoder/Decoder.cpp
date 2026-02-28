#include "Decoder.h"
#include "DecoderWidget.h"
#include "DecoderWrapper.h"
#include "PrjCommon/recipewrapperregistry.h"
#include "Decoder4Recipe.h"

BT_REGISTER_NODES(factory)
{
	DecoderSN::RegisterNodes(factory);
}

Decoder::Decoder()
{
}

Decoder::~Decoder()
{
}

bool Decoder::initialize(const QStringList& arguments, QString* errorMessage)
{
	DecoderWrapper* wrapper = new DecoderWrapper(this);
	RecipeWrapperRegistry::Instance()->regist("QrCode", wrapper);
	DecoderSN::Decoder4Recipe::getInstance();

	DecoderWidget *widget = new DecoderWidget("Barcode Reader");
	return true;
}

void Decoder::extensionsInitialized()
{
}

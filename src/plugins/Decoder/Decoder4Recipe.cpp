#include "Decoder4Recipe.h"
#include "loggingwrapper.h"
#include "DecoderModel.h"
#include <PrjCommon\metricsdata.h>

using namespace DecoderSN;
Decoder4Recipe* Decoder4Recipe::self = nullptr;
Decoder4Recipe* Decoder4Recipe::getInstance() {

	if (!self)
	{
		self = new Decoder4Recipe();
	}
	return self;
}

Decoder4Recipe::Decoder4Recipe(QObject* parent)
	: QObject(parent)
{
	ObjectManager::getInstance()->registerObject("Decoder4Recipe", static_cast<void*>(this));
}

Decoder4Recipe::~Decoder4Recipe()
{
}

QString Decoder4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
{
	auto f_value = node.getInput<std::string>(name);
	if (!f_value)
	{
		throw BT::RuntimeError("Missing input [force]: ", f_value.error());
	}
	return QString::fromStdString(f_value.value());
}

NodeStatus DecoderSN::Decoder4Recipe::Decoder_Connect()
{
	QString sn;
	Result res = DecoderModel::instance().connect();
	if (!res.success) {
		QString message = QString("Recipe Node [ Decoder_Connect ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus Decoder4Recipe::Decoder_SN(BT::TreeNode& node)
{
	QString sn;
	Result res = DecoderModel::instance().queryBarcode(sn);
	if (!res.success) {
		QString message = QString("Recipe Node [ Decoder_SN ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	node.setOutput("key", sn.toStdString());
	MetricsData::instance()->setDutBarCode(sn);

	QString message = QString("=========================== Current Test SN: %1 ===========================")
		.arg(sn);
	LoggingWrapper::instance()->info(message);

	return BT::NodeStatus::SUCCESS;
}
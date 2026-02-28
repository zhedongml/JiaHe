#include "Polarizer4Recipe.h"
#include "ThorlabsMode.h"
#include "loggingwrapper.h"

using namespace Polarizer;
Polarizer4Recipe* Polarizer4Recipe::self = nullptr;
Polarizer4Recipe* Polarizer4Recipe::getInstance() {

	if (!self)
	{
		self = new Polarizer4Recipe();
	}
	return self;
}

Polarizer4Recipe::Polarizer4Recipe(QObject* parent)
	: QObject(parent)
{
	ObjectManager::getInstance()->registerObject("Polarizer4Recipe", static_cast<void*>(this));
}

Polarizer4Recipe::~Polarizer4Recipe()
{
}

QString Polarizer4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
{
	auto f_value = node.getInput<std::string>(name);
	if (!f_value)
	{
		throw BT::RuntimeError("Missing input [force]: ", f_value.error());
	}
	return QString::fromStdString(f_value.value());
}

NodeStatus Polarizer4Recipe::Connect()
{
	Result res = ThorlabsMode::instance()->Connect();
	if (!res.success) {
		QString message = QString("Recipe Node [ Polarizer_Connect ] run error, connect failed");
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	return BT::NodeStatus::SUCCESS;
}

NodeStatus Polarizer4Recipe::MoveAbs(BT::TreeNode& node)
{
	Result res;

	QString position = getNodeValueByName(node, "position");

	res = ThorlabsMode::instance()->AbsMoveSync(position.toDouble());
	if (!res.success) {
		QString message = QString("Recipe Node [ Polarizer_MoveRes ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	return BT::NodeStatus::SUCCESS;
}

NodeStatus Polarizer4Recipe::MoveRes(BT::TreeNode& node)
{
	Result res;

	QString offset = getNodeValueByName(node, "offset");

	res = ThorlabsMode::instance()->RelativeMoveSync(offset.toDouble());
	if (!res.success) {
		QString message = QString("Recipe Node [ Polarizer_MoveRes ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	return BT::NodeStatus::SUCCESS;
}

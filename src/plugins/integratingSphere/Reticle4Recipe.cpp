#include "Reticle4Recipe.h"
#include "ReticleMode2D.h"
#include "loggingwrapper.h"
#include "ReticleMode.h"

using namespace Reticle;
Reticle4Recipe* Reticle4Recipe::self = nullptr;
Reticle4Recipe* Reticle4Recipe::getInstance() {

	if (!self)
	{
		self = new Reticle4Recipe();
	}
	return self;
}

Reticle4Recipe::Reticle4Recipe(QObject* parent)
	: QObject(parent)
{
	ObjectManager::getInstance()->registerObject("Reticle4Recipe", static_cast<void*>(this));
}

Reticle4Recipe::~Reticle4Recipe()
{
}

QString Reticle4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
{
	auto f_value = node.getInput<std::string>(name);
	if (!f_value)
	{
		throw BT::RuntimeError("Missing input [force]: ", f_value.error());
	}
	return QString::fromStdString(f_value.value());
}

NodeStatus Reticle4Recipe::Reticle2D_Connect()
{
	Result res = ReticleMode2D::instance()->connect();
	if (!res.success) {
		QString message = QString("Recipe Node [ Reticle_2D_Connect ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	return BT::NodeStatus::SUCCESS;
}

NodeStatus Reticle4Recipe::Reticle2D_Change(BT::TreeNode& node)
{
	Result res;
	
	QString type = getNodeValueByName(node, "type");
	QString reticle_name = getNodeValueByName(node, "reticle_name");
	if (type.contains("Small"))
	{
		res = ReticleMode2D::instance()->changeReticle(reticle_name,0);
	}
	else if (type.contains("Large"))
	{
		res = ReticleMode2D::instance()->changeReticle(reticle_name, 1);
	}
	else
	{
		QString message = QString("Recipe Node [ Reticle_2D_Change ] run error, Eye type mismatch, please check the input.");
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	if (!res.success) {
		QString message = QString("Recipe Node [ Reticle_2D_Change ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	return BT::NodeStatus::SUCCESS;
}

NodeStatus Reticle4Recipe::Reticle2D_MoveAbs(BT::TreeNode& node)
{
	QString coord_x = getNodeValueByName(node, "coord_x");
	QString coord_y = getNodeValueByName(node, "coord_y");

	Result res = ReticleMode2D::instance()->absMove({ coord_x.toFloat(), coord_y.toFloat() });
	if (!res.success) {
		QString message = QString("Recipe Node [ Reticle_2D_MoveAbs ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus Reticle::Reticle4Recipe::Reticle1D_Connect()
{
	Result res = ReticleMode::instance()->connect();
	if (!res.success) {
		QString message = QString("Recipe Node [ Reticle_1D_Connect ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	return BT::NodeStatus::SUCCESS;
}

NodeStatus Reticle::Reticle4Recipe::Reticle1D_Change(BT::TreeNode& node)
{
	Result res;
	QString reticle_name = getNodeValueByName(node, "reticle_name");
	res = ReticleMode::instance()->changeReticle(reticle_name);
	
	if (!res.success) {
		QString message = QString("Recipe Node [ Reticle_1D_Change ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	return BT::NodeStatus::SUCCESS;
}

NodeStatus Reticle::Reticle4Recipe::Reticle1D_MoveAbs(BT::TreeNode& node)
{
	QString coord_x = getNodeValueByName(node, "coord_x");

	Result res = ReticleMode::instance()->absMove(coord_x.toFloat());
	if (!res.success) {
		QString message = QString("Recipe Node [ Reticle_1D_MoveAbs ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

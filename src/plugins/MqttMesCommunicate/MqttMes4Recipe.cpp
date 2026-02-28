#include "MqttMes4Recipe.h"
#include "ObjectManager.h"
#include "MqttModel.h"

MqttMes4Recipe* MqttMes4Recipe::self = nullptr;
MqttMes4Recipe* MqttMes4Recipe::getInstance() {
	if (!self)
	{
		self = new MqttMes4Recipe();
	}
	return self;
}

MqttMes4Recipe::~MqttMes4Recipe()
{
}

MqttMes4Recipe::MqttMes4Recipe(QObject* parent)
	: QObject(parent)
{
	ObjectManager::getInstance()->registerObject("MqttMes4Recipe", static_cast<void*>(this));
}

QString MqttMes4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
{
	auto f_value = node.getInput<std::string>(name);
	if (!f_value)
	{
		throw BT::RuntimeError("Missing input [force]: ", f_value.error());
	}
	return QString::fromStdString(f_value.value());
}

NodeStatus MqttMes4Recipe::MES_Connect()
{
	MqttModel::getInstance()->connectMqttServer();
	return BT::NodeStatus::SUCCESS;
}

NodeStatus MqttMes4Recipe::MES_UploadCsv(BT::TreeNode& node)
{
	QString file_path = getNodeValueByName(node, "file_path");
	Result res = MqttModel::getInstance()->processSendMessage(file_path.toStdString());
	if (!res.success)
	{
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus MqttMes4Recipe::MES_UploadStatus(BT::TreeNode& node)
{
	QString status = getNodeValueByName(node, "status");
	int nStatus = status.toInt();
	Result res = MqttModel::getInstance()->processDeviceStatusMessage(nStatus);
	if (!res.success)
	{
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

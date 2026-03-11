#pragma once

#include <QObject>
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "ObjectManager.h"
#include "mqttmescommunicate_global.h"

using BT::NodeStatus;

class MQTTMESCOMMUNICATE_EXPORT  MqttMes4Recipe : public QObject
{
	Q_OBJECT
public:
	static  MqttMes4Recipe* getInstance();
	~MqttMes4Recipe();
	MqttMes4Recipe(QObject* parent = nullptr);
	QString getNodeValueByName(BT::TreeNode& node, std::string name);

	// ¨T¨T¨T¨T¨T¨T¨T MES Nodes ¨T¨T¨T¨T¨T¨T¨T¨T //
	NodeStatus MES_Connect();
	NodeStatus MES_UploadCsv(BT::TreeNode& node);
	NodeStatus MES_UploadStatus(BT::TreeNode& node);

private:
	static MqttMes4Recipe* self;
};

inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
{
	MqttMes4Recipe* obj = ((MqttMes4Recipe*)ObjectManager::getInstance()->getObject("MqttMes4Recipe"));
	if(!obj) {
		throw BT::RuntimeError("MqttMes4Recipe object not found !");
	}

	/*factory.registerSimpleAction(
		"MES_Connect",
		[=](BT::TreeNode& node)-> BT::NodeStatus
		{
			return obj->MES_Connect();
		}, {});

	factory.registerSimpleAction(
		"MES_UploadCsv",
		[=](BT::TreeNode& node)-> BT::NodeStatus
		{
			return obj->MES_UploadCsv(node);
		},
		{
			BT::InputPort<std::string>("file_path","string, e.g. D:/data.csv")
		});

	factory.registerSimpleAction(
		"MES_UploadStatus",
		[=](BT::TreeNode& node)-> BT::NodeStatus
		{
			return obj->MES_UploadStatus(node);
		}, 
		{
			BT::InputPort<std::string>("status","int, e.g. start->1/alarm->4/stop->10")
		});*/
}
#pragma once

#include <QObject>
#include "collimatedlighttube_global.h"
#include "ObjectManager.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "behaviortree_cpp_v3/behavior_tree.h"

using BT::NodeStatus;

class COLLIMATEDLIGHTTUBE_EXPORT CollimatedLightTube4Recipe : public QObject
{
	Q_OBJECT
public:
	static CollimatedLightTube4Recipe* getInstance();
	explicit CollimatedLightTube4Recipe(QObject* parent = nullptr);
	~CollimatedLightTube4Recipe();

	QString getNodeValueByName(BT::TreeNode& node, std::string name);

	NodeStatus Collimator_Connect();
	NodeStatus Collimator_Close();
	NodeStatus Collimator_SetExposure(BT::TreeNode& node);
	NodeStatus Collimator_SaveAngleToCsv(BT::TreeNode& node);

private:
	static CollimatedLightTube4Recipe* self;
};

inline void registernode(BT::BehaviorTreeFactory& factory)
{
	CollimatedLightTube4Recipe* obj = (CollimatedLightTube4Recipe*)ObjectManager::getInstance()->getObject("CollimatedLightTube4Recipe");
	if (!obj)
	{
		throw BT::RuntimeError("CollimatedLightTube4Recipe object not found !");
	}

	factory.registerSimpleAction(
		"Collimator_Connect",
		[=](BT::TreeNode& node)-> BT::NodeStatus
		{
			return obj->Collimator_Connect();
		},
		{});

	factory.registerSimpleAction(
		"Collimator_Close",
		[=](BT::TreeNode& node)-> BT::NodeStatus
		{
			return obj->Collimator_Close();
		});

	factory.registerSimpleAction(
		"Collimator_SetExposure",
		[=](BT::TreeNode& node)-> BT::NodeStatus
		{
			return obj->Collimator_SetExposure(node);
		},
		{
			BT::InputPort<std::string>("is_auto", "BOOL e.g. 0/1"),
			BT::InputPort<std::string>("exposure", "string")
		});

	factory.registerSimpleAction(
		"Collimator_SaveAngleToCsv",
		[=](BT::TreeNode& node)-> BT::NodeStatus
		{
			return obj->Collimator_SaveAngleToCsv(node);
		},
		{
			BT::InputPort<std::string>("file_name", "string")
		});
}

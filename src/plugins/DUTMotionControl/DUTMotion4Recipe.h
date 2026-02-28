#pragma once
#include <QObject>
#include "dutmotioncontrol_global.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "ObjectManager.h"

using BT::NodeStatus;

namespace DUTMotion {
	class DUTMOTIONCONTROL_EXPORT DUTMotion4Recipe : public QObject
	{
		Q_OBJECT
	public:
		static DUTMotion4Recipe* getInstance();
		~DUTMotion4Recipe();
		DUTMotion4Recipe(QObject* parent = nullptr);

		QString getNodeValueByName(BT::TreeNode& node, std::string name);
		
		NodeStatus OM_Open(BT::TreeNode& node);
		NodeStatus OM_Close(BT::TreeNode& node);
		NodeStatus OM_Degree_Async(BT::TreeNode& node);
		NodeStatus OM_Degree_Sync(BT::TreeNode& node);

	private:
		static DUTMotion4Recipe* self;
	};

	inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
	{
		DUTMotion4Recipe* obj = ((DUTMotion4Recipe*)ObjectManager::getInstance()->getObject("DUTMotion4Recipe"));
		if (!obj) {
			throw BT::RuntimeError("DUTMotion4Recipe object not found !");
		}
		/*连接电机*/
		factory.registerSimpleAction(
			"OM_Open",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->OM_Open(node);
			},
			{
				BT::InputPort<std::string>("module", "string, e.g. X/Y/Z")
			}
		);

		factory.registerSimpleAction(
			"OM_Close",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->OM_Close(node);
			},
			{
				BT::InputPort<std::string>("module", "string, e.g. X/Y/Z")
			}
		);

		factory.registerSimpleAction(
			"OM_DutTilt_Async",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->OM_Degree_Async(node);
			},
			{
				BT::InputPort<std::string>("module", "string, e.g. X/Y/Z"),
				BT::InputPort<std::string>("degree", "double")
			});

		factory.registerSimpleAction(
			"OM_DutTilt_Sync",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->OM_Degree_Sync(node);
			},
			{
				BT::InputPort<std::string>("module", "string, e.g. X/Y/Z"),
				BT::InputPort<std::string>("degree", "double")
			});
	}
}


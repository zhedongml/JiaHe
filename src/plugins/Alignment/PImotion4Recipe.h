#pragma once
#include <QObject>
#include "alignment_global.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "ObjectManager.h"

using BT::NodeStatus;

namespace PImotion
{
	class ALIGNMENT_EXPORT PImotion4Recipe : public QObject
	{
		Q_OBJECT

	public:
		static PImotion4Recipe* getInstance();
		~PImotion4Recipe();
		PImotion4Recipe(QObject* parent = nullptr);
		QString getNodeValueByName(BT::TreeNode& node, std::string name);


		// ¨T¨T¨T¨T¨T¨T¨T PImotion Nodes¨T¨T¨T¨T¨T¨T¨T¨T //

		//Absolute Move
		NodeStatus PI_2D_AbsProTilt(BT::TreeNode& node);
		NodeStatus PI_2D_AbsMeasureCameraTilt(BT::TreeNode& node);
		NodeStatus PI_3D_AbsDutMove(BT::TreeNode& node);
		NodeStatus PI_3D_AbsMeasureCameraMove(BT::TreeNode& node);

	private:
		static PImotion4Recipe* self;
	};

	inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
	{
		PImotion4Recipe* obj = ((PImotion4Recipe*)ObjectManager::getInstance()->getObject("PImotion4Recipe"));
		if (!obj) {
			throw BT::RuntimeError("PImotion4Recipe object not found !");
		}
		factory.registerSimpleAction(
			"PI_2D_AbsProTilt",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->PI_2D_AbsProTilt(node);
			},
			{
				BT::InputPort<std::string>("x_degree","double"),
				BT::InputPort<std::string>("y_degree","double")
			});

		factory.registerSimpleAction(
			"PI_2D_AbsMeasureCameraTilt",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->PI_2D_AbsMeasureCameraTilt(node);
			},
			{
				BT::InputPort<std::string>("x_degree","double"),
				BT::InputPort<std::string>("y_degree","double")
			});

		factory.registerSimpleAction(
			"PI_3D_AbsDutMove",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->PI_3D_AbsDutMove(node);
			},
			{
				BT::InputPort<std::string>("x_position","double"),
				BT::InputPort<std::string>("y_position","double"),
				BT::InputPort<std::string>("z_position","double")
			});
		factory.registerSimpleAction(
			"PI_3D_AbsMeasureCameraMove",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->PI_3D_AbsMeasureCameraMove(node);
			},
			{
				BT::InputPort<std::string>("x_position","double"),
				BT::InputPort<std::string>("y_position","double"),
				BT::InputPort<std::string>("z_position","double")
			});
	}
}




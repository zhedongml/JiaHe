#pragma once
#include <QObject>
#include "polarizercontrol_global.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "ObjectManager.h"

using BT::NodeStatus;

namespace Polarizer
{
	class POLARIZERCONTROL_EXPORT Polarizer4Recipe : public QObject
	{
		Q_OBJECT

	public:
		static  Polarizer4Recipe* getInstance();
		~Polarizer4Recipe();
		Polarizer4Recipe(QObject* parent = nullptr);
		QString getNodeValueByName(BT::TreeNode& node, std::string name);


		// ¨T¨T¨T¨T¨T¨T¨T Reticle Nodes¨T¨T¨T¨T¨T¨T¨T¨T //
		NodeStatus Connect();
		NodeStatus MoveAbs(BT::TreeNode& node);
		NodeStatus MoveRes(BT::TreeNode& node);

	private:
		static Polarizer4Recipe* self;
	};

	inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
	{
		Polarizer4Recipe* obj = ((Polarizer4Recipe*)ObjectManager::getInstance()->getObject("Polarizer4Recipe"));
		if (!obj) {
			throw BT::RuntimeError("Polarizer4Recipe object not found !");
		}
		factory.registerSimpleAction(
			"Polarizer_Connect",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->Connect();
			}, {});

		factory.registerSimpleAction(
			"Polarizer_MoveAbs",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->MoveAbs(node);
			},
			{
				BT::InputPort<std::string>("position","double"),
			});

		factory.registerSimpleAction(
			"Polarizer_MoveRes",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->MoveRes(node);
			},
			{
				BT::InputPort<std::string>("offset","double"),
			});

	}
}


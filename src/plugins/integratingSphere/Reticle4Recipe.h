#pragma once
#include <QObject>
#include "integratingsphere_global.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "ObjectManager.h"

using BT::NodeStatus;

namespace Reticle
{
	class INTEGRATINGSPHERE_EXPORT Reticle4Recipe : public QObject
	{
		Q_OBJECT

	public:
		static  Reticle4Recipe* getInstance();
		~Reticle4Recipe();
		Reticle4Recipe(QObject* parent = nullptr);
		QString getNodeValueByName(BT::TreeNode& node, std::string name);


		// ¨T¨T¨T¨T¨T¨T¨T Reticle Nodes¨T¨T¨T¨T¨T¨T¨T¨T //
		NodeStatus Connect();
		NodeStatus ReticleChange(BT::TreeNode& node);
		NodeStatus ReticleMoveAbs(BT::TreeNode& node);

	private:
		static Reticle4Recipe* self;
	};

	inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
	{
		Reticle4Recipe* obj = ((Reticle4Recipe*)ObjectManager::getInstance()->getObject("Reticle4Recipe"));
		if (!obj) {
			throw BT::RuntimeError("Reticle4Recipe object not found !");
		}
		factory.registerSimpleAction(
			"Reticle_2D_Connect",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->Connect();
			}, {});

		factory.registerSimpleAction(
			"Reticle_2D_Change",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->ReticleChange(node);
			}, 
			{
				BT::InputPort<std::string>("type","string, e.g. Small/Large"),
				BT::InputPort<std::string>("reticle_name","string, e.g. Cross/Clear/Dark/Flare/Ghost/Grid/Checker0.5deg/PositiveChecker/NegativeChecker/PositiveCheckerSmall/NegativeCheckerSmall"),
			});

		factory.registerSimpleAction(
			"Reticle_2D_MoveAbs",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->ReticleMoveAbs(node);
			},
			{
				BT::InputPort<std::string>("coord_x","float"),
				BT::InputPort<std::string>("coord_y","float"),
			});
	}
}


#pragma once
#include <QObject>
#include "integratingsphere_global.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "ObjectManager.h"

using BT::NodeStatus;

namespace IntegratingSphere
{
	class INTEGRATINGSPHERE_EXPORT IntegratingSphere4Recipe : public QObject
	{
		Q_OBJECT

	public:
		static  IntegratingSphere4Recipe* getInstance();
		~IntegratingSphere4Recipe();
		IntegratingSphere4Recipe(QObject* parent = nullptr);
		QString getNodeValueByName(BT::TreeNode& node, std::string name);

		// ¨T¨T¨T¨T¨T¨T¨T IntegratingSphere Nodes¨T¨T¨T¨T¨T¨T¨T¨T //
		NodeStatus Connect();
		NodeStatus Disconnect();
		NodeStatus SetSolution(BT::TreeNode& node);

		NodeStatus SetCurLuminance(BT::TreeNode& node);

	private:
		static IntegratingSphere4Recipe* self;
	};

	inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
	{
		IntegratingSphere4Recipe* obj = ((IntegratingSphere4Recipe*)ObjectManager::getInstance()->getObject("IntegratingSphere4Recipe"));
		if (!obj) {
			throw BT::RuntimeError("IntegratingSphere4Recipe object not found !");
		}
		factory.registerSimpleAction(
			"IS_Connect",
			[=](BT::TreeNode& node)-> BT::NodeStatus 
			{
				return obj->Connect();
			},{});

		factory.registerSimpleAction(
			"IS_Disconnect",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->Disconnect();
			}, {});

		factory.registerSimpleAction(
			"IS_SetSolution",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->SetSolution(node);
			},
			{
				BT::InputPort<std::string>("light_source","string, e.g. R/G/B/W"),
				BT::InputPort<std::string>("isphere_scale","string list, e.g. 0.18,0,0"),
			});
		//factory.registerSimpleAction(
		//	"IS_SetCurLuminance",
		//	[=](BT::TreeNode& node)-> BT::NodeStatus
		//	{
		//		return obj->SetCurLuminance(node);
		//	},
		//	{
		//		BT::InputPort<std::string>("color_name","char, e.g. R/G/B/W"),
		//		BT::InputPort<std::string>("lumin","double")

		//	});
		
		//factory.registerSimpleAction("Close", std::bind(&TipTilt::close, (TipTilt*)ObjectManager2::instance().getObject<MLMotion2D>("TipTilt1")));
	}
}


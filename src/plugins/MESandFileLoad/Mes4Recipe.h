#pragma once
#include <QObject>
#include "mesandfileload_global.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "ObjectManager.h"

using BT::NodeStatus;

namespace MesNS
{
	class MESANDFILELOAD_EXPORT Mes4Recipe : public QObject
	{
		Q_OBJECT

	public:
		static  Mes4Recipe* getInstance();
		~Mes4Recipe();
		Mes4Recipe(QObject* parent = nullptr);
		QString getNodeValueByName(BT::TreeNode& node, std::string name);

		NodeStatus Mes_Lens_Load(BT::TreeNode& node);
		NodeStatus Mes_SubmitAutoDPTask(BT::TreeNode& node);
		NodeStatus Mes_SubmitAdpTaskNew(BT::TreeNode& node);
		NodeStatus Mes_MetricsLimitAutoDP(BT::TreeNode& node);
		NodeStatus Mes_Test_Node(BT::TreeNode& node);
		//NodeStatus Mes_AutoDPResultPath(BT::TreeNode& node);

	private:
		static Mes4Recipe* self;
	};

	inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
	{
		Mes4Recipe* obj = ((Mes4Recipe*)ObjectManager::getInstance()->getObject("Mes4Recipe"));
		if (!obj) {
			throw BT::RuntimeError("Mes4Recipe object not found !");
		}

		factory.registerSimpleAction(
			"MES_Lens_Load",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->Mes_Lens_Load(node);
			},
			{
				BT::InputPort<std::string>("dut_sn","string")
			});

		factory.registerSimpleAction(
			"MES_SubmitAutoDPTask",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->Mes_SubmitAutoDPTask(node);
			},
			{
				//BT::InputPort<std::string>("sn","string"),
				BT::InputPort<std::string>("sourceImageDir","int"),
				BT::InputPort<std::string>("resultSaveDir","bool, e.g. 0,1")
			});

		factory.registerSimpleAction(
			"MES_SubmitAdpTaskNew",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->Mes_SubmitAdpTaskNew(node);
			},
			{
				//BT::InputPort<std::string>("sn","string"),
				//BT::InputPort<std::string>("resultSaveDir","bool, e.g. 0,1")
				//BT::InputPort<std::string>("checkEnabled","bool, e.g. 0,1"),
				//BT::InputPort<std::string>("modelName","string"),
				//BT::InputPort<std::string>("liveRefresh","bool, e.g. 0,1")
			});

		factory.registerSimpleAction(
			"MES_MetricsLimitAutoDP",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->Mes_MetricsLimitAutoDP(node);
			},
			{
				BT::InputPort<std::string>("checkEnabled","bool, e.g. 0,1"),
				BT::InputPort<std::string>("modelName","string"),
				BT::InputPort<std::string>("liveRefresh","bool, e.g. 0,1")
			});

		factory.registerSimpleAction(
			"MES_Test_Node",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->Mes_Test_Node(node);
			},
			{});
	}
}




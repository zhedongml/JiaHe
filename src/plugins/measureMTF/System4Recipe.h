#pragma once
#include <QObject>
#include "measuremtf_global.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "ObjectManager.h"

using BT::NodeStatus;

namespace System
{
	class MEASUREMTF_EXPORT System4Recipe : public QObject
	{
		Q_OBJECT

	public:
		static  System4Recipe* getInstance();
		~System4Recipe();
		System4Recipe(QObject* parent = nullptr);
		QString getNodeValueByName(BT::TreeNode& node, std::string name);


		// ¨T¨T¨T¨T¨T¨T¨T System Nodes¨T¨T¨T¨T¨T¨T¨T¨T //
		NodeStatus System_Sleep(BT::TreeNode& node);
		NodeStatus System_Date(BT::TreeNode& node);
		NodeStatus System_DateTime(BT::TreeNode& node);
		NodeStatus System_InitValue(BT::TreeNode& node);
		NodeStatus System_GetForNodeValue1(BT::TreeNode& node);
		NodeStatus System_GetForNodeValue2(BT::TreeNode& node);
		NodeStatus System_GetForNodeValue3(BT::TreeNode& node);
		NodeStatus System_CatStr2(BT::TreeNode& node);
		NodeStatus System_CatStr3(BT::TreeNode& node);
		NodeStatus System_CatStr4(BT::TreeNode& node);
		NodeStatus System_ParseStr3(BT::TreeNode& node);
		NodeStatus System_Split(BT::TreeNode& node);
		NodeStatus System_Tips(BT::TreeNode& node);
		NodeStatus System_InputTips(BT::TreeNode& node);
		NodeStatus System_Subfolders(BT::TreeNode& node);
		NodeStatus System_SequenceConversion(BT::TreeNode& node);
		NodeStatus System_CompareStr(BT::TreeNode& node);


	private:
		static System4Recipe* self;
	};

	inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
	{
		System4Recipe* obj = ((System4Recipe*)ObjectManager::getInstance()->getObject("System4Recipe"));
		if (!obj) {
			throw BT::RuntimeError("System4Recipe object not found !");
		}
		factory.registerSimpleAction(
			"System_Sleep",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_Sleep(node);
			}, 
			{
				BT::InputPort<std::string>("sleep_time","int, unit:s"),
			});

		factory.registerSimpleAction(
			"System_Date",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_Date(node);
			},
			{
				BT::OutputPort <std::string> ("date_key")
			
			});

		factory.registerSimpleAction(
			"System_DateTime",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_DateTime(node);
			},
			{
				BT::OutputPort<std::string>("date_time_key")

			});

		//factory.registerSimpleAction(
		//	"System_InitValue",
		//	[=](BT::TreeNode& node)-> BT::NodeStatus
		//	{
		//		return obj->System_InitValue(node);
		//	},
		//	{
		//		BT::InputPort<std::string>("value"),
		//		BT::OutputPort<std::string>("value_key")

		//	});

		factory.registerSimpleAction(
			"System_GetForNodeValue1",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_GetForNodeValue1(node);
			},
			{
				BT::OutputPort<std::string>("value_key")
			});

		factory.registerSimpleAction(
			"System_GetForNodeValue2",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_GetForNodeValue2(node);
			},
			{
				BT::OutputPort<std::string>("value_key1"),
				BT::OutputPort<std::string>("value_key2")

			});

		factory.registerSimpleAction(
			"System_GetForNodeValue3",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_GetForNodeValue3(node);
			},
			{
				BT::OutputPort<std::string>("value_key1"),
				BT::OutputPort<std::string>("value_key2"),
				BT::OutputPort<std::string>("value_key3"),
			});
		
		/*factory.registerSimpleAction(
			"System_CatStr2",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_CatStr2(node);
			},
			{
				BT::InputPort<std::string>("str1"),
				BT::InputPort<std::string>("str2"),

				BT::OutputPort<std::string>("value_key")
			});

		factory.registerSimpleAction(
			"System_CatStr3",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_CatStr3(node);
			},
			{
				BT::InputPort<std::string>("str1"),
				BT::InputPort<std::string>("str2"),
				BT::InputPort<std::string>("str3"),
				BT::OutputPort<std::string>("value_key")
			});
		
		factory.registerSimpleAction(
			"System_CatStr4",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_CatStr4(node);
			},
			{
				BT::InputPort<std::string>("str1"),
				BT::InputPort<std::string>("str2"),
				BT::InputPort<std::string>("str3"),
				BT::InputPort<std::string>("str4"),
				BT::OutputPort<std::string>("value_key")
			});*/

		factory.registerSimpleAction(
			"System_ParseStr3",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_ParseStr3(node);
			},
			{
				BT::InputPort<std::string>("str"),
				BT::OutputPort<std::string>("value_key1"),
				BT::OutputPort<std::string>("value_key2"),
				BT::OutputPort<std::string>("value_key3")
			}); 

		factory.registerSimpleAction(
			"System_Tips",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_Tips(node);
			},
			{
				BT::InputPort<std::string>("tip")
			});

		factory.registerSimpleAction(
			"System_InputTips",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_InputTips(node);
			},
			{
				BT::InputPort<std::string>("tip"),
				BT::InputPort<std::string>("param"),
				BT::InputPort<std::string>("is_lable_visual"),
				BT::OutputPort<std::string>("text_key")
			});

		factory.registerSimpleAction(
			"System_Subfolders",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_Subfolders(node);
			},
			{
				BT::InputPort<std::string>("parent_dir","string"),
				BT::OutputPort<std::string>("subfolders_key","string")

			});

		factory.registerSimpleAction(
			"System_Split",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_Split(node);
			},
			{
				BT::InputPort<std::string>("input_str","string"),
				BT::InputPort<std::string>("separator","string, e.g. ,/;/_/#"),
				BT::InputPort<std::string>("pos_for_return","int,start from index 0"),
				BT::OutputPort<std::string>("split_result_key","string")
			});

		factory.registerSimpleAction(
			"System_SequenceConversion",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_SequenceConversion(node);
			},
			{
				BT::InputPort<std::string>("cycles_num","string"),
				BT::OutputPort<std::string>("value_key","string")
			});

		factory.registerSimpleAction(
			"System_CompareStr",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->System_CompareStr(node);
			},
			{
				BT::InputPort<std::string>("origin_str","string"),
				BT::InputPort<std::string>("target_str","string")
			});
		
	}
}



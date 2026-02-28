#pragma once
#include <QObject>
#include "decoder_global.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "ObjectManager.h"

using BT::NodeStatus;

namespace DecoderSN
{
	class DECODER_EXPORT Decoder4Recipe : public QObject
	{
		Q_OBJECT

	public:
		static  Decoder4Recipe* getInstance();
		~Decoder4Recipe();
		Decoder4Recipe(QObject* parent = nullptr);
		QString getNodeValueByName(BT::TreeNode& node, std::string name);


		// ═══════ Decoder Nodes════════ //
		NodeStatus Decoder_Connect();
		NodeStatus Decoder_SN(BT::TreeNode& node);

	private:
		static Decoder4Recipe* self;
	};

	inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
	{
		Decoder4Recipe* obj = ((Decoder4Recipe*)ObjectManager::getInstance()->getObject("Decoder4Recipe"));
		if (!obj) {
			throw BT::RuntimeError("Decoder4Recipe object not found !");
		}
		factory.registerSimpleAction(
			"Decoder_Connect",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->Decoder_Connect();
			},
			{});

		factory.registerSimpleAction(
			"Decoder_SN",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->Decoder_SN(node);
			}, 
            { 
				BT::OutputPort<std::string>("key")
			});
	}
}



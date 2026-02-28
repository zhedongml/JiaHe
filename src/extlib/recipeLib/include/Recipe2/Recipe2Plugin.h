/*************************************************
Author:jason.huang
Date:2025.4.29
Version:0.0.1
Description:Drag and drop automation tool
**************************************************/

#ifndef RECIPE2_PLUGIN
#define RECIPE2_PLUGIN
#include "recipe2_global.h"
#include <QWidget>
//#include <tuple>
//#include <utility>
//#include "behaviortree_cpp_v3/bt_factory.h"

//using namespace BT;

//完全泛型版本，有参绑定，方式3
//从 BT 节点中获取一个输入端口的值，并存入变量 out,如果端口不存在或者类型不对，会抛出异常
//template <typename ArgType>
//void extractSingleInput(BT::TreeNode& node, const std::string& port_name, ArgType& out)
//{
//	if (!node.getInput<ArgType>(port_name, out))
//	{
//		throw BT::RuntimeError("Missing or invalid input port [" + port_name + "]");
//	}
//}

//这是展开多个参数的模板函数,它依次从节点中读取多个输入端口，并将结果写入 std::tuple<TArgs...>
//template <typename... TArgs>
//void extractInputs(BT::TreeNode& node, const std::vector<std::string>& port_names,
//	std::tuple<TArgs...>& args)
//{
//	size_t index = 0;
//	(..., extractSingleInput<TArgs>(node, port_names[index++], std::get<TArgs>(args)));
//}

//提供给外部的绑定函数，构造一个 Lambda，返回给 registerSimpleAction
//创建一个 tuple：用于保存参数,用 extractInputs 填充 tuple（从端口读取参数）,用 std::apply 把 tuple 拆开并调用你的函数 (instance->*method)(...)
//template <typename TClass, typename... TArgs>
//auto bindWithInputs(TClass* instance, BT::NodeStatus(TClass::* method)(TArgs...),
//	const std::vector<std::string>& port_names)
//{
//	return [=](BT::TreeNode& node) -> BT::NodeStatus {
//		std::tuple<TArgs...> args;
//		extractInputs(node, port_names, args);
//		return std::apply(
//			[&](auto&&... unpackedArgs) { return (instance->*method)(unpackedArgs...); },
//			args);
//	};
//}

//无参绑定，模板化，方式3
//template <typename TClass>
//auto bindWithoutInput(TClass* instance, BT::NodeStatus(TClass::* method)())
//{
//	return [=](BT::TreeNode&) -> BT::NodeStatus { return (instance->*method)(); };
//}

class RECIPE2_EXPORT Recipe2Plugin
{
public:
	virtual ~Recipe2Plugin() {}
	virtual QWidget* GetWidget() = 0;

	//Binding member functions to behavior tree nodes
	/*virtual void NodeBinding(const std::string& ID,
		const SimpleActionNode::TickFunctor& tick_functor,
		PortsList ports = {}) = 0;*/

		// load custom node
	virtual void LoadCustomNode(std::string nodePath) = 0;
	//Engineer and Operator Modes
	virtual void IsSwitchOperatorMode(bool isSwitchOperatorMode) = 0;
	//switch tree ui or xml ui
	virtual void SwitchTreeOrXmlUI(int Tree_XmlUIMode) = 0;
	//zoom and center view
	virtual void ZoomAndCenterView() = 0;
	//set Horizontal and vertical layout
	virtual void HorizontalandVerticalLayout() = 0;
	virtual void Reorder() = 0;
};
//Plug-in identifier
#define AbstractInterface_iid "Recipe2 plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(Recipe2Plugin, AbstractInterface_iid)

#endif
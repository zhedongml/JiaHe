/* Copyright (C) 2015-2018 Michele Colledanchise -  All Rights Reserved
*  Copyright (C) 2018-2020 Davide Faconti, Eurecat -  All Rights Reserved
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
*   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
*   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <condition_variable>
#include <mutex>
#include <map>

#include "behaviortree_cpp_v3/utils/signal.h"
#include "behaviortree_cpp_v3/exceptions.h"
#include "behaviortree_cpp_v3/basic_types.h"
#include "behaviortree_cpp_v3/blackboard.h"
#include "behaviortree_cpp_v3/utils/strcat.hpp"
#include "behaviortree_cpp_v3/utils/wakeup_signal.hpp"

#ifdef _MSC_VER
#pragma warning(disable : 4127)
#endif

namespace BT
{
class ParamCallback
{
public:
  //循环次数回调
  virtual void NotifyNumberOfCycles(std::string registration_ID, int uid, int times) = 0;
  //错误信息回调
  virtual void NotifyErrorMsg(std::string errorMsg) = 0;
  //参数遍历回调
  virtual void NotifyParameterTraversal(std::string registration_ID, int uid,
                                        std::string param) = 0;
  //日志回调
  virtual void NotifyLogPrint(std::string logMsg) = 0;
};

typedef void (*StopSignalCallBack)(bool isStopTreeSystem);

/// This information is used mostly by the XMLParser.
struct TreeNodeManifest
{
  NodeType type;
  std::string registration_ID;
  PortsList ports;
  std::string description;
};

typedef std::unordered_map<std::string, std::string> PortsRemapping;

struct NodeConfiguration
{
  NodeConfiguration()
  {}

  Blackboard::Ptr blackboard;
  PortsRemapping input_ports;
  PortsRemapping output_ports;
};

/// Abstract base class for Behavior Tree Nodes
class TreeNode
{
public:
  typedef std::shared_ptr<TreeNode> Ptr;

  /**
     * @brief TreeNode main constructor.
     *
     * @param name     name of the instance, not the type.
     * @param config   information about input/output ports. See NodeConfiguration
     *
     * Note: If your custom node has ports, the derived class must implement:
     *
     *     static PortsList providedPorts();
     */
  TreeNode(std::string name, NodeConfiguration config);

  virtual ~TreeNode() = default;

  /// The method that should be used to invoke tick() and setStatus();
  virtual BT::NodeStatus executeTick();

  /// The method used to interrupt the execution of a RUNNING node.
  /// Only Async nodes that may return RUNNING should implement it.
  virtual void halt() = 0;

  bool isHalted() const;

  NodeStatus status() const;

  /// Name of the instance, not the type
  const std::string& name() const;

  /// Blocking function that will sleep until the setStatus() is called with
  /// either RUNNING, FAILURE or SUCCESS.
  BT::NodeStatus waitValidStatus();

  virtual NodeType type() const = 0;

  using StatusChangeSignal = Signal<TimePoint, const TreeNode&, NodeStatus, NodeStatus>;
  using StatusChangeSubscriber = StatusChangeSignal::Subscriber;
  using StatusChangeCallback = StatusChangeSignal::CallableFunction;

  using PreTickOverrideCallback =
      std::function<Optional<NodeStatus>(TreeNode&, NodeStatus)>;
  using PostTickOverrideCallback =
      std::function<Optional<NodeStatus>(TreeNode&, NodeStatus, NodeStatus)>;

  /**
     * @brief subscribeToStatusChange is used to attach a callback to a status change.
     * When StatusChangeSubscriber goes out of scope (it is a shared_ptr) the callback
     * is unsubscribed automatically.
     *
     * @param callback The callback to be execute when status change.
     *
     * @return the subscriber handle.
     */
  StatusChangeSubscriber subscribeToStatusChange(StatusChangeCallback callback);

  /** This method attaches to the TreeNode a callback with signature:
     *
     *     Optional<NodeStatus> myCallback(TreeNode& node, NodeStatus current_status)
     *
     * This callback is executed BEFORE the tick() and, if it returns a valid Optional<NodeStatus>,
     * the actual tick() will NOT be executed and this result will be returned instead.
     *
     * This is useful to inject a "dummy" implementation of the TreeNode at run-time
     */
  void setPreTickOverrideFunction(PreTickOverrideCallback callback);

  /**
     * This method attaches to the TreeNode a callback with signature:
     *
     *     Optional<NodeStatus> myCallback(TreeNode& node, NodeStatus prev_status, NodeStatus tick_status)
     *
     * This callback is executed AFTER the tick() and, if it returns a valid Optional<NodeStatus>,
     * the value returned by the actual tick() is overriden with this one.
     */
  void setPostTickOverrideFunction(PostTickOverrideCallback callback);

  // get an unique identifier of this instance of treeNode
  uint16_t UID() const;

  /// registrationName is the ID used by BehaviorTreeFactory to create an instance.
  const std::string& registrationName() const;

  /// Configuration passed at construction time. Can never change after the
  /// creation of the TreeNode instance.
  const NodeConfiguration& config() const;

  /** Read an input port, which, in practice, is an entry in the blackboard.
     * If the blackboard contains a std::string and T is not a string,
     * convertFromString<T>() is used automatically to parse the text.
     *
     * @param key   the identifier (before remapping) of the port.
     * @return      false if an error occurs.
     */
  template <typename T>
  Result getInput(const std::string& key, T& destination) const;

  /** Same as bool getInput(const std::string& key, T& destination)
     * but using optional.
     */
  template <typename T>
  Optional<T> getInput(const std::string& key) const
  {
    T out;
    auto res = getInput(key, out);
    return (res) ? Optional<T>(out) : nonstd::make_unexpected(res.error());
  }

  template <typename T>
  Result setOutput(const std::string& key, const T& value);

  // function provide mostrly for debugging purpose to see the raw value
  // in the port (no remapping and no conversion to a type)
  StringView getRawPortValue(const std::string& key) const;

  /// Check a string and return true if it matches either one of these
  /// two patterns:  {...} or ${...}
  //判断给定的 str 是否是一个有效的“黑板指针”字符串,即是否形如 {xxx} 或 ${xxx}
  static bool isBlackboardPointer(StringView str);

  //提取黑板指针中实际的变量名部分，即去除 {} 或 ${} 包裹
  static StringView stripBlackboardPointer(StringView str);

  static Optional<StringView> getRemappedKey(StringView port_name,
                                             StringView remapping_value);

  // Notify the tree should be ticked again()
  void emitStateChanged();

  void StopChildNodes(bool isStopNodes);

  bool IsStopChildNodes();

  //注册参数回调
  void RegisterParamCallback(ParamCallback* paramCallback);
  ParamCallback* GetParamCallback();

  //注册停止信号回调
  void RegisterStopSignalCallback(StopSignalCallBack stopSignalCallback);

  //输入11{22}33，输出11输出端口字符串33，如果有{}嵌套或者不完整的{}，则输出原字符串
  template <typename T>
  Result StringPairingReplacement(const std::string& key, const std::string& input,
                                  T& destination) const;

  //注册异步节点时自动设置节点信息
  void setNodeAsynInfo(int uid, void* treeNode);
  std::map<int, void*> getNodeAsynInfo();
  void ClearNodeAsynInfo();

  //Set the asyn nodes status to IDLE
  void resetAsynStatus();

  //all tree is run done
  void setTreeNodeDone(bool isDone);
  bool isTreeNodeDone();

  void SendNodePrintLog(bool isStart);

protected:
  /// Method to be implemented by the user
  virtual BT::NodeStatus tick() = 0;

  friend class BehaviorTreeFactory;
  friend class DecoratorNode;
  friend class ControlNode;
  friend class Tree;

  // Only BehaviorTreeFactory should call this
  void setRegistrationID(StringView ID);

  void setWakeUpInstance(std::shared_ptr<WakeUpSignal> instance);

  void modifyPortsRemapping(const PortsRemapping& new_remapping);

  void setStatus(NodeStatus new_status);

private:
  const std::string name_;

  NodeStatus status_;

  std::condition_variable state_condition_variable_;

  mutable std::mutex state_mutex_;

  StatusChangeSignal state_change_signal_;

  const uint16_t uid_;

  NodeConfiguration config_;

  std::string registration_ID_;

  PreTickOverrideCallback pre_condition_callback_;

  PostTickOverrideCallback post_condition_callback_;

  std::shared_ptr<WakeUpSignal> wake_up_;

  /// Set the status to IDLE
  void resetStatus();

  bool isStop_children_nodes_;

  ParamCallback* _paramCallback = nullptr;
  StopSignalCallBack _stopSignalCallBack = nullptr;
};

//-------------------------------------------------------
template <typename T>
inline Result TreeNode::getInput(const std::string& key, T& destination) const
{
  auto remap_it = config_.input_ports.find(key);
  if (remap_it == config_.input_ports.end())
  {
    return nonstd::make_unexpected(StrCat("getInput() failed because "
                                          "NodeConfiguration::input_ports "
                                          "does not contain the key: [",
                                          key, "]"));
  }
  //if string only is {xxx}，remapped_res retrun true and the internal string of {}
  //else，remapped_res retrun false and "Not a blackboard pointer"
  auto remapped_res = getRemappedKey(key, remap_it->second);
  try
  {
    if (!remapped_res)
    {
      //out put string or xxx_ {xxx}_xxx
      return StringPairingReplacement<T>(key, remap_it->second, destination);

      /*destination = convertFromString<T>(remap_it->second);
      return {};*/
    }
    const auto& remapped_key = remapped_res.value();

    if (!config_.blackboard)
    {
      return nonstd::make_unexpected("getInput() trying to access a Blackboard(BB) "
                                     "entry, "
                                     "but BB is invalid");
    }

    std::unique_lock<std::mutex> entry_lock(config_.blackboard->entryMutex());
    const Any* val = config_.blackboard->getAny(static_cast<std::string>(remapped_key));
    if (val && val->empty() == false)
    {
      //Val type is always string
      /* if (val->type() == typeid(std::string))
      {
        std::string aa = val->cast<std::string>();
        int a = 0;
      }*/

      if (std::is_same<T, std::string>::value == false &&
          val->type() == typeid(std::string))
      {
        destination = convertFromString<T>(val->cast<std::string>());
      }
      else
      {
        destination = val->cast<T>();
      }
      return {};
    }

    return nonstd::make_unexpected(StrCat("getInput() failed because it was unable to "
                                          "find the "
                                          "key [",
                                          key, "] remapped to [", remapped_key, "]"));
  }
  catch (std::exception& err)
  {
    return nonstd::make_unexpected(err.what());
  }
}

template <typename T>
inline Result TreeNode::setOutput(const std::string& key, const T& value)
{
  if (!config_.blackboard)
  {
    return nonstd::make_unexpected("setOutput() failed: trying to access a "
                                   "Blackboard(BB) entry, but BB is invalid");
  }

  auto remap_it = config_.output_ports.find(key);
  if (remap_it == config_.output_ports.end())
  {
    return nonstd::make_unexpected(StrCat("setOutput() failed: "
                                          "NodeConfiguration::output_ports "
                                          "does not "
                                          "contain the key: [",
                                          key, "]"));
  }
  StringView remapped_key = remap_it->second;
  if (remapped_key == "=")
  {
    remapped_key = key;
  }
  if (isBlackboardPointer(remapped_key))
  {
    remapped_key = stripBlackboardPointer(remapped_key);
  }
  config_.blackboard->set(static_cast<std::string>(remapped_key), value);

  return {};
}

// Utility function to fill the list of ports using T::providedPorts();
template <typename T>
inline void assignDefaultRemapping(NodeConfiguration& config)
{
  for (const auto& it : getProvidedPorts<T>())
  {
    const auto& port_name = it.first;
    const auto direction = it.second.direction();
    if (direction != PortDirection::OUTPUT)
    {
      config.input_ports[port_name] = "=";
    }
    if (direction != PortDirection::INPUT)
    {
      config.output_ports[port_name] = "=";
    }
  }
}

template <typename T>
inline Result TreeNode::StringPairingReplacement(const std::string& key,
                                                 const std::string& input,
                                                 T& destination) const
{
  //{}is bad

  int depth = 0;
  bool hasBrace = false;
  for (char ch : input)
  {
    if (ch == '{')
    {
      if (depth > 0)
      {
        destination = convertFromString<T>(input);   // 嵌套
        return {};
      }
      depth++;
      hasBrace = true;
    }
    else if (ch == '}')
    {
      if (depth == 0)
      {
        destination = convertFromString<T>(input);   // 多余的右括号
        return {};
      }
      depth--;
    }
  }
  if (depth != 0 || !hasBrace)
  {
    destination = convertFromString<T>(input);   // {}不成对 或 没有{}
    return {};
  }

  //{}is ok
  //Start replacing {xxx}
  std::string result;
  size_t i = 0;
  while (i < input.size())
  {
    if (input[i] == '{')
    {
      size_t end = input.find('}', i);
      std::string tempVar = input.substr(i, end - i + 1);   // include{}
      auto remapped_res = getRemappedKey(
          key, tempVar);   //If there is {}, it usually does not return false
      if (!remapped_res)
        return nonstd::make_unexpected(StrCat("getInput() failed because it was unable "
                                              "to "
                                              "find the "
                                              "key [",
                                              key, "] remapped to [",
                                              remapped_res.value(), "]"));

      const auto& remapped_key = remapped_res.value();
      if (!config_.blackboard)
      {
        return nonstd::make_unexpected("getInput() trying to access a Blackboard(BB) "
                                       "entry, "
                                       "but BB is invalid");
      }
      std::unique_lock<std::mutex> entry_lock(config_.blackboard->entryMutex());
      const Any* val = config_.blackboard->getAny(static_cast<std::string>(remapped_key));
      if (val && val->empty() == false)
      {
        /*if (std::is_same<T, std::string>::value == false &&
            val->type() == typeid(std::string))
        {
          result += convertFromString<T>(val->cast<std::string>());
        }
        else
        {
          result += val->cast<T>();
        }*/

        result += val->cast<std::string>();
        i = end + 1;
      }
      else
        return nonstd::make_unexpected(StrCat("getInput() failed because it was unable "
                                              "to "
                                              "find the "
                                              "key [",
                                              key, "] remapped to [", remapped_key, "]"));
    }
    else
    {
      result += input[i++];
    }
  }

  destination = convertFromString<T>(result);
  return {};
}
}   // namespace BT

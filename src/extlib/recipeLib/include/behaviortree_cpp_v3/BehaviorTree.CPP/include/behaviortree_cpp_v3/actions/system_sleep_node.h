/*  Copyright (C) 2018-2022 Davide Faconti, Eurecat -  All Rights Reserved
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

#include "behaviortree_cpp_v3/action_node.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace BT
{
/**
 * System pause, in seconds
 */

class SystemSleepNode : public SyncActionNode
{
public:
  SystemSleepNode(const std::string& name, const NodeConfiguration& config) :
    SyncActionNode(name, config)
  {
    setRegistrationID("SystemSleep");
  }

  static PortsList providedPorts()
  {
    return {BT::InputPort<double>("sleep_time", "double: unit in seconds")};
  }

private:
  virtual BT::NodeStatus tick() override
  {
    setStatus(NodeStatus::RUNNING);

    auto sleeTimeMsg = getInput<double>("sleep_time");
    if (!sleeTimeMsg)
    {
      throw BT::RuntimeError("missing required input [sleep_time]: ",
                             sleeTimeMsg.error());
    }

    double sleeTime = sleeTimeMsg.value();
    //std::this_thread::sleep_for(std::chrono::duration<double>(sleeTime));
    interruptibleSleep(sleeTime);

    if (IsStopChildNodes())
    {
      setStatus(NodeStatus::FAILURE);
      return NodeStatus::FAILURE;
    }

    setStatus(NodeStatus::SUCCESS);
    return BT::NodeStatus::SUCCESS;
  }

  void interruptibleSleep(double seconds)
  {
    using namespace std::chrono;
    auto endTime = steady_clock::now() + duration<double>(seconds);

    while (steady_clock::now() < endTime && !IsStopChildNodes())
    {
      // 小歇一下，避免空转 CPU
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
};
}   // namespace BT

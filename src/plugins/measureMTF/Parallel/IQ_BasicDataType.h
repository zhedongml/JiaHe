#pragma once

#include <vector>
#include <iostream>

namespace IQ_Parallel_NS
{
	typedef enum class IQ_TaskState
	{
		Idle = 0x00,
		Busy = 0xFF
	};

	typedef enum class IQ_WaitResult
	{
		Success = 0x00,
		TimeOut = 0x01,
		Abort = 0x02
	};
}


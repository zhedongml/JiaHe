#pragma once

#include <vector>
#include <iostream>
#include <mutex>
#include <condition_variable>

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

	struct SharedData {
		std::mutex mtx;                     
		std::condition_variable cv;
		std::condition_variable consumer_done_cv;

		bool stop = false;
		bool ready = false;
		bool consumers_done = false;
		QString imageName = "";

		int active_consumers = 0;
		int total_consumers = 0;

		void reset() {
			std::lock_guard<std::mutex> lock(mtx);
			ready = false;
			consumers_done = false;
			active_consumers = 0;
			total_consumers = 0;
			imageName = "";
		}
	};
}


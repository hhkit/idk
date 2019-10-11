#include "stdafx.h"
#include "Logger.h"

namespace idk
{
	void LogSystem::LogMessage(LogPool pool, string_view message)
	{
		logs[s_cast<size_t>(pool)].Post(message);
		if (pool != LogPool::ANY)
			logs.front().Post(message);
	}

	void LogSystem::FlushLog(LogPool pool)
	{
		auto& buffer = logs[s_cast<size_t>(pool)].buffer;
		auto tst = buffer.begin();
		buffer.erase(buffer.begin(), buffer.end());
	}

	void LogSystem::Log::Post(string_view msg)
	{
		// lock
		bool curr_gate = false;

		//while (writing.compare_exchange_strong(curr_gate, true, std::memory_order_acquire)); 
		buffer.emplace_back(string{ msg });

		// unlock
		//writing.store(false);
	}
}
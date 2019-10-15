#include "stdafx.h"
#include "LogSystem.h"
#include <iostream>

namespace idk
{
	LogSystem::~LogSystem()
	{
		std::cerr.flush();
	}
	void LogSystem::LogMessage(LogPool pool, string_view message)
	{
		auto& log = logs[s_cast<size_t>(pool)];
		log.Post(message);
		if (pool != LogPool::ANY)
			logs.front().Post(message);

		if (log.direct_to_cout)
			std::cout << message;
	}

	void LogSystem::FlushLog(LogPool pool)
	{
		auto& buffer = logs[s_cast<size_t>(pool)].buffer;
		auto tst = buffer.begin();
		buffer.erase(buffer.begin(), buffer.end());
	}

	void LogSystem::PipeToCout(LogPool pool, bool pipe)
	{
		logs[s_cast<size_t>(pool)].direct_to_cout.store(pipe);
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
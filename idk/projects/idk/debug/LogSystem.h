#pragma once
#include <idk.h>

#include <mutex>

#include <core/ISystem.h>
#include <debug/LogPools.h>
#include <ds/circular_buffer.h>

namespace idk
{

	class LogSystem
		: public ISystem
	{
	public:

		~LogSystem();

		void LogMessage(LogPool pool, string_view message);
		void FlushLog(LogPool pool);

		template<typename Functor>
		void Process(LogPool pool, Functor&& functor);

		void PipeToCout(LogPool pool, bool pipe = false);
	private:
		struct Log
		{
			circular_buffer<string, 128> buffer {};
			//atomic<bool> writing{ false };
			atomic<bool> direct_to_cout{ false };
			void Post(string_view msg);
		};

		array<Log, s_cast<size_t>(LogPool::COUNT)> logs;

		void Init()     noexcept override {}
		void Shutdown() noexcept override {}
	};

	template<typename Functor>
	inline void LogSystem::Process(LogPool pool, Functor&& functor)
	{
		auto& buffer = logs[s_cast<size_t>(pool)].buffer;

		for (auto& elem : buffer)
			functor(elem);
	}
}
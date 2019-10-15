#pragma once
#include <idk.h>

#include <mutex>

#include <core/ISystem.h>
#include <ds/circular_buffer.h>

//#define LOG(msg, ...) Core::GetSystem<LogSystem>().LogMessage();

namespace idk
{
	enum class LogPool
	{
		ANY,
		SYS,
		PHYS,
		GFX,
		GAME,
		COUNT
	};

	class LogSystem
		: public ISystem
	{
	public:
		void LogMessage(LogPool pool, string_view message);
		void FlushLog(LogPool pool);

		template<typename Functor>
		void Process(LogPool pool, Functor&& functor);
	private:
		struct Log
		{
			circular_buffer<string, 100> buffer {};
			//atomic<bool> writing{ false };

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
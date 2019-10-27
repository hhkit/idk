#pragma once
#include <string_view>
#include <atomic>
#include <mutex>
#include <event/Signal.h>
#include <meta/casts.h>
#include <debug/LogPools.h>

namespace idk
{
	using string_view = std::string_view;
	class LogSingleton
	{
	public:
		using PrefaceType = string_view;
		using MessageType = string_view;
		using LogSignal = Signal<LogLevel, PrefaceType, MessageType>;

		~LogSingleton();

		void LogMessage(LogLevel level, LogPool pool, string_view preface, string_view message);
		void LogMessage(LogLevel level, LogPool pool, string_view preface, string_view message, va_list);
		void PipeToCout(LogPool pool, bool pipe = false);

		LogSignal& SignalFor(LogPool);

		static LogSingleton& Get();
	private:
		struct Log
		{
			LogSignal signal;
			std::atomic<bool> direct_to_cout{ false };
		};

		array<Log, static_cast<size_t>(LogPool::COUNT)> logs;
	};
}
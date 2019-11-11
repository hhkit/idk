#include "stdafx.h"
#include "LogSingleton.h"
#include <iostream>
#include <cstdio>
#include "LogSystem.h"
namespace idk
{
	LogSingleton::~LogSingleton()
	{
		std::cerr.flush();
	}
	void LogSingleton::LogMessage(LogLevel level, LogPool pool, string_view preface, string_view message)
	{
		auto now = Clock::now();

		auto& log = logs[s_cast<size_t>(pool)];

		if (log.direct_to_cout || log.signal.ListenerCount() == 0)
			std::cerr << message << '\n';

		log.signal.Fire(level, now, preface, message);
		if (pool != LogPool::ANY)
			logs[s_cast<int>(LogPool::ANY)].signal.Fire(level, now, preface, message);
	}
	void LogSingleton::LogMessage(LogLevel level, LogPool pool, string_view preface, string_view message, va_list args)
	{
		auto now = Clock::now();
		auto& log = logs[s_cast<size_t>(pool)];
		char print_buffer[LogSystem::log_buffer_size] = {};
		
		vsnprintf_s(print_buffer, sizeof(print_buffer),_TRUNCATE,message.data(), args);

		if (log.direct_to_cout || log.signal.ListenerCount() == 0)
			std::cerr << print_buffer << '\n';

		log.signal.Fire(level, now, preface, message);
		if (pool != LogPool::ANY)
			logs[s_cast<int>(LogPool::ANY)].signal.Fire(level, now, preface, message);
	}

	void LogSingleton::PipeToCout(LogPool pool, bool pipe)
	{
		logs[s_cast<size_t>(pool)].direct_to_cout.store(pipe);
	}

	LogSingleton::LogSignal& LogSingleton::SignalFor(LogPool pool)
	{
		return logs[s_cast<size_t>(pool)].signal;
	}

	LogSingleton& LogSingleton::Get()
	{
		static LogSingleton myers;
		return myers;
	}
}
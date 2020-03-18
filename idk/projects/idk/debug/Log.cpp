#include "stdafx.h"
#include "Log.h"
#include "LogSingleton.h"

#include "LogSystem.h"
#include <cstdarg>

namespace idk
{
	void Log(LogLevel level, LogPool pool, std::string_view preface, std::string_view message, ...)
	{
		char buf[LogSystem::log_buffer_size];
		size_t printed_size = {};
		{
			va_list args;
			va_start(args, message);
			printed_size = vsnprintf(NULL, 0, message.data(), args);
			va_end(args);
		}
		if (printed_size < std::size(buf))
		{
			va_list args;
			va_start(args, message);
			vsprintf_s(buf, message.data(), args);
			va_end(args);
			LogSingleton::Get().LogMessage(level, pool, preface, buf);
		}
		else
		{
			std::string long_message;
			long_message.resize(printed_size + 1);
			va_list args;
			va_start(args, message);
			vsprintf_s(long_message.data(), long_message.size(), message.data(), args);
			va_end(args);
			LogSingleton::Get().LogMessage(level, pool, preface, long_message);
		}

	}
}

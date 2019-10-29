#include "stdafx.h"
#include "Log.h"
#include "LogSingleton.h"

namespace idk
{
	void Log(LogLevel level, LogPool pool, std::string_view preface, std::string_view message, ...)
	{
		char buf[256];

		try {
			va_list args;
			va_start(args, message);
			vsprintf_s(buf, message.data(), args);
			va_end(args);
			LogSingleton::Get().LogMessage(level, pool, preface, buf);
		}
		catch (...)
		{
			LogSingleton::Get().LogMessage(level, pool, preface, "log message exceeded 256 characters");
		}
	}
}

#include "stdafx.h"
#include "Log.h"
#include "LogSystem.h"

namespace idk
{
	void Log(LogLevel level, LogPool pool, std::string_view preface, std::string_view message, ...)
	{
		va_list args;
		va_start(args, message);
		LogSingleton::Get().LogMessage(level, pool, preface, message, args);
		va_end(args);
	}
}

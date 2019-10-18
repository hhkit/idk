#pragma once
#include <string_view>
#include <debug/LogPools.h>

namespace idk
{
	void Log(LogPool pool, std::string_view preface, std::string_view message);
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#define LOG_TO(POOL, MESSAGE) idk::Log(POOL, AT "\t", MESSAGE)
#define LOG(MESSAGE) LOG_TO(LogPool::ANY, MESSAGE)
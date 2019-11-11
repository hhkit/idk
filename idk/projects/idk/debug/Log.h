#pragma once
#include <string_view>
#include <debug/LogPools.h>

/*
	Logging system uses C style logging to avoid mallocs
*/

namespace idk
{
	void Log(LogLevel level, LogPool pool, std::string_view preface, std::string_view message, ...);
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT() \
	[&]()	\
{	\
constexpr auto preface = std::string_view(__FILE__ ":" TOSTRING(__LINE__)); \
return preface.substr(preface.find_last_of('\\') + 1);\
}()

#define LOG_TO(POOL, MESSAGE, ...)  idk::Log(idk::LogLevel::INFO, POOL, AT(), MESSAGE, __VA_ARGS__)
#define LOG(MESSAGE, ...)         LOG_TO(idk::LogPool::ANY, MESSAGE, __VA_ARGS__)

#define LOG_WARNING_TO(POOL, MESSAGE, ...) idk::Log(idk::LogLevel::WARNING, POOL, AT(), MESSAGE, __VA_ARGS__)
#define LOG_WARNING(MESSAGE, ...) LOG_WARNING_TO(idk::LogPool::ANY, MESSAGE, __VA_ARGS__)

#define LOG_ERROR_TO(POOL, MESSAGE, ...)   idk::Log(idk::LogLevel::ERR, POOL, AT(), MESSAGE, __VA_ARGS__)
#define LOG_ERROR(MESSAGE, ...) LOG_ERROR_TO(idk::LogPool::ANY, MESSAGE, __VA_ARGS__)

#define LOG_CRASH_TO(POOL, MESSAGE, ...)   idk::Log(idk::LogLevel::FATAL, POOL, AT(), MESSAGE, __VA_ARGS__)
#define LOG_CRASH(MESSAGE, ...) LOG_CRASH_TO(idk::LogPool::ANY, MESSAGE, __VA_ARGS__)

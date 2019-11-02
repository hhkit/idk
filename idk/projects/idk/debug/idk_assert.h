#pragma once
#ifdef _DEBUG
#include <debug/Log.h>

namespace idk 
{
	struct FailedAssert {};
}

#define IDK_ASSERT(COND)                                               \
{                                                                      \
	if (!(COND))                                                       \
	{                                                                  \
		LOG_CRASH_TO(LogPool::SYS, "Assertion failed!\n");             \
		LOG_CRASH_TO(LogPool::SYS, "Condition was: " #COND "\n");      \
		throw idk::FailedAssert{};                                     \
	}                                                                  \
}

#define IDK_ASSERT_MSG(COND, MSG, ...)                                 \
{                                                                      \
	if (!(COND))                                                       \
	{                                                                  \
		LOG_CRASH_TO(LogPool::SYS, "Message: " MSG "\n", __VA_ARGS__); \
		LOG_CRASH_TO(LogPool::SYS, "Assertion failed!\n");             \
		LOG_CRASH_TO(LogPool::SYS, "Condition was: " #COND "\n");      \
		throw idk::FailedAssert{};                                     \
	}                                                                  \
}

#else
#define IDK_ASSERT(COND) {(void) (COND);}
#define IDK_ASSERT_MSG(COND, MSG, ...) {(void) (COND);}
#endif

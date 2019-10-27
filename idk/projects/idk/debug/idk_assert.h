#pragma once
#ifdef _DEBUG
#include <debug/Log.h>
#define IDK_ASSERT(COND)                                                              \
{                                                                                     \
	if (!(COND))                                                                      \
	{                                                                                 \
		LOG_CRASH_TO(LogPool::SYS, "Assertion failed!\n");                                \
		LOG_CRASH_TO(LogPool::SYS, "Condition was: " #COND "\n");                         \
		throw;                                                                        \
	}                                                                                 \
}

#define IDK_ASSERT_MSG(COND, MSG)                                                     \
{                                                                                     \
	if (!(COND))                                                                      \
	{                                                                                 \
		LOG_CRASH_TO(LogPool::SYS, "Message: " MSG "\n");                                 \
		LOG_CRASH_TO(LogPool::SYS, "Assertion failed!\n");                                \
		LOG_CRASH_TO(LogPool::SYS, "Condition was: " #COND "\n");                         \
		throw;                                                                        \
	}                                                                                 \
}

#else
#define IDK_ASSERT(COND) {(COND);}
#define IDK_ASSERT_MSG(COND, MSG) {(COND);}
#endif

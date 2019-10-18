#pragma once
#ifdef _DEBUG
#include <debug/Log.h>
#define IDK_ASSERT(COND)                                                              \
{                                                                                     \
	if (!(COND))                                                                      \
	{                                                                                 \
		LOG_TO(LogPool::FATAL, "Assertion failed!\n");                                \
		LOG_TO(LogPool::FATAL, "Condition was: " #COND "\n");                         \
		throw;                                                                        \
	}                                                                                 \
}

#define IDK_ASSERT_MSG(COND, MSG)                                                     \
{                                                                                     \
	if (!(COND))                                                                      \
	{                                                                                 \
		LOG_TO(LogPool::FATAL, "Message: " MSG "\n");                                 \
		LOG_TO(LogPool::FATAL, "Assertion failed!\n");                                \
		LOG_TO(LogPool::FATAL, "Condition was: " #COND "\n");                         \
		throw;                                                                        \
	}                                                                                 \
}

#else
#define IDK_ASSERT(COND) {(COND);}
#define IDK_ASSERT_MSG(COND, MSG) {(COND);}
#endif

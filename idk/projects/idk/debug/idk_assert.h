#pragma once
#ifdef _DEBUG
#include <cstdio>
#define IDK_ASSERT(COND)                                                              \
{                                                                                     \
	if (!(COND))                                                                      \
	{                                                                                 \
		fprintf(stderr, "Assertion failed in file " __FILE__ ", line %d\n", __LINE__);\
		fprintf(stderr, "Condition was: " #COND "\n");                                \
		fflush(stderr);                                                               \
		throw;                                                                        \
	}                                                                                 \
}

#define IDK_ASSERT_MSG(COND, MSG)                                                     \
{                                                                                     \
	if (!(COND))                                                                      \
	{                                                                                 \
		fprintf(stderr, "Message: " MSG);                                             \
		fprintf(stderr, "Assertion failed in file " __FILE__ ", line %d\n", __LINE__);\
		fprintf(stderr, "Condition was: " #COND "\n");                                \
		fflush(stderr);                                                               \
		throw;                                                                        \
	}                                                                                 \
}

#else
#define IDK_ASSERT(COND) {(COND);}
#define IDK_ASSERT_MSG(COND, MSG) {(COND);}
#endif

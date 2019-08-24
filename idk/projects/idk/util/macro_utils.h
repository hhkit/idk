#pragma once

#define IDENTITY(x) x
#define GLUE(L, R) L##R
#define _GET_NTH(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,N,...) N
#define COUNT_ARGS(...) IDENTITY(_GET_NTH(__VA_ARGS__,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))

#define _FOREACH_1(m, x)      m(x)
#define _FOREACH_2(m, x, ...) m(x) IDENTITY(_FOREACH_1(m, __VA_ARGS__))
#define _FOREACH_3(m, x, ...) m(x) IDENTITY(_FOREACH_2(m, __VA_ARGS__))
#define _FOREACH_4(m, x, ...) m(x) IDENTITY(_FOREACH_3(m, __VA_ARGS__))
#define _FOREACH_5(m, x, ...) m(x) IDENTITY(_FOREACH_4(m, __VA_ARGS__))
#define _FOREACH_6(m, x, ...) m(x) IDENTITY(_FOREACH_5(m, __VA_ARGS__))
#define _FOREACH_7(m, x, ...) m(x) IDENTITY(_FOREACH_6(m, __VA_ARGS__))
#define _FOREACH_8(m, x, ...) m(x) IDENTITY(_FOREACH_7(m, __VA_ARGS__))
#define _FOREACH_9(m, x, ...) m(x) IDENTITY(_FOREACH_8(m, __VA_ARGS__))
#define _FOREACH_10(m, x, ...) m(x) IDENTITY(_FOREACH_9(m, __VA_ARGS__))
#define _FOREACH_11(m, x, ...) m(x) IDENTITY(_FOREACH_10(m, __VA_ARGS__))
#define _FOREACH_12(m, x, ...) m(x) IDENTITY(_FOREACH_11(m, __VA_ARGS__))
#define _FOREACH_13(m, x, ...) m(x) IDENTITY(_FOREACH_12(m, __VA_ARGS__))
#define _FOREACH_14(m, x, ...) m(x) IDENTITY(_FOREACH_13(m, __VA_ARGS__))
#define _FOREACH_15(m, x, ...) m(x) IDENTITY(_FOREACH_14(m, __VA_ARGS__))
#define _FOREACH_16(m, x, ...) m(x) IDENTITY(_FOREACH_15(m, __VA_ARGS__))

#define _FOREACH_N(macro, N, ...) IDENTITY(GLUE(_FOREACH_, N)(macro, __VA_ARGS__))
#define FOREACH(macro, ...) _FOREACH_N(macro, COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#pragma once

#define CACHE_LINE 64

#ifdef _MSC_VER
#define ALIGN(x) __declspec(align(x))
#elif
#define ALIGN(x) alignas(x)
#endif

#define CACHE_ALIGN ALIGN(CACHE_LINE)
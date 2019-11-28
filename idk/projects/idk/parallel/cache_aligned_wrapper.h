#pragma once
#include <machine.h>
#pragma warning(disable:4324)
namespace idk::mt
{
	template<typename T>
	struct alignas(machine::cache_line_sz) cache_aligned_wrapper
	{
		T value;
	};
}
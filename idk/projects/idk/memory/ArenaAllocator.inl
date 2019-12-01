#pragma once
#include "ArenaAllocator.h"

namespace idk
{
	template<typename T>
	template<typename Y>
	inline ArenaAllocator<T>::ArenaAllocator(Y* pool, size_t bytes) noexcept
	{
		auto ptr = reinterpret_cast<unsigned char*>(pool);
		auto aligned_ptr = align<arena_block>(ptr);
		auto start = aligned_ptr + 1;
		new (aligned_ptr) arena_block{ start, bytes - static_cast<size_t>(reinterpret_cast<unsigned char*>(start) - ptr) };
		arena = aligned_ptr;
	}
	template<typename T>
	inline typename ArenaAllocator<T>::ptr_t ArenaAllocator<T>::allocate_fallback(size_t n)
	{
		return (ptr_t) new empty[n];
	}
	template<typename T>
	inline void ArenaAllocator<T>::deallocate_fallback(ptr_t ptr)
	{
		delete[](empty*)ptr;
	}
	template<typename T>
	typename ArenaAllocator<T>::ptr_t ArenaAllocator<T>::allocate(size_t n)
	{
		if (!arena)
			return allocate_fallback(n);
		ptr_t result = align<T>(arena->next);
		auto new_next = reinterpret_cast<unsigned char*>(result + n * sizeof(T));

		if (new_next > arena->end)
		{
			return allocate_fallback(n);
		}
		arena->next = new_next;
		return result;
	}
	template<typename T>
	inline void ArenaAllocator<T>::deallocate(ptr_t ptr, size_t N) noexcept
	{
		auto cptr = reinterpret_cast<unsigned char*>(ptr);
		if (arena && cptr >= arena->arena && cptr < arena->end)
			arena->try_release(ptr, N); // tiny optimization for stuff like stacks w/ lifo allocation
		else
			deallocate_fallback(ptr);
	}
}
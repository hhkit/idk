#pragma once
#include "ArenaAllocator.h"

namespace idk
{
	template<typename T,typename arena_block_t>
	template<typename Y>
	inline ArenaAllocator<T,arena_block_t>::ArenaAllocator(Y* pool, size_t bytes) 
	{
		if (pool)
		{
			arena = arena_block_t::init(reinterpret_cast<unsigned char*>(pool), bytes);
		}
	}
	template<typename T, typename arena_block_t>
	inline typename ArenaAllocator<T,arena_block_t>::ptr_t ArenaAllocator<T,arena_block_t>::allocate_fallback(size_t n)
	{
		return (ptr_t) new empty[n];
	}
	template<typename T, typename arena_block_t>
	inline void ArenaAllocator<T,arena_block_t>::deallocate_fallback(ptr_t ptr)
	{
		delete[](empty*)ptr;
	}
	template<typename T, typename arena_block_t>
	typename ArenaAllocator<T,arena_block_t>::ptr_t ArenaAllocator<T,arena_block_t>::allocate(size_t n)
	{
		auto num_bytes = n * sizeof(T);
		if (arena && (arena->end - arena->arena) >= static_cast<ptrdiff_t>(num_bytes))
		{
			auto result = arena->try_allocate<T>(num_bytes);
			if (result)
				return result;
		}
		return allocate_fallback(n);
	}
	template<typename T, typename arena_block_t>
	inline void ArenaAllocator<T,arena_block_t>::deallocate(ptr_t ptr, size_t N) noexcept
	{
		auto cptr = reinterpret_cast<unsigned char*>(ptr);
		if (arena && cptr >= arena->arena && cptr < arena->end)
		{
			arena->try_release(ptr, N*sizeof(T)); // tiny optimization for stuff like stacks w/ lifo allocation
				//_free_list.Free(cptr - arena->arena, N);
		}
		else
			deallocate_fallback(ptr);
	}
}
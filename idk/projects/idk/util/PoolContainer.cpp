#include "stdafx.h"
#include "PoolContainer.h"
#include <unordered_map>
#include <shared_mutex>
#include <atomic>
namespace idk
{
	std::atomic<size_t> PCTracker::size = 0;
	static std::mutex mtx;
	static std::unordered_map<const void*, size_t> registered_pools;
	bool PCTracker::should_block = false;
	void PCTracker::Deregister(const void* pool, size_t sz) {} static void Dereg(const void* pool, size_t sz)
	{
		std::lock_guard guard{ mtx };
		if(registered_pools[pool])
			registered_pools[pool] -= sz;
		if (registered_pools[pool] > (1 << 12))
		{
			DoNothing2();
		}
		//size -= sz;
	}
	void PCTracker::Register(const void* pool, size_t sz) {} static void Rereg(const void* pool, size_t sz)
	{
		std::lock_guard guard{ mtx };
		registered_pools[pool] += sz;
		//size += sz;
	}
	size_t PCTracker::TotalSize()
	{
		size_t sz = 0;
		for (auto& [ptr, count] : registered_pools)
		{
			sz += count;
		}
		return sz;
	}
	//#pragma optimize("",off)
	void DoNothing2(bool a)
	{
		while (PCTracker::should_block);
	}
}
#include "pch.h"
#include "VulkanResourceManager.h"
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <mutex>
namespace idk::vkn
{
VulkanResourceManager* idk::vkn::impl::GetRscManager()
{
	return &Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().ResourceManager();
}



/*void RegisterRsc(shared_ptr<VulkanRscBase> rsc)
{
managed.emplace_back(rsc);
}*/
/**/
namespace dbg
{
	bool& crit()
	{
		thread_local static bool fml = false;
		return fml;
	}
	hash_set<void*>& Destroyed()
	{
		static hash_set<void*> destroyed;
		return destroyed;
	}
	hash_set<void*>& Destroyed2()
	{
		static hash_set<void*> destroyed;
		return destroyed;
	}
	hash_table<void*, RscHandle<Texture>>& LUT()
	{
		static hash_table<void*, RscHandle<Texture>> lut;
		return lut;
	}
	static RscHandle<Texture> dafuq;

	static std::mutex usage_lock;
	hash_table<void*, size_t>& UsageCache()
	{
		static hash_table<void*, size_t> lut;
		return lut;
	}
	hash_table<void*, void*>& DsUsageCache()
	{
		static hash_table<void*, void*> lut;
		return lut;
	}

	size_t func_counter;
	static hash_table<void*, size_t> tracker;
	size_t get_counter()
	{
		return func_counter;
	}
	hash_table<void*, size_t>& get_tracker()
	{
		return tracker;
	}
	hash_table<void*, bool> &get_ds_is_valid()
	{
		static hash_table<void*, bool> table;
		return table;
	}
	void UpdateUsage(vk::DescriptorSet ds)
	{
		return;
		std::lock_guard guard{ usage_lock };
		UsageCache()[(void*)ds.operator VkDescriptorSet()] = get_counter();
	}
	void UpdateUsage(vk::Sampler sampler,vk::DescriptorSet ds)
	{
		return;
		std::lock_guard guard{ usage_lock };
		auto ptr = (void*)sampler.operator VkSampler();
		auto dsptr =DsUsageCache()[ptr] = (void*)ds.operator VkDescriptorSet();
		get_ds_is_valid()[dsptr] = true;
	}
	bool GetValid(vk::DescriptorSet ds)
	{
		return true;
		std::lock_guard guard{ usage_lock };
		auto itr = get_ds_is_valid().find((void*)ds.operator VkDescriptorSet());
		if (itr == get_ds_is_valid().end())
			return true;
		return itr->second;
	}
	size_t GetLastUsed(void* ptr)
	{
		return 0;
		std::lock_guard guard{ usage_lock };
		auto itr = DsUsageCache().find(ptr);
		if (itr == DsUsageCache().end())
			return 0;
		auto itr2 = UsageCache().find(itr->second);
		if (itr2 == UsageCache().end())
			return 0;
		return itr2->second;
	}
	static auto func2 = [](auto&& uni)
	{
		return;
		void* data = *reinterpret_cast<void**>(&uni);
		std::lock_guard guard{ usage_lock };
		if (data)
		{
			tracker[data] = func_counter;
			auto itr = DsUsageCache().find(data);
			if (DsUsageCache().end() != itr)
			{
				get_ds_is_valid()[itr->second] = false;
			}
		}
	};
}
static std::mutex derp;
void VulkanResourceManager::QueueToDestroy(ptr_t obj_to_destroy)
{
	std::lock_guard guard{ derp };
	//if (dbg::crit())
	//	DebugBreak();
	//dbg::func2(obj_to_destroy->Data());
	//if (obj_to_destroy->Data() && !dbg::Destroyed2().emplace(obj_to_destroy->Data()).second)
	//	DebugBreak();
	managed.emplace_back(std::move(obj_to_destroy));
}
static size_t last_used = 0;
void VulkanResourceManager::ProcessQueue(uint32_t frame_index)
{
	//++dbg::func_counter;
	std::lock_guard guard{ derp };
	//if(destroy_queue.size()!=0|| frame_index!=0)
		std::swap(destroy_queue[frame_index], managed);
	//for(auto& ptr : managed)
	//{
	//	auto itr = dbg::LUT().find(ptr->Data());
	//	if (itr != dbg::LUT().end())
	//		dbg::dafuq = itr->second;
	//	else
	//		dbg::dafuq = {}; 
	//	
	//	last_used = dbg::GetLastUsed(ptr->Data());
	//	//if (last_used!=0 && last_used + 2 > dbg::get_counter())
	//	//	DebugBreak();
	//	//if (ptr->Data() && !dbg::Destroyed().emplace(ptr->Data()).second)
	//	//	DebugBreak();
	//	ptr.reset();
	//}
	managed.clear();
}

/*
//Finds and queues all the resources with no existing references.
void ProcessSingles(uint32_t frame_index)
{
auto& queue = destroy_queue[frame_index];
//Destroy the stuff that have already waited for 1 full frame cycle
queue.clear();//Assumes that the shared_ptrs queued are ready for destruction
auto itr = managed.begin(), end = managed.end();
for (; itr < end; ++itr)
{
//Last owner, assume no weak_ptrs to be locked in another thread at this time
if (itr->use_count() == 1)
{
//swap to "back"
std::swap(*itr, (*--end));
}
}
//Queue the next batch of stuff to be destroyed.
queue.insert(queue.end(), end, managed.end());
managed.resize(end - managed.begin());//trim it down
}
*/

void VulkanResourceManager::DestroyAll()
{
	for (auto& ptr : managed)
	{
		ptr->Destroy();
	}
	for (auto& pair : destroy_queue)
	{
		for (auto& ptr : pair.second)
		{
			ptr->Destroy();
		}
	}
}

}
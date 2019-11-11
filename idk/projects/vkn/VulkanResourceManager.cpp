#include "pch.h"
#include "VulkanResourceManager.h"
#include <vkn/VulkanWin32GraphicsSystem.h>
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

void VulkanResourceManager::QueueToDestroy(ptr_t obj_to_destroy)
{
	managed.emplace_back(std::move(obj_to_destroy));
}

void VulkanResourceManager::ProcessQueue(uint32_t frame_index)
{
	std::swap(destroy_queue[frame_index], managed);
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
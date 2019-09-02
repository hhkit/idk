#include "pch.h"
#include "DescriptorPoolsManager.h"
#include <vkn/BufferHelpers.h>
#include <vkn/VulkanView.h>

namespace idk::vkn
{
	DescriptorPoolsManager::Manager::Manager(uint32_t capacity_, vk::Device device, vk::DescriptorType type_) :type{ type_ },capacity{capacity_}
	{
		vk::DescriptorPoolSize pool_size[]
		{
			vk::DescriptorPoolSize{
				type,
				capacity
			}
		};
		vk::DescriptorPoolCreateInfo create_info
		{
			 vk::DescriptorPoolCreateFlagBits{} //Flag if we'll be deleting or updating the descriptor sets afterwards
			,capacity
			,hlp::arr_count(pool_size)
			,std::data(pool_size)
		};
		pool = device.createDescriptorPoolUnique(create_info, nullptr, vk::DispatchLoaderDefault{});
	}
	vk::DescriptorPool DescriptorPoolsManager::Add(uint32_t num_ds,vk::DescriptorType type)
	{
		uint32_t new_cap = base_chunk_size;
		while (num_ds > new_cap) new_cap *= 2;
		auto& new_manager = managers[type].emplace_back(new_cap, *view.Device(),type);
		new_manager.size = num_ds;
		return *new_manager.pool;
	}

	void DescriptorPoolsManager::ResetManager(Manager& manager)
	{
		vk::Device d = *view.Device();
		d.resetDescriptorPool(*manager.pool, vk::DescriptorPoolResetFlags{}, vk::DispatchLoaderDefault{});
		manager.size = 0;
	}

	void DescriptorPoolsManager::Reset(vk::DescriptorPool& pool)
	{
		for(auto& [type,managers_] : managers)
		{
			type;
			auto result = std::find_if(managers_.begin(), managers_.end(), [pool](auto& manager) {return *manager.pool == pool; });
			if (result != managers_.end())
				ResetManager(*result);
		}

	}

	void DescriptorPoolsManager::ResetAll()
	{
		for (auto& pair : managers)
		{
			for(auto&manager :pair.second)
				ResetManager(manager);
		}
	}

}
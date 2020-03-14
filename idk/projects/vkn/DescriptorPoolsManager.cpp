#include "pch.h"
#include "DescriptorPoolsManager.h"
#include <vkn/BufferHelpers.h>
#include <vkn/VulkanView.h>
namespace idk::vkn
{
	DescriptorPoolsManager::Manager::Manager(uint32_t capacity_, vk::Device device, vk::DescriptorType type_, vk::DescriptorPoolCreateFlagBits flags) :type{ type_ }, capacity{ capacity_ }
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
			 flags //Flag if we'll be deleting or updating the descriptor sets afterwards
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

	vk::DescriptorPool DescriptorPoolsManager::Add(const uint32_t(&num_dses)[DescriptorTypeI::size()])
	{
		uint32_t new_caps[std::size_v<decltype(num_dses)>] = {};
		for (size_t i=0;i<std::size(num_dses);++i)
		{
			auto& num_ds = num_dses[i];
			auto& new_cap = new_caps[i]= (num_ds )?base_chunk_size:0;
			while (num_ds && num_ds > new_cap) new_cap *= 2;
		}
		auto& new_manager = managers2.emplace_back(*view.Device(), new_caps);
		return *new_manager.pool;
	}

	void DescriptorPoolsManager::ResetManager(Manager& manager)
	{
		vk::Device d = *view.Device();
		d.resetDescriptorPool(*manager.pool, vk::DescriptorPoolResetFlags{}, vk::DispatchLoaderDefault{});
		manager.size = 0;
	}
	void DescriptorPoolsManager::ResetManager(Manager2& manager)
	{
		vk::Device d = *view.Device();
		d.resetDescriptorPool(*manager.pool, vk::DescriptorPoolResetFlags{}, vk::DispatchLoaderDefault{});
		for (auto& cap : manager.cap)
		{
			cap.size = 0;
		}
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
		for (auto& manager : managers2)
		{
			if (*manager.pool == pool)
				ResetManager(manager);
		}

	}

	void DescriptorPoolsManager::ResetAll()
	{
		for (auto& pair : managers)
		{
			for (auto& manager : pair.second)
				ResetManager(manager);
		}
		for (auto& manager : managers2)
			ResetManager(manager);
	}

	DescriptorPoolsManager::Manager2::Manager2(vk::Device device, const uint32_t(&capacities)[DescriptorTypeI::size()])
	{
		vk::DescriptorPoolSize pool_size[std::size_v<decltype(capacities)>];
		uint32_t max_sets = 0;
		uint32_t total_types = 0;
		for (size_t i = 0; i < std::size(capacities); ++i)
		{
			if (capacities[i])
			{
				pool_size[total_types] = vk::DescriptorPoolSize{
					DescriptorTypeI::map(i),
					cap[i].capacity = capacities[i]
				};
				max_sets += capacities[i];
				cap[i].size = 0;
				++total_types;
			}
		}
		vk::DescriptorPoolCreateInfo create_info
		{
			 vk::DescriptorPoolCreateFlagBits{} //Flag if we'll be deleting or updating the descriptor sets afterwards
			,max_sets
			,total_types//hlp::arr_count(pool_size)
			,std::data(pool_size)
		};
		pool = device.createDescriptorPoolUnique(create_info, nullptr, vk::DispatchLoaderDefault{});
	}

}
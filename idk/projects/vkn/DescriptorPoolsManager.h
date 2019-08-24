#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>

namespace idk::vkn
{
	class VulkanView;
struct DescriptorPoolsManager
{
	struct Manager
	{
		const vk::DescriptorType type = vk::DescriptorType::eUniformBuffer;
		vk::UniqueDescriptorPool pool;
		uint32_t size, capacity;
		Manager(uint32_t capacity_, vk::Device device, vk::DescriptorType type_ = vk::DescriptorType::eUniformBuffer);
	};
	struct FirstFit
	{
		static std::optional<vk::DescriptorPool> Get(uint32_t num_ds,
			vector<Manager>& managers)
		{
			std::optional<vk::DescriptorPool> result;
			for (auto& manager : managers)
			{
				if (manager.capacity - manager.size >= num_ds)
				{
					manager.size += num_ds;
					result = *manager.pool;
					break;
				}
			}
			return result;
		}
	};

	VulkanView& view;
	vector<Manager> managers;
	constexpr static uint32_t base_chunk_size = 1024;
	uint32_t curr_chunk_size = base_chunk_size;

	vk::DescriptorPool Add(uint32_t num_ds);


	template<typename Policy = FirstFit>
	vk::DescriptorPool Get(uint32_t pool);
	template<typename Policy = FirstFit>
	std::optional<vk::DescriptorPool> TryGet(uint32_t pool);
	void ResetManager(Manager& manager);
	void Reset(vk::DescriptorPool& pool);
	void ResetAll();

	template<typename Policy>
	struct IPolicy
	{
		static std::optional<vk::DescriptorPool> Get(uint32_t num_ds
			, vector<Manager>& managers);
	};
};
}
#include "DescriptorPoolsManager.inl"
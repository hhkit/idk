#pragma once
#pragma once
#include <vulkan/vulkan.hpp>
#include <optional>
#include <vector>
#include <unordered_map>
using std::vector;
template<typename K,typename V>
using hash_table = std::unordered_map<K, V>;
namespace vknu
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

		hash_table<vk::DescriptorType, vector<Manager>> managers;

		vk::Device _device;
		constexpr static uint32_t base_chunk_size = 128;
		uint32_t curr_chunk_size = base_chunk_size;

		DescriptorPoolsManager(VkDevice device);

		vk::DescriptorPool Add(uint32_t num_ds, vk::DescriptorType type = vk::DescriptorType::eUniformBuffer);


		template<typename Policy = FirstFit>
		vk::DescriptorPool Get(uint32_t pool, vk::DescriptorType type = vk::DescriptorType::eUniformBuffer);
		template<typename Policy = FirstFit>
		std::optional<vk::DescriptorPool> TryGet(uint32_t pool, vk::DescriptorType type = vk::DescriptorType::eUniformBuffer);
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
#include "DescriptorPoolManager.inl"
#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/vulkan_enum_info.h>
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
	struct Manager2
	{
		vk::UniqueDescriptorPool pool;
		struct alloc_info
		{
			uint32_t size{}, capacity{};
		};
		alloc_info cap[DescriptorTypeI::size()];
		Manager2(vk::Device device, const uint32_t (&capacities)[DescriptorTypeI::size()]);
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

		static std::optional<vk::DescriptorPool> Get(const uint32_t(&num_ds)[DescriptorTypeI::size()],
			vector<Manager2>& managers)
		{
			std::optional<vk::DescriptorPool> result;
			for (auto& manager : managers)
			{
				bool pass = true;
				for (size_t i = 0; i < std::size(num_ds); ++i)
				{
					if (manager.cap[i].capacity - manager.cap[i].size < num_ds[i])
					{
						pass = false;
						break;
					}
				}
				if (pass)
				{
					for (size_t i = 0; i < std::size(num_ds); ++i)
					{
						manager.cap[i].size += num_ds[i];
					}
					result = *manager.pool;
					break;
				}
			}
			return result;
		}
		static bool Check(uint32_t num_ds,
			vector<Manager>& managers)
		{
			bool result = false;
			for (auto& manager : managers)
			{
				if (manager.capacity - manager.size >= num_ds)
				{
					result = true;
					break;
				}
			}
			return result;
		}
	};


	VulkanView& view;
	hash_table<vk::DescriptorType,vector<Manager>> managers;
	vector<Manager2> managers2;
	constexpr static uint32_t base_chunk_size = 128;
	uint32_t curr_chunk_size = base_chunk_size;

	vk::DescriptorPool Add(uint32_t num_ds, vk::DescriptorType type = vk::DescriptorType::eUniformBuffer);
	vk::DescriptorPool Add(const uint32_t (&num_ds)[DescriptorTypeI::size()]);


	template<typename Policy = FirstFit>
	vk::DescriptorPool Get(uint32_t pool,vk::DescriptorType type=vk::DescriptorType::eUniformBuffer);
	template<typename Policy = FirstFit>
	std::optional<vk::DescriptorPool> TryGet(uint32_t pool, vk::DescriptorType type = vk::DescriptorType::eUniformBuffer);
	template<typename Policy = FirstFit>
	std::optional<vk::DescriptorPool> TryGet(const std::array<uint32_t,DescriptorTypeI::size()>& required);

	template<typename Policy = FirstFit>
	bool CanGet(uint32_t pool, vk::DescriptorType type = vk::DescriptorType::eUniformBuffer);
	void ResetManager(Manager& manager);
	void ResetManager(Manager2& manager);
	void Reset(vk::DescriptorPool& pool);
	void ResetAll();

	template<typename Policy>
	struct IPolicy
	{
		static std::optional<vk::DescriptorPool> Get(uint32_t num_ds
			, vector<Manager>& managers);
		static std::optional<vk::DescriptorPool> Get(const uint32_t (&num_ds)[DescriptorTypeI::size()]
			, vector<Manager2>& managers);
		static bool  Check(uint32_t num_ds
			, vector<Manager>& managers);
	};
};
}
#include "DescriptorPoolsManager.inl"
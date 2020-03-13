#pragma once
#include <idk.h>
#include <vkn/VulkanHashes.h>
#include <vkn/DescriptorPoolsManager.h>
#include <vulkan/vulkan.hpp>
#include <vkn/vulkan_enum_info.h>

#include <vkn/VulkanView.h>

namespace idk::vkn
{
struct DescriptorSets
{
	vector<vk::DescriptorSet> sets{};
	uint32_t curr_index{};

	uint32_t size()const;
	uint32_t num_available()const;

	vector<vk::DescriptorSet>& operator=(vector<vk::DescriptorSet>&& rhs);
	vector<vk::DescriptorSet>& operator=(const vector<vk::DescriptorSet>& rhs);
	void FreeDS(vk::DescriptorSet ds);
	vk::DescriptorSet GetNext();
	std::optional<DescriptorSets> GetRange(uint32_t num);
};
using DescriptorSetLookup = hash_table<vk::DescriptorSetLayout, DescriptorSets>;
struct DescriptorsManager
{
	using DsCountArray =std::array < uint32_t, DescriptorTypeI::size()>;
	DescriptorPoolsManager pools;
	//hash_table<vk::DescriptorSetLayout, DescriptorSetManager> ds_sets;

	//DescriptorSetLookup allocated_dses;
	DescriptorSetLookup free_dses;

	DescriptorsManager(VulkanView& view = vkn::View());

	DescriptorSetLookup Allocate(const hash_table<vk::DescriptorSetLayout, std::pair<vk::DescriptorType, uint32_t>>& allocations);
	void Free(vk::DescriptorSetLayout layout, vk::DescriptorSet ds);
	//pair<num_ds,num_descriptors_per_type>
	DescriptorSetLookup Allocate(const hash_table < vk::DescriptorSetLayout, std::pair<uint32_t,DsCountArray>>& allocations);

	void Grow(const hash_table<vk::DescriptorSetLayout, std::pair<vk::DescriptorType, uint32_t>>& allocations);
	void Grow(const hash_table<vk::DescriptorSetLayout, std::pair<uint32_t, DsCountArray>>& allocations);

	void Reset();
};
}
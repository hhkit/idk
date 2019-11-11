#pragma once
#include <idk.h>
#include <vkn/DescriptorPoolsManager.h>
#include <vulkan/vulkan.hpp>
#include <vkn/vulkan_enum_info.h>
namespace idk::vkn
{
struct DescriptorSets
{
	vector<vk::DescriptorSet> sets{};
	uint32_t curr_index{};

	uint32_t size()const;

	vector<vk::DescriptorSet>& operator=(vector<vk::DescriptorSet>&& rhs);
	vector<vk::DescriptorSet>& operator=(const vector<vk::DescriptorSet>& rhs);
	vk::DescriptorSet& GetNext();
};
using DescriptorSetLookup = hash_table<vk::DescriptorSetLayout, DescriptorSets>;
struct DescriptorsManager
{
	DescriptorPoolsManager pools;
	//hash_table<vk::DescriptorSetLayout, DescriptorSetManager> ds_sets;
	DescriptorsManager(VulkanView& view);
	DescriptorSetLookup Allocate(const hash_table<vk::DescriptorSetLayout, std::pair<vk::DescriptorType, uint32_t>>& allocations);
	//pair<num_ds,num_descriptors_per_type>
	DescriptorSetLookup Allocate(const hash_table < vk::DescriptorSetLayout, std::pair<uint32_t,std::array<uint32_t, DescriptorTypeI::size()>>>& allocations);
	void Reset();
};
}
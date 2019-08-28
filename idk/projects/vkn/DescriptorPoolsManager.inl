#pragma once
#include <vkn/DescriptorPoolsManager.h>
namespace idk::vkn
{
	template<typename Policy>
	vk::DescriptorPool DescriptorPoolsManager::Get(uint32_t pool)
	{
		auto result = IPolicy<Policy>::Get(pool, managers);
		if (!result)
		{
			result = Add(pool);
		}
		return *result;
	}
	template<typename Policy>
	std::optional<vk::DescriptorPool> DescriptorPoolsManager::TryGet(uint32_t pool)
	{
		auto result = IPolicy<Policy>::Get(pool, managers);
		return result;
	}
	template<typename Policy>
	std::optional<vk::DescriptorPool> DescriptorPoolsManager::IPolicy<Policy>::Get(uint32_t num_ds, vector<Manager>& managers)
	{
		return Policy::Get(num_ds, managers);
	}
}
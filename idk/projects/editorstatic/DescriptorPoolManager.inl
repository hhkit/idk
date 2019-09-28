#pragma once
#include "DescriptorPoolManager.h"
namespace vknu
{
	template<typename Policy>
	vk::DescriptorPool DescriptorPoolsManager::Get(uint32_t pool, vk::DescriptorType type)
	{
		auto result = IPolicy<Policy>::Get(pool, managers[type]);
		if (!result)
		{
			result = Add(pool, type);
		}
		return *result;
	}
	template<typename Policy>
	std::optional<vk::DescriptorPool> DescriptorPoolsManager::TryGet(uint32_t pool, vk::DescriptorType type)
	{
		auto result = IPolicy<Policy>::Get(pool, managers[type]);
		return result;
	}
	template<typename Policy>
	std::optional<vk::DescriptorPool> DescriptorPoolsManager::IPolicy<Policy>::Get(uint32_t num_ds, vector<Manager>& managers)
	{
		return Policy::Get(num_ds, managers);
	}
}
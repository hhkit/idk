#pragma once
#include <vkn/DescriptorPoolsManager.h>
namespace idk::vkn
{
	template<typename Policy>
	vk::DescriptorPool DescriptorPoolsManager::Get(uint32_t num_ds, vk::DescriptorType type )
	{
		auto result = IPolicy<Policy>::Get(num_ds, managers[type]);
		if (!result)
		{
			result = Add(num_ds,type);
		}
		return *result;
	}
	template<typename Policy>
	std::optional<vk::DescriptorPool> DescriptorPoolsManager::TryGet(uint32_t num_ds, vk::DescriptorType type)
	{
		auto result = IPolicy<Policy>::Get(num_ds, managers[type]);
		return result;
	}
	template<typename Policy>
	std::optional<vk::DescriptorPool> DescriptorPoolsManager::TryGet(const std::array<uint32_t, DescriptorTypeI::size()>& required)
	{
		auto result = IPolicy<Policy>::Get(required._Elems, managers2);
		return result;
	}
	template<typename Policy>
	bool DescriptorPoolsManager::CanGet(uint32_t num_ds, vk::DescriptorType type)
	{
		auto result = IPolicy<Policy>::Check(num_ds, managers[type]);
		return result;
	}
	template<typename Policy>
	std::optional<vk::DescriptorPool> DescriptorPoolsManager::IPolicy<Policy>::Get(uint32_t num_ds, vector<Manager>& managers)
	{
		return Policy::Get(num_ds, managers);
	}
	template<typename Policy>
	std::optional<vk::DescriptorPool> DescriptorPoolsManager::IPolicy<Policy>::Get(const uint32_t(&num_ds)[DescriptorTypeI::size()], vector<Manager2>& managers)
	{
		return Policy::Get(num_ds, managers);
	}
	template<typename Policy>
	inline bool DescriptorPoolsManager::IPolicy<Policy>::Check(uint32_t num_ds, vector<Manager>& managers)
	{
		return Policy::Check(num_ds,managers);
	}
}
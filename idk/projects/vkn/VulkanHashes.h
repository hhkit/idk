#pragma once
#include <vulkan/vulkan.hpp>
#include <cstdint>

namespace std
{
	template<>
	struct hash<vk::DescriptorSetLayout>
	{
		size_t operator()(const vk::DescriptorSetLayout& dsl)const
		{
			return reinterpret_cast<intptr_t>(dsl.operator VkDescriptorSetLayout());
		}
	};
	template<>
	struct hash<vk::DescriptorPool>
	{
		size_t operator()(const vk::DescriptorPool& dsl)const
		{
			return reinterpret_cast<intptr_t>(dsl.operator VkDescriptorPool());
		}
	};
	template<>
	struct hash<vk::Semaphore>
	{
		size_t operator()(const vk::Semaphore& s)const
		{
			return reinterpret_cast<intptr_t>(s.operator VkSemaphore());
		}
	};
}

#pragma once
#include <vulkan/vulkan.hpp>
#include <vkn/utils/utils.h>
#include <vkn/utils/utils.inl>
namespace idk::vkn
{
	namespace EnumInfo
	{

	using DescriptorTypePack = meta::enum_pack<vk::DescriptorType,
		vk::DescriptorType::eSampler, // VK_DESCRIPTOR_TYPE_SAMPLER,
		vk::DescriptorType::eCombinedImageSampler, // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		vk::DescriptorType::eSampledImage, // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
		vk::DescriptorType::eStorageImage, // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		vk::DescriptorType::eUniformTexelBuffer, // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
		vk::DescriptorType::eStorageTexelBuffer, // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
		vk::DescriptorType::eUniformBuffer, // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		vk::DescriptorType::eStorageBuffer, // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		vk::DescriptorType::eUniformBufferDynamic, // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		vk::DescriptorType::eStorageBufferDynamic, // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
		vk::DescriptorType::eInputAttachment, // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
		vk::DescriptorType::eInlineUniformBlockEXT, // VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT,
		vk::DescriptorType::eAccelerationStructureNV // VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV
	>;
	using DescriptorTypeI = meta::enum_info < vk::DescriptorType, DescriptorTypePack>;

	}
	using namespace EnumInfo;
}
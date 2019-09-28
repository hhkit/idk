#pragma once
#include <idk.h>
#include <vkn/VulkanResourceManager.h>

namespace idk::vkn
{
	using UniqueBuffer = VulkanRsc<vk::Buffer>;
	using UniqueSemaphore = VulkanRsc<vk::Semaphore>;
	using UniqueFramebuffer = VulkanRsc<vk::Framebuffer>;
	using UniqueImageView = VulkanRsc<vk::ImageView>;
	using UniqueShaderModule = VulkanRsc<vk::ShaderModule>;
	using UniqueImage = VulkanRsc<vk::Image>;

}
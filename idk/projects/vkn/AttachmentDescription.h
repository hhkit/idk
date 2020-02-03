#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VknTexture.h>
namespace idk::vkn
{

	struct TextureDescription
	{
		string_view name;
		uvec2 size;
		vk::Format format;
		vk::ImageAspectFlagBits aspect;
		vk::ImageType type=vk::ImageType::e2D;
		uint32_t layer_count=1;
		uint32_t mipmap_level = 1;
		vk::ImageTiling tiling_format = vk::ImageTiling::eOptimal;
		vk::ImageUsageFlags usage{};
		std::optional<RscHandle<VknTexture>> actual_rsc;
	};
	struct AttachmentDescription
	{
		vk::AttachmentLoadOp load_op;
		vk::AttachmentStoreOp store_op;
		vk::AttachmentLoadOp  stencil_load_op;
		vk::AttachmentStoreOp stencil_store_op;
		vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
		vk::ImageSubresourceRange sub_resource_range{};
		std::optional<vk::ClearValue> clear_value;
		std::optional<vk::Format> format{};
		vk::ImageViewType view_type{ vk::ImageViewType::e2D };
		vk::ComponentMapping mapping{};
	};
}
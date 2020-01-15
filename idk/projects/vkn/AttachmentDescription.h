#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{

	struct AttachmentDescription
	{
		string_view name;
		ivec2 size;
		vk::Format format;
		vk::ImageAspectFlags aspect;
		vk::ImageType type=vk::ImageType::e2D;
		uint32_t layer_count=1;
		vk::ImageTiling tiling_format;
		//Format format;
	};
}
#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/FrameGraphResource.h>
namespace idk::vkn
{

	struct CopyOptions
	{
		vk::ImageLayout dest_layout;
		vector<vk::ImageCopy> regions;
	};

	struct FrameGraphCopyResource
	{
		FrameGraphResource src, dest;
		CopyOptions options;
	};
}
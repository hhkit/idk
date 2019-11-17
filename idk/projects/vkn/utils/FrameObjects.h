#pragma once
#include <vulkan/vulkan.hpp>

#include <vkn/DescriptorsManager.h>
#include <vkn/UboManager.h>
#include <meta/stl_hack.h>
namespace idk::vkn
{
	struct FrameObjects
	{
		DescriptorsManager pools;
		UboManager ubo_manager;
		void FrameReset();
	};
};
MARK_NON_COPY_CTORABLE(idk::vkn::FrameObjects)
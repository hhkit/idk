#pragma once
#include <vulkan/vulkan.hpp>

#include <vkn/DescriptorsManager.h>
#include <vkn/UboManager.h>

namespace idk::vkn
{
	struct FrameObjects
	{
		DescriptorsManager pools;
		UboManager ubo_manager;
		FrameObjects() = default;
		FrameObjects(FrameObjects&&) = default;
		void FrameReset();
	};
};
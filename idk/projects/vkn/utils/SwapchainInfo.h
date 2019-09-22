#pragma once
#include <vulkan/vulkan.hpp>

#include <vkn/utils/TriBuffer.h>


namespace idk::vkn {

	struct TriBuffer;
	struct SwapChainInfo
	{
		uint32_t curr_index{};
		vk::UniqueSwapchainKHR             swap_chain;
		vk::PresentModeKHR				   present_mode;
		vk::SurfaceFormatKHR			   surface_format;
		//vk::Format                         format;
		vk::Extent2D                       extent;

		TriBuffer							m_graphics;
		vector<shared_ptr<TriBuffer>>       m_inBetweens;
		TriBuffer							m_swapchainGraphics;

		vector<FrameObjects> frame_objects;

	};
};
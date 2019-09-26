#pragma once
#include <vulkan/vulkan.hpp>

#include <vkn/utils/TriBuffer.h>

#include <vkn/utils/FrameObjects.h>

#include <vkn/VulkanView.h>


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

		SwapChainInfo() = default;
		SwapChainInfo(VulkanView& view,vk::UniqueSwapchainKHR s, vk::PresentModeKHR pm, vk::SurfaceFormatKHR sf,vk::Extent2D e);
		SwapChainInfo(VulkanView& view);

		SwapChainInfo(SwapChainInfo&& rhs);

		SwapChainInfo& operator=(SwapChainInfo&& rhs);
	};
};
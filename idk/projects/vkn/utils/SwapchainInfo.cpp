#include "pch.h"
#include "SwapchainInfo.h"

namespace idk::vkn {

	SwapChainInfo::SwapChainInfo(VulkanView& view, vk::UniqueSwapchainKHR s, vk::PresentModeKHR pm, vk::SurfaceFormatKHR sf, vk::Extent2D e)
		:swap_chain{std::move(s) },
		present_mode{ pm },
		surface_format{ sf },
		extent{ e },
		m_graphics{ view, true },
		m_inBetweens{ },
		m_swapchainGraphics{ view, true },
		frame_objects{}
	{
	}
	SwapChainInfo::SwapChainInfo(VulkanView& view)
		:swap_chain{ },
		present_mode{  },
		surface_format{  },
		extent{  },
		m_graphics{ view, true },
		m_inBetweens{ },
		m_swapchainGraphics{ view, true },
		frame_objects{}
	{
	}
	SwapChainInfo::SwapChainInfo(SwapChainInfo&& rhs) noexcept
		:swap_chain{ std::move(rhs.swap_chain) },
		present_mode{ std::move(rhs.present_mode) },
		surface_format{ std::move(rhs.surface_format) },
		extent{ std::move(rhs.extent) },
		m_graphics{ std::move(m_graphics) },
		m_inBetweens{ std::move(m_inBetweens) },
		m_swapchainGraphics{ std::move(m_swapchainGraphics) },
		frame_objects{ std::move(frame_objects) }
	{
	}

	SwapChainInfo& SwapChainInfo::operator=(SwapChainInfo&& rhs) noexcept
	{
		std::swap(swap_chain,rhs.swap_chain);
		std::swap(present_mode,rhs.present_mode);
		std::swap(surface_format,rhs.surface_format);
		std::swap(extent,rhs.extent);
		std::swap(m_graphics,rhs.m_graphics);
		std::swap(m_inBetweens,rhs.m_inBetweens);
		std::swap(m_swapchainGraphics,rhs.m_swapchainGraphics);
		std::swap(frame_objects,rhs.frame_objects);

		return *this;
	}
}

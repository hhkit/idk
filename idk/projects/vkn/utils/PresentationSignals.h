#pragma once
#include <vkn/ValHandler.h>
#include <vkn/VulkanResourceManager.h>
#include <vkn/ManagedVulkanObjs.h>

namespace idk::vkn
{
	struct PresentationSignals
	{
		vk::UniqueSemaphore		image_available{};
		vk::UniqueSemaphore		blit_finished{};
		vk::UniqueSemaphore		render_finished{};
		vk::UniqueFence			inflight_fence{};

		PresentationSignals() = default;
		PresentationSignals(PresentationSignals&& rhs) noexcept;

		PresentationSignals& operator=(PresentationSignals&& rhs) noexcept;
		
		void Init(VulkanView& view);
	};
};
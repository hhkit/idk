#pragma once
#include <vkn/ValHandler.h>
#include <vkn/VulkanResourceManager.h>
#include <vkn/ManagedVulkanObjs.h>

namespace idk::vkn
{
	struct PresentationSignals
	{
		vk::UniqueSemaphore		image_available{};
		vk::UniqueSemaphore		render_finished{};
		vk::UniqueFence			inflight_fence{};

		PresentationSignals() = default;
		PresentationSignals(PresentationSignals&& rhs);

		PresentationSignals& operator=(PresentationSignals&& rhs);
		
		void Init(VulkanView& view);
	};
};
#include "pch.h"
#include "PresentationSignals.h"
#include "vkn/VulkanView.h"

namespace idk::vkn {
	PresentationSignals::PresentationSignals(PresentationSignals&& rhs)
		:image_available{std::move(rhs.image_available)},
		render_finished{std::move(rhs.render_finished)},
		inflight_fence{std::move(rhs.inflight_fence)}
	{
	}
	PresentationSignals& PresentationSignals::operator=(PresentationSignals&& rhs)
	{
		// TODO: insert return statement here
		std::swap(image_available,rhs.image_available);
		std::swap(render_finished,rhs.render_finished);
		std::swap(inflight_fence,rhs.inflight_fence);

		return *this;
	}
	void idk::vkn::PresentationSignals::Init(VulkanView& view)
	{
			vk::SemaphoreCreateInfo info{};
			vk::FenceCreateInfo     fenceInfo{ vk::FenceCreateFlagBits::eSignaled };

			image_available = view.Device()->createSemaphoreUnique(info, nullptr, view.Dispatcher());
			render_finished = view.Device()->createSemaphoreUnique(info, nullptr, view.Dispatcher());
			inflight_fence = view.Device()->createFenceUnique(fenceInfo, nullptr, view.Dispatcher());
	}
};
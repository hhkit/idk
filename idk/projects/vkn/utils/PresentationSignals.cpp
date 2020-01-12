#include "pch.h"
#include "PresentationSignals.h"
#include "vkn/VulkanView.h"

namespace idk::vkn {
	vk::UniqueFence& PresentationSignals::inflight_fence()
	{
		return _inflight_fence;
	}
	PresentationSignals::PresentationSignals(PresentationSignals&& rhs) noexcept
		:image_available{std::move(rhs.image_available)},
		render_finished{std::move(rhs.render_finished)},
		_inflight_fence{std::move(rhs._inflight_fence)}
	{
	}
	PresentationSignals& PresentationSignals::operator=(PresentationSignals&& rhs) noexcept
	{
		// TODO: insert return statement here
		std::swap(image_available,rhs.image_available);
		std::swap(render_finished,rhs.render_finished);
		std::swap(_inflight_fence,rhs._inflight_fence);

		return *this;
	}
	void idk::vkn::PresentationSignals::Init(VulkanView& view)
	{
			vk::SemaphoreCreateInfo info{};
			vk::FenceCreateInfo     fenceInfo{ vk::FenceCreateFlagBits::eSignaled };

			image_available = view.Device()->createSemaphoreUnique(info, nullptr, view.Dispatcher());
			blit_finished = view.Device()->createSemaphoreUnique(info, nullptr, view.Dispatcher());
			render_finished = view.Device()->createSemaphoreUnique(info, nullptr, view.Dispatcher());
			_inflight_fence = view.Device()->createFenceUnique(fenceInfo, nullptr, view.Dispatcher());
	}
};
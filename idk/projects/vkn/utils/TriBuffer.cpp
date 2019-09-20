#include "pch.h"
#include "PresentationSignals.h"
#include "vkn/VulkanView.h"
#include "vkn/utils/SwapchainInfo.h"

#include "TriBuffer.h"

namespace idk::vkn {
	TriBuffer::TriBuffer(VulkanView& view)
		:images{ }, image_views{}, pSignals{}
	{
		pSignals.resize(view.MaxFrameInFlight());
		for (auto& elem : pSignals)
			elem.Init(view);
	}
	TriBuffer::TriBuffer(vector<vk::Image> img, vector<vk::UniqueImageView> imgView, VulkanView& view)
		:images{ img }, image_views{ std::move(imgView) }, pSignals{}
	{
		pSignals.resize(view.MaxFrameInFlight());
		for(auto& elem: pSignals)
			elem.Init(view);
	}
	TriBuffer::TriBuffer(TriBuffer&& rhs)
		:images{ std::move(rhs.images) }, image_views{ std::move(rhs.image_views) }, pSignals{ std::move(rhs.pSignals) }
	{
	}
	TriBuffer& TriBuffer::operator=(TriBuffer&& rhs)
	{
		// TODO: insert return statement here
		std::swap(images,rhs.images);
		std::swap(image_views,rhs.image_views);
		std::swap(pSignals,rhs.pSignals);

		return *this;
	}
	TriBuffer::~TriBuffer()
	{
		images.clear();
		image_views.clear();
		pSignals.clear();
	}
	void TriBuffer::CreateImageViewWithCurrImgs(VulkanView& view)
	{
		image_views.clear();
		//for (size_t i = 0; i < images.size(); ++i)
		for (auto& image : images)
		{
			vk::ImageViewCreateInfo createInfo{
				vk::ImageViewCreateFlags{},
				image,
				vk::ImageViewType::e2D,
				view.Swapchain().surface_format.format,
				vk::ComponentMapping{},
				vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor,0,1,0,1 }
			};
			image_views.emplace_back(view.Device()->createImageViewUnique(createInfo, nullptr, view.Dispatcher()));
		}
	}
	void TriBuffer::CreatePresentationSignals(VulkanView& view)
	{
		pSignals.resize(view.MaxFrameInFlight());
		for (auto& elem : pSignals)
			elem.Init(view);
	}
};
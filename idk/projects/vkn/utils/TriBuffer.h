#pragma once
#include <vkn/ValHandler.h>
#include <vkn/VulkanResourceManager.h>
#include <vkn/ManagedVulkanObjs.h>
#include <vkn/utils/PresentationSignals.h>
#include <vkn/MemoryAllocator.h>

namespace idk::vkn
{

	struct TriBuffer {
		vector<hlp::UniqueAlloc     > allocs{};
		vector<vk::UniqueImageView  > image_views{};
		vector<PresentationSignals>	  pSignals{};
		bool						  enabled{true};

		hlp::MemoryAllocator          mems;

		TriBuffer() = default;
		TriBuffer(VulkanView& view, const bool& createImage = false);
		//TriBuffer(vector<vk::Image> img, vector<vk::UniqueImageView> imgView, vector<hlp::MemoryAllocator>,VulkanView& view);

		TriBuffer(TriBuffer&& rhs) noexcept;
		TriBuffer& operator=(TriBuffer&& rhs) noexcept;
		~TriBuffer();

		void CreateImageViewWithCurrImgs(VulkanView& view);
		void CreatePresentationSignals(VulkanView&  view);

		void CreateImagePool(VulkanView& view);
		const vector<vk::Image            >& Images()const;
		void Images(vector<vk::Image            >&& imgs);
	private:
		vector<vk::Image            > images{};

	};
};
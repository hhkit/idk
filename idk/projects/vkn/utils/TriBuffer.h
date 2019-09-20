#pragma once
#include <vkn/ValHandler.h>
#include <vkn/VulkanResourceManager.h>
#include <vkn/ManagedVulkanObjs.h>
#include <vkn/utils/PresentationSignals.h>

namespace idk::vkn
{
	struct TriBuffer {
		vector<vk::Image            > images{};
		vector<vk::UniqueImageView  > image_views{};
		vector<PresentationSignals>	  pSignals{};
		bool						  enabled{true};

		TriBuffer() = default;
		TriBuffer(VulkanView& view);
		TriBuffer(vector<vk::Image> img, vector<vk::UniqueImageView> imgView, VulkanView& view);

		TriBuffer(TriBuffer&& rhs);
		TriBuffer& operator=(TriBuffer&& rhs);
		~TriBuffer();

		void CreateImageViewWithCurrImgs(VulkanView& view);
		void CreatePresentationSignals(VulkanView&  view);

	};
};
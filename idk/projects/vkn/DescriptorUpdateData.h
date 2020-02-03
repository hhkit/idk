#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <forward_list>
namespace idk::vkn
{

	struct DescriptorUpdateData
	{
		vector<vk::WriteDescriptorSet> descriptorWrite;
		std::forward_list<vector<vk::DescriptorImageInfo>> image_infos;
		std::forward_list<vk::DescriptorBufferInfo> buffer_infos;

		vector<vk::WriteDescriptorSet> scratch_descriptorWrite;
		vector<vector<vk::DescriptorImageInfo>> scratch_image_info;
		std::forward_list<vk::DescriptorBufferInfo> scratch_buffer_infos;
		void AbsorbFromScratch();
		void SendUpdates();
		void Reset();
	};
}
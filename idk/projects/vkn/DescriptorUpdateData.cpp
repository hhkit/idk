#include "pch.h"
#include "DescriptorUpdateData.h"
#include <vkn/VulkanView.h>
namespace idk::vkn
{

void DescriptorUpdateData::AbsorbFromScratch()
{
	auto& dsw = scratch_descriptorWrite;
	descriptorWrite.insert(descriptorWrite.end(), dsw.begin(), dsw.end());
	dsw.clear();
	for (auto& img_info : scratch_image_info)
	{
		image_infos.emplace_front(std::move(img_info));
	}
	scratch_image_info.clear();

	//buffer_infos.reserve(buffer_infos.size() + scratch_buffer_info.size());
	buffer_infos.splice_after(buffer_infos.before_begin(), std::move(scratch_buffer_infos));
	scratch_buffer_infos.clear();
}
VulkanView& View();
void DescriptorUpdateData::SendUpdates()
{
	auto& device = *View().Device();
	device.updateDescriptorSets(descriptorWrite, nullptr, vk::DispatchLoaderDefault{});
}

}
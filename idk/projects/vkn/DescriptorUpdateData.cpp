#include "pch.h"
#include "DescriptorUpdateData.h"
#include <vkn/VulkanView.h>
namespace idk::vkn
{
	void* DescriptorUpdateData::derp(void* ptr)
	{
		//while (true);
		return ptr;
	}
	void DescriptorUpdateData::JustBreak()
	{
	//	while (true);
	}
	void DescriptorUpdateData::AbsorbFromScratch()
{
	auto& dsw = scratch_descriptorWrite;
	descriptorWrite.insert(descriptorWrite.end(), dsw.begin(), dsw.end());
	for (auto& img_info : scratch_image_info)
	{
		image_infos.emplace_front(std::move(img_info));
	}

	//buffer_infos.reserve(buffer_infos.size() + scratch_buffer_info.size());
	buffer_infos.splice_after(buffer_infos.before_begin(), std::move(scratch_buffer_infos));
	dsw.clear();
	scratch_image_info.clear();
	scratch_buffer_infos.clear();
}
VulkanView& View();
void DescriptorUpdateData::SendUpdates()
{
	auto& device = *View().Device();
	device.updateDescriptorSets(descriptorWrite, nullptr, vk::DispatchLoaderDefault{});
}

void DescriptorUpdateData::Reset()
{
	descriptorWrite.clear();
	image_infos.clear();
	buffer_infos.clear();
}

static bool some_value = false;
static bool some_toggle = false;
bool some_func()
{
	bool value = some_value;
	some_value = some_toggle;
	return value;
}

}
namespace idk
{
	void free_block(arena_block_free* ptr) 
	{
		while (true);
		ptr->~arena_block_free(); 
	}
}
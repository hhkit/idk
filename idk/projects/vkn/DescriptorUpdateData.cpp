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
	void DescriptorUpdateData::ConvertBufferIdxToPointer()
	{
		for (auto& ds_write : descriptorWrite)
		{
			if (ds_write.pBufferInfo)
			{
				auto idx = reinterpret_cast<size_t>(ds_write.pBufferInfo)-1;
				assert(idx < buffer_infos.data());//catch when Convert was called twice.
				ds_write.pBufferInfo = buffer_infos.data()+idx;
			}
		}
	}
VulkanView& View();
void DescriptorUpdateData::SendUpdates()
{
	auto& device = *View().Device();
	ConvertBufferIdxToPointer();
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
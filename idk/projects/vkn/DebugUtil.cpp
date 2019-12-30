#include "pch.h"
#include "DebugUtil.h"
#include <vkn/VulkanView.h>
namespace idk::vkn
{
	VulkanView& View();
}
namespace idk::vkn::dbg
{

	auto CreateLabel(const char* label, const color& col)
	{
		return vk::DebugUtilsLabelEXT
		{
			label,*r_cast<const std::array<float,4>*>(col.data())
		};
	}

	void AddLabel(vk::CommandBuffer cmd_buffer, const char* label, const color& color)
	{
		auto dbg_label = CreateLabel(label, color);
		if (View().DynDispatcher().vkQueueInsertDebugUtilsLabelEXT)
			cmd_buffer.insertDebugUtilsLabelEXT(dbg_label, View().DynDispatcher());
	}

	void BeginLabel(vk::CommandBuffer cmd_buffer, const char* label, const color& color)
	{
		auto dbg_label = CreateLabel(label, color);
		if (View().DynDispatcher().vkQueueBeginDebugUtilsLabelEXT)
			cmd_buffer.beginDebugUtilsLabelEXT(dbg_label, View().DynDispatcher());
	}

	void EndLabel(vk::CommandBuffer cmd_buffer)
	{
		if (View().DynDispatcher().vkQueueEndDebugUtilsLabelEXT)
			cmd_buffer.endDebugUtilsLabelEXT(View().DynDispatcher());
	}

}
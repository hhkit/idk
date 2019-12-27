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
		cmd_buffer.insertDebugUtilsLabelEXT(dbg_label, View().DynDispatcher());
	}

	void BeginLabel(vk::CommandBuffer cmd_buffer, const char* label, const color& color)
	{
		auto dbg_label = CreateLabel(label, color);
#pragma message("YO HC I ADDED THIS. PLEASE REMOVE IF YOU'RE OKAY WITH THIS LINE.")
		if (View().DynDispatcher().vkQueueBeginDebugUtilsLabelEXT)
			cmd_buffer.beginDebugUtilsLabelEXT(dbg_label, View().DynDispatcher());
	}

	void EndLabel(vk::CommandBuffer cmd_buffer)
	{
#pragma message("I ADDED THIS TOO")
		if (View().DynDispatcher().vkQueueEndDebugUtilsLabelEXT)
			cmd_buffer.endDebugUtilsLabelEXT(View().DynDispatcher());
	}

}
#include <pch.h>
#include <vkn/RenderStateV2.h>
namespace idk::vkn
{
	void RenderStateV2::Reset() {
		cmd_buffer.reset({});
		ubo_manager.Clear();
		dpools.Reset();
		has_commands = false;
	}
}
#include <pch.h>
#include <vkn/RenderStateV2.h>
namespace idk::vkn
{
	void RenderStateV2::Reset() {
		cmd_buffer.reset({});
		ubo_manager.Clear();
		dpools.Reset();
		skyboxRenderer.ResetRsc();
		has_commands = false;
	}
}
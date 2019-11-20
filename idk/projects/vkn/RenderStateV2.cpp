#include <pch.h>
#include <vkn/RenderStateV2.h>
namespace idk::vkn
{
	const vk::CommandBuffer& RenderStateV2::CommandBuffer() const
	{
		return *cmd_buffer;
	}
	void RenderStateV2::Reset() {
		CommandBuffer().reset({});
		ubo_manager.Clear();
		dpools.Reset();
		skyboxRenderer.ResetRsc();
		has_commands = false;
	}
}
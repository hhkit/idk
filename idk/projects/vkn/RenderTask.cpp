#include "pch.h"
#include "RenderTask.h"

namespace idk::vkn
{
	void RenderTask::SetRenderPass(RenderPassObj render_pass)
	{
		curr_rp = render_pass;
	}
	void RenderTask::SetFrameBuffer(const Framebuffer& fb)
	{
		curr_frame_buffer = fb;
	}
}
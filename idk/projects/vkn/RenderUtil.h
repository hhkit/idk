#pragma once
#include <vkn/VulkanPipeline.h>
#include <gfx/GraphicsSystem.h>
#include <gfx/FakeMat4.h>
namespace idk::vkn
{
	VulkanView& View();
	void SetViewport(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, uvec2 vp_size);
	void SetScissor(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, uvec2 vp_size);


	string PrepareLightBlock(const CameraData& cam, const vector<LightData>& lights);
	string PrepareLightBlock(const mat4 view, const vector<LightData>& lights);
}
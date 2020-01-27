#include "pch.h"
#include "RenderTask.h"

#include <vkn/ShaderModule.h>

namespace idk::vkn
{
	void RenderTask::BindUniform(string_view name, uint32_t index, string_view data,bool skip_if_bound)
	{
		_uniform_manager.BindUniformBuffer(name, index, data,skip_if_bound);
	}
	void RenderTask::BindUniform(string_view name, uint32_t index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		_uniform_manager.BindSampler(name, index, texture, skip_if_bound,layout);
	}
	void RenderTask::BindShader(ShaderStage stage,RscHandle<ShaderProgram> shader_handle)
	{
		UnbindShader(stage);
		auto& shader = shader_handle.as<ShaderModule>();
		for (auto itr = shader.LayoutsBegin(), end = shader.LayoutsEnd(); itr != end; ++itr)
		{
			_uniform_manager.AddBinding(itr->first,*itr->second.layout,itr->second.entry_counts);
		}
		for (auto itr = shader.InfoBegin(), end = shader.InfoEnd(); itr != end; ++itr)
		{
			auto [name, info] = *itr;
			_uniform_manager.RegisterUniforms(name, info.set, info.binding, info.size);
		}
		_current_batch.shaders.shaders[static_cast<size_t>(stage)] = shader_handle;
	}
	void RenderTask::UnbindShader(ShaderStage stage)
	{
		auto& oshader = _current_batch.shaders.shaders[static_cast<size_t>(stage)];
		if (oshader)
		{
			auto& shader = oshader->as<ShaderModule>();

			for (auto itr = shader.LayoutsBegin(), end = shader.LayoutsEnd(); itr != end; ++itr)
			{
				_uniform_manager.RemoveBinding(itr->first);
			}
			oshader.reset();
		}
	}
	void RenderTask::SetRenderPass(RenderPassObj render_pass)
	{
		curr_rp = render_pass;
	}
	void RenderTask::SetFrameBuffer(const Framebuffer& fb)
	{
		curr_frame_buffer = fb;
	}
}
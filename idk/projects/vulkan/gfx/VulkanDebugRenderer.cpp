#include "pch.h"
#include "VulkanDebugRenderer.h"
#include "Vulkan.h"
#include <VulkanPipeline.h>
#include <vulkan/vulkan.hpp>
namespace idk
{
	struct VulkanDebugRenderer::pimpl
	{
		idk::VulkGfxPipeline pipeline;
		vgfx::VulkanDetail detail;
		idk::VulkGfxPipeline::uniform_info uniforms;
	};


	void idk::VulkanDebugRenderer::Init(GfxSystem& system)
	{
		impl = std::make_unique<pimpl>();
		impl->detail = system.GetDetail();
		idk::VulkGfxPipeline::config_t config;
		impl->pipeline.Create(config, impl->detail);
	}

	void idk::VulkanDebugRenderer::Render()
	{
		auto& cmd_buffer = *impl->detail.CurrCommandbuffer();
		impl->pipeline.Bind(cmd_buffer, impl->detail);
		impl->pipeline.BindUniformDescriptions(cmd_buffer, impl->detail, impl->uniforms);
		impl->detail
		//Bind vtx buffers
		//Bind idx buffers
		//Draw
	}
}
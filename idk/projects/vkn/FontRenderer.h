#pragma once
#include <vkn/VknFontAtlas.h>
#include <vkn/VulkanPipeline.h>
#include <vkn/VknFrameBuffer.h>
#include <ds/circular_buffer.h>
#include <vkn/ShaderModule.h>
#include <gfx/vertex_descriptor.h>
#include <vkn/VulkanMesh.h>
#include <gfx/GraphicsSystem.h>

#include <gfx/TextMesh.h>

#include <vkn/PipelineThingy.h>
#include <gfx/FramebufferFactory.h>
#include <vkn/PipelineManager.h>
#include <gfx/FontData.h>
namespace idk::vkn
{
	class PipelineThingy;
	struct GraphicsState;
	struct RenderStateV2;

	struct FontRenderer
	{
		RenderObject font_ro;
		vector<RenderObject> font_ro_inst;
		shared_ptr<pipeline_config> font_pipeline = std::make_shared<pipeline_config>();

		renderer_attributes font_vertex_req = renderer_attributes{ {
							{ vtx::Attrib::Position, 0 }
						}
		};
		void InitConfig();

		void DrawFont(PipelineThingy& the_interface, const GraphicsState& state, RenderStateV2& rs);
	};
}
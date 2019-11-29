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
#include <ui/Canvas.h>
#include <ui/Text.h>

#include <vkn/PipelineThingy.h>
#include <gfx/FramebufferFactory.h>
#include <vkn/PipelineManager.h>
#include <gfx/FontData.h>

namespace idk::vkn
{
	class PipelineThingy;
	struct GraphicsState;
	struct RenderStateV2;
	struct PostRenderData;

	struct CanvasRenderer
	{
		vector<RenderObject> canvas_ro_inst;
		shared_ptr<pipeline_config> canvas_pipeline = std::make_shared<pipeline_config>();
		shared_ptr<pipeline_config> canvas_pipeline2 = std::make_shared<pipeline_config>();

		renderer_attributes canvas_vertex_req = renderer_attributes{ {				
							{ vtx::Attrib::Position, 0 },
							{ vtx::Attrib::UV, 1 }
						}
		};
		void InitConfig();

		void DrawCanvas(PipelineThingy& the_interface,  const PostRenderData& state, RenderStateV2& rs, const vector<UIRenderObject>& canvas_data);
	};
}
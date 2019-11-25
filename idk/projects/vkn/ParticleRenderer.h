#pragma once
#include <idk.h>
#include <gfx/RenderObject.h>

namespace idk::vkn
{
	class PipelineThingy;
	struct GraphicsState;
	struct RenderStateV2;

	struct ParticleRenderer
	{
		RenderObject particle_ro;
		vector<RenderObject> particle_ro_inst;
		shared_ptr<pipeline_config> particle_pipeline = std::make_shared<pipeline_config>();

		renderer_attributes particle_vertex_req = renderer_attributes{ {
							{ vtx::Attrib::Position, 0 },
							{ vtx::Attrib::UV, 1 },
						}
		};
		void InitConfig();

		void DrawParticles(PipelineThingy& the_interface, const GraphicsState& state, RenderStateV2& rs);
	};
}
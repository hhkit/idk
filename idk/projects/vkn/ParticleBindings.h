#pragma once
#include <vkn/RenderBindings.h>

namespace idk::vkn::bindings
{
	struct ParticleVertexBindings : RenderBindings
	{
		//const GraphicsState* _state;
		//const GraphicsState& State();
		mat4 view_trf, proj_trf;
		void SetState(const GraphicsState& vstate);
		void SetState(const CameraData& camera);

		void Bind(RenderInterface& the_interface)override;

	};
}
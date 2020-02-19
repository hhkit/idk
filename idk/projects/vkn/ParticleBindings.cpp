#include "pch.h"
#include "ParticleBindings.h"

#include <vkn/GraphicsState.h>
#include <vkn/BufferHelpers.inl>

namespace idk::vkn::bindings
{

	void ParticleVertexBindings::SetState(const GraphicsState& vstate)
	{
		auto& cam = vstate.camera;
		SetState(cam);
	}

	void ParticleVertexBindings::SetState(const CameraData& cam)
	{
		view_trf = cam.view_matrix;
		proj_trf = cam.projection_matrix;
	}

	void ParticleVertexBindings::Bind(RenderInterface& the_interface)
	{
		//map back into z: (0,1)
		mat4 projection_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		} *
			proj_trf;//map back into z: (0,1)
		mat4 block[] = { projection_trf,view_trf };
		the_interface.BindUniform("CameraBlock", 0, hlp::to_data(block));
	}
}
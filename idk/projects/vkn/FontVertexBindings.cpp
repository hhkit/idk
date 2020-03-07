#include "pch.h"
#include "FontVertexBindings.h"
#include <vkn/GraphicsState.h>
#include <vkn/VknFontAtlas.h>
#include <res/ResourceHandle.inl>
#include <vkn/BufferHelpers.inl>
namespace idk::vkn::bindings
{
#pragma optimize("",off)
	void FontVertexBindings::SetState(const GraphicsState& vstate)
	{
		auto& cam = vstate.camera;
		SetState(cam);
	}

	void FontVertexBindings::SetState(const CameraData& cam)
	{
		view_trf = cam.view_matrix;
		proj_trf = cam.projection_matrix;
	}

	void FontVertexBindings::Bind(RenderInterface& the_interface)
	{
		//map back into z: (0,1)
		mat4 projection_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		}*proj_trf;//map back into z: (0,1)
		mat4 block[] = { projection_trf,view_trf };
		the_interface.BindUniform("CameraBlock", 0, hlp::to_data(block));
	}

	void FontVertexBindings::BindFont(RenderInterface& the_interface, const FontRenderData& dc)
	{
		mat4 obj_trfm = view_trf * dc.transform;
		mat4 obj_ivt = obj_trfm.inverse().transpose();
		mat4 block2[] = { obj_trfm,obj_ivt };
		the_interface.BindUniform("ObjectMat4Block", 0, hlp::to_data(block2));
		vec4 block3[] = { dc.color.as_vec4 };
		the_interface.BindUniform("FontBlock", 0, hlp::to_data(block3));
		the_interface.BindUniform("tex", 0, *dc.atlas.as<VknFontAtlas>().texture);
	}
}
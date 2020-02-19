#include "pch.h"
#include "CanvasVertexBindings.h"
#include <vkn/BufferHelpers.inl>
#include <vkn/VknFontAtlas.h>
#include <res/ResourceHandle.inl>
namespace idk::vkn::bindings
{
	void CanvasVertexBindings::SetState(const CameraData& cam)
	{
		view_trf = cam.view_matrix;
		proj_trf = cam.projection_matrix;
	}

	void CanvasVertexBindings::Bind(RenderInterface& the_interface)
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

	void CanvasVertexBindings::BindCanvas(RenderInterface& the_interface, const TextData& dc, const UIRenderObject& dc_one)
	{
		mat4 obj_trfm = view_trf * dc_one.transform;
		mat4 obj_ivt = obj_trfm.inverse().transpose();
		mat4 block2[] = { obj_trfm,obj_ivt };
		the_interface.BindUniform("ObjectMat4Block", 0, hlp::to_data(block2));
		UIBlockInfo block3[] = { { dc_one.color.as_vec4, 1 } };
		the_interface.BindUniform("UIBlock", 0, hlp::to_data(block3));
		the_interface.BindUniform("tex", 0, *dc.atlas.as<VknFontAtlas>().texture);
	}

	void CanvasVertexBindings::BindCanvas(RenderInterface& the_interface, const ImageData& dc, const UIRenderObject& dc_one)
	{
		mat4 obj_trfm = view_trf * dc_one.transform;
		mat4 obj_ivt = obj_trfm.inverse().transpose();
		mat4 block2[] = { obj_trfm,obj_ivt };
		the_interface.BindUniform("ObjectMat4Block", 0, hlp::to_data(block2));
		UIBlockInfo block3[] = { { dc_one.color.as_vec4, 0 } };
		the_interface.BindUniform("UIBlock", 0, hlp::to_data(block3));
		the_interface.BindUniform("tex", 0, dc.texture.as<VknTexture>());
	}

}
#pragma once
#include <vkn/RenderBindings.h>
namespace idk::vkn::bindings
{
	struct CanvasVertexBindings : RenderBindings
	{
		//const GraphicsState* _state;
		//const GraphicsState& State();
		mat4 view_trf, proj_trf, obj_trf;
		vec4 color;
		//void SetState(const PostRenderData& vstate);
		void SetState(const CameraData& camera);

		void Bind(RenderInterface& the_interface)override;
		void BindCanvas(RenderInterface& the_interface, const TextData& dc, const UIRenderObject& dc_one)override;
		void BindCanvas(RenderInterface& the_interface, const ImageData& dc, const UIRenderObject& dc_one)override;
	};

}
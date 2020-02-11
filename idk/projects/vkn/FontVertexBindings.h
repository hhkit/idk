#pragma once
#include <vkn/RenderBindings.h>
namespace idk::vkn::bindings
{
	struct FontVertexBindings : RenderBindings
	{
		//const GraphicsState* _state;
		//const GraphicsState& State();
		mat4 view_trf, proj_trf, obj_trf;
		vec4 color;
		void SetState(const GraphicsState& vstate);
		void SetState(const CameraData& camera);

		void Bind(RenderInterface& the_interface)override;
		void BindFont(RenderInterface& the_interface, const FontRenderData& dc)override;


	};

}
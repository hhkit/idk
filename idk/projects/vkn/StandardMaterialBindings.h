#pragma once
#include <vkn/RenderBindings.h>

namespace idk::vkn::bindings
{
	struct StandardMaterialFragBindings : RenderBindings
	{
		//Assumes that the material is valid.
		void Bind(RenderInterface& the_interface, const  RenderObject& dc)override;

	};
	struct StandardMaterialBindings : RenderBindings
	{
		const GraphicsState* _state;
		const GraphicsState& State() { return *_state; }
		RscHandle<MaterialInstance> prev_material_inst{};
		void SetState(const GraphicsState& vstate);

		//Assumes that the material is valid.
		void Bind(RenderInterface& the_interface, const RenderObject& dc) override;

		void BindAni(RenderInterface& the_interface, const AnimatedRenderObject& dc)override;

	};

}
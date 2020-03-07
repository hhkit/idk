#pragma once

#include <vkn/RenderBindings.h>

namespace idk::vkn::bindings
{
	struct FragmentShaderBinding : RenderBindings
	{
		RscHandle<ShaderProgram> fragment_shader;
		//Assumes that the material is valid.
		void Bind(RenderInterface& the_interface)override;
	};
}
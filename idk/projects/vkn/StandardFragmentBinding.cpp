#include "pch.h"
#include "StandardFragmentBinding.h"

namespace idk::vkn::bindings
{
	void FragmentShaderBinding::Bind(RenderInterface& the_interface)
	{
		the_interface.BindShader(ShaderStage::Fragment, fragment_shader);
	}
}

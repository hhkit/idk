#pragma once
#include <vkn/RenderBindings.h>

namespace idk::vkn::bindings
{
	class CameraViewportBindings : public RenderBindings
	{
	public:
		rect viewport;
		void Bind(RenderInterface& the_interface) override
		{
			the_interface.SetViewport(viewport);
			the_interface.SetScissors(viewport);
		}
	};
	namespace defaults
	{
	}
}
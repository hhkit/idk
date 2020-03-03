#include "pch.h"
#include "CameraViewportBindings.h"

namespace idk::vkn::bindings
{
	void CameraViewportBindings::Bind(RenderInterface& the_interface)
	{
		the_interface.SetViewport(viewport);
		the_interface.SetScissors(viewport);
	}
	void CameraViewportBindings::SetState(const CameraData& cam) noexcept
	{
		viewport = cam.viewport;
	}

}
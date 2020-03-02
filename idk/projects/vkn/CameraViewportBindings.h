#pragma once
#include <vkn/RenderBindings.h>

namespace idk::vkn::bindings
{
	class CameraViewportBindings : public RenderBindings
	{
	public:
		rect viewport;
		void Bind(RenderInterface& the_interface) override;
		void SetState(const CameraData& cam) noexcept;
	};
	namespace defaults
	{
	}
}
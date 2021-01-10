#include "pch.h"
#include "RenderBindings.inl"


namespace idk::vkn::bindings
{
	void RenderBindings::Bind(RenderInterface&) {}

	void RenderBindings::Bind(RenderInterface&, const RenderObject&) {}

	void RenderBindings::PrepareBindRange(RenderInterface& the_interface, strided_span<const RenderObject> dc, span<const size_t> processable_indices)
	{
	}

	void RenderBindings::BindRange(RenderInterface& the_interface, strided_span<const AnimatedRenderObject> dc)
	{
	}

	void RenderBindings::Bind(RenderInterface& the_interface, const AnimatedRenderObject& dc)
	{
		Bind(the_interface, s_cast<const RenderObject&>(dc));
		BindAni(the_interface, dc);
	}

	void RenderBindings::BindAni(RenderInterface&, const AnimatedRenderObject&) {}

	void RenderBindings::BindAniRange(RenderInterface& the_interface, strided_span<const AnimatedRenderObject> dc)
	{
	}

	void RenderBindings::BindFont(RenderInterface&, const FontRenderData&) {}

	void RenderBindings::BindCanvas([[maybe_unused]] RenderInterface& the_interface, [[maybe_unused]] const TextData& dc, [[maybe_unused]] const UIRenderObject& dc_one) {}
	void RenderBindings::BindCanvas([[maybe_unused]] RenderInterface& the_interface, [[maybe_unused]] const ImageData& dc, [[maybe_unused]] const UIRenderObject& dc_one) {}

}
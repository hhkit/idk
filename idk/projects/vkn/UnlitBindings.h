#pragma once
#include <vkn/RenderBindings.h>
#include <vkn/StandardMaterialBindings.h>
#include <vkn/StandardVertexBindings.h>
namespace idk::vkn::bindings
{
	struct UnlitFilter :RenderBindings
	{
		bool Skip(RenderInterface& the_interface, const  RenderObject& dc) override;
	};
	using UnlitMaterialBinding = CombinedBindings<UnlitFilter, StandardVertexBindings, StandardMaterialFragBindings, StandardMaterialBindings>;
}
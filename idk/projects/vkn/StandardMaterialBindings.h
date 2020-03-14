#pragma once
#include <vkn/RenderBindings.h>

#include <vkn/GraphicsState.h>

#include <vkn/MaterialInstanceCache.h>

namespace idk::vkn::bindings
{
	struct StandardMaterialFragBindings : RenderBindings
	{
		//Assumes that the material is valid.
		void Bind(RenderInterface& the_interface, const  RenderObject& dc)override;

	};
	struct StandardMaterialBindings : RenderBindings
	{
		using mat_inst_lookup_t = hash_table<RscHandle<MaterialInstance>, ProcessedMaterial>;

		RscHandle<MaterialInstance> prev_material_inst{};
		const MaterialInstanceCache* p_cached_mat_insts;
		const mat_inst_lookup_t* p_material_instances;
		//Assumes that the material is valid.
		void Bind(RenderInterface& the_interface, const RenderObject& dc) override;

		void BindAni(RenderInterface& the_interface, const AnimatedRenderObject& dc)override;

	};

}
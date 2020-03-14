#include "pch.h"
#include "StandardMaterialBindings.h"

#include <vkn/GraphicsState.h>
#include <vkn/BufferHelpers.inl>
#include <res/ResourceHandle.inl>

#include <vkn/MaterialInstanceCache.h>

namespace idk::vkn::bindings
{
	void StandardMaterialFragBindings::Bind(RenderInterface& the_interface, const RenderObject& dc)
	{
		auto& mat_inst = *dc.material_instance;
		auto& mat = *mat_inst.material;
		the_interface.BindShader(ShaderStage::Fragment, mat._shader_program);
	}
	//// 
		//Assumes that the material is valid.
	void StandardMaterialBindings::Bind(RenderInterface& the_interface, const RenderObject& dc)
	{
		//Bind the material uniforms
		{
			if (dc.material_instance != prev_material_inst)
			{
				prev_material_inst = dc.material_instance;
				auto descriptors =p_cached_mat_insts->GetDescriptorSets(dc.material_instance);
				for (auto& [set,dsl, ds] : descriptors)
				{
					the_interface.BindDescriptorSet(set, ds, dsl);
				}
				/*
				auto mat = material_instances.find(dc.material_instance);
				auto& mat_inst = mat->second;
				//[[maybe_unused]]auto& mat = *mat_inst.material;
				for (auto itr = mat_inst.ubo_table.begin(); itr != mat_inst.ubo_table.end(); ++itr)
				{
					the_interface.BindUniform(itr->first, 0, hlp::to_data(itr->second));
				}
				for (auto& [name, tex_array] : mat_inst.tex_table)
				{
					uint32_t i = 0;
					for (auto& img : tex_array)
					{
						the_interface.BindUniform(name, i++, img.as<VknTexture>());
					}
				}
				*/
			}
		}
	}

	void StandardMaterialBindings::BindAni(RenderInterface&, const AnimatedRenderObject&)
	{
	}
}
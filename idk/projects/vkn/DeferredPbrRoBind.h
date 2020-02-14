#pragma once

#include <vkn/PbrFwdBindings.h>
#include <vkn/CameraViewportBindings.h>

#include <core/Core.inl>
#include <gfx/GraphicsSystem.h>

namespace idk::vkn::bindings
{
	//class DeferredPbrRoBind: public 
	//{
	//public:
	//private:
	//};

	struct TypeCheck : RenderBindings
	{
		BlendMode blend;
		ShadingModel model;
		bool Skip([[maybe_unused]] RenderInterface& the_interface, const  RenderObject& dc)  override
		{
			if (dc.material_instance)
			{
				auto& mat_inst = *dc.material_instance;
				if (mat_inst.material)
				{
					auto& mat = *mat_inst.material;
					return mat.model != model || mat.blend != blend;
				}
			}
			return true;
		}
	};
	using DeferredPbrRoBind  = CombinedBindings<TypeCheck, CameraViewportBindings,VertexShaderBinding, StandardVertexBindings, StandardMaterialFragBindings, StandardMaterialBindings>;
	using DeferredPbrAniBind = CombinedBindings<TypeCheck, CameraViewportBindings,VertexShaderBinding, StandardVertexBindings, StandardMaterialFragBindings, StandardMaterialBindings>;

	struct DeferredPbrInfo
	{
		rect viewport;
		BlendMode blend; 
		ShadingModel model; 
		StandardMaterialBindings::mat_inst_lookup_t& material_instances;
		StandardVertexBindings::StateInfo vertex_state_info;
	};

	namespace detail
	{

		DeferredPbrRoBind make_deferred_pbr_bind(DeferredPbrInfo info)
		{
			DeferredPbrRoBind result{};
			result.Get<CameraViewportBindings>().viewport = info.viewport;
			auto& type_filter = result.Get<TypeCheck>();
			type_filter.blend = info.blend;
			type_filter.model = info.model;
			result.Get<StandardMaterialBindings>().p_material_instances = &info.material_instances;
			result.Get<StandardVertexBindings>().state = info.vertex_state_info;
			return result;
		}
	}

	DeferredPbrRoBind make_deferred_pbr_ro_bind(DeferredPbrInfo info)
	{
		DeferredPbrRoBind result = detail::make_deferred_pbr_bind(info);
		auto& shader = result.Get<VertexShaderBinding>();
		shader.vertex_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VertexShaders::VNormalMesh];
		return result;
	}
	DeferredPbrAniBind make_deferred_pbr_ani_bind(DeferredPbrInfo info)
	{
		DeferredPbrAniBind result = detail::make_deferred_pbr_bind(info);
		auto& shader = result.Get<VertexShaderBinding>();
		shader.vertex_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VertexShaders::VSkinnedMesh];
		return result;
	}

}
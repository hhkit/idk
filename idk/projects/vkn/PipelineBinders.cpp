#include "pch.h"
#include "PipelineBinders.inl"
#include <vkn/PipelineThingy.h>
#include <gfx/GraphicsSystem.h>
#include <vkn/GraphicsState.h>

#include <vkn/VknTexture.h>
#include <vkn/VknRenderTarget.h>
#include <vkn/ShaderModule.h>
#include <gfx/RenderObject.h>
#include <gfx/MaterialInstance.h>

#include <vkn/VknCubemap.h>

#include <vkn/utils/utils.inl>

namespace idk::vkn
{
	void BindCameraStuff(PipelineThingy& the_interface, const CameraData& cam)
	{
		//map back into z: (0,1)
		mat4 projection_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		}*cam.projection_matrix;//map back into z: (0,1)
		the_interface.BindUniformBuffer("CameraBlock", 0, projection_trf);
	}

	string PrepareLightBlock(const CameraData& cam, const vector<LightData>& lights)
	{
		vector<BaseLightData> tmp_light(lights.size());
		for (size_t i = 0; i < tmp_light.size(); ++i)
		{
			auto& light = tmp_light[i] = (lights)[i];
			light.v_pos = cam.view_matrix * vec4{ light.v_pos,1 };
			light.v_dir = (cam.view_matrix * vec4{ light.v_dir,0 }).get_normalized();
		}

		string light_block;
		uint32_t len = s_cast<uint32_t>(tmp_light.size());
		light_block += string{ reinterpret_cast<const char*>(&len),sizeof(len) };
		light_block += string(16 - sizeof(len), '\0');
		light_block += string{ reinterpret_cast<const char*>(tmp_light.data()), hlp::buffer_size(tmp_light) };
		return light_block;
	}


	void StandardBindings::Bind(PipelineThingy& ) {}

	void StandardBindings::Bind(PipelineThingy& , const RenderObject& ) {}

	void StandardBindings::Bind(PipelineThingy& the_interface, const AnimatedRenderObject& dc)
	{
		Bind(the_interface, s_cast<const RenderObject&>(dc));
		BindAni(the_interface, dc);
	}

	void StandardBindings::BindAni(PipelineThingy& , const AnimatedRenderObject& ) {}

	//const GraphicsState& StandardVertexBindings::State() { return *_state; }

	void StandardVertexBindings::SetState(const GraphicsState& vstate) {
		auto& cam = vstate.camera;
		SetState(cam,vstate.GetSkeletonTransforms());
	}

	void StandardVertexBindings::SetState(const CameraData& cam, const vector<SkeletonTransforms>& skel)
	{
		view_trf = cam.view_matrix;
		proj_trf = cam.projection_matrix;
		skeletons = &skel;
	}

	void StandardVertexBindings::Bind(PipelineThingy& the_interface)
	{
		//map back into z: (0,1)
		mat4 projection_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		}*proj_trf;//map back into z: (0,1)
		the_interface.BindUniformBuffer("CameraBlock", 0, projection_trf);
	}

	void StandardVertexBindings::Bind(PipelineThingy& the_interface, const RenderObject& dc)
	{
		mat4 obj_trf = view_trf * dc.transform;
		mat4 obj_ivt = obj_trf.inverse().transpose();
		vector<mat4> mat4_block{ obj_trf,obj_ivt };
		the_interface.BindUniformBuffer("ObjectMat4Block", 0, mat4_block);
	}

	void StandardVertexBindings::Bind(PipelineThingy& the_interface, const AnimatedRenderObject& dc)
	{
		Bind(the_interface, s_cast<const RenderObject&>(dc));
		BindAni(the_interface, dc);
	}

	void StandardVertexBindings::BindAni(PipelineThingy& the_interface, const AnimatedRenderObject& dc)
	{
		//auto& state = State();
		the_interface.BindUniformBuffer("BoneMat4Block", 0, (*skeletons)[dc.skeleton_index].bones_transforms);
	}

	const GraphicsState& PbrFwdBindings::State() { return *_state; }

	void PbrFwdBindings::SetState(const GraphicsState& vstate) {
		_state = &vstate;
		auto& state = State();
		cam = state.camera;
		light_block = PrepareLightBlock(cam, *state.lights);
		view_trf = cam.view_matrix;
		pbr_trf = view_trf.inverse();
		LoadStuff(vstate);
	}

	template<typename T>
	span<const T> get_span(const std::vector<T>& vec)
	{
		return span<const T>{vec.data(),vec.data()+vec.size()};
	}

	void PbrFwdBindings::LoadStuff(const GraphicsState& vstate)
	{
		ResetCubeMaps();
		pbr_cube_map_names[PbrCubeMapVarsInfo::map< PbrCubeMapVars::eIrradiance>()] = "irradiance_probe";
		pbr_cube_map_names[PbrCubeMapVarsInfo::map< PbrCubeMapVars::eEnvironmentProbe>()] = "environment_probe";


		vector<RscHandle<CubeMap>> cube_maps[PbrCubeMapVarsInfo::size()] = {};
		//TODO: actually bind something.
		cube_maps[PbrCubeMapVarsInfo::map< PbrCubeMapVars::eIrradiance>()] = {
			(vstate.camera.clear_data.index() == meta::IndexOf<CameraData::ClearData_t,RscHandle<CubeMap>>::value)
			? RscHandle<CubeMap>{std::get<RscHandle<CubeMap>>(vstate.camera.clear_data).as<VknCubemap>().GetConvoluted() } : RscHandle <CubeMap>{}
		};
		cube_maps[PbrCubeMapVarsInfo::map< PbrCubeMapVars::eEnvironmentProbe>()] = {
			(vstate.camera.clear_data.index() == meta::IndexOf<CameraData::ClearData_t,RscHandle<CubeMap>>::value)
			? std::get<RscHandle<CubeMap>>(vstate.camera.clear_data) : RscHandle < CubeMap>{}
		};
		AddCubeMaps(PbrCubeMapVars::eIrradiance, get_span(cube_maps[PbrCubeMapVarsInfo::map(PbrCubeMapVars::eIrradiance)]));
		AddCubeMaps(PbrCubeMapVars::eEnvironmentProbe, get_span(cube_maps[PbrCubeMapVarsInfo::map(PbrCubeMapVars::eEnvironmentProbe)]));
	}

	void PbrFwdBindings::ResetCubeMaps(size_t reserve_size)
	{
		pbr_cube_maps.clear();
		pbr_cube_maps.reserve(reserve_size);
		pbr_cube_maps_ranges.clear();
		pbr_cube_maps_ranges.resize(PbrCubeMapVarsInfo::size(), { 0,0 });
	}

	void PbrFwdBindings::AddCubeMaps(PbrCubeMapVars var, span<const RscHandle<CubeMap>> Cube_maps)
	{
		size_t start = pbr_cube_maps.size();
		size_t end = pbr_cube_maps.size() + Cube_maps.size();
		pbr_cube_maps.insert(pbr_cube_maps.end(), Cube_maps.begin(), Cube_maps.end());
		pbr_cube_maps_ranges[PbrCubeMapVarsInfo::map(var)] = std::make_pair(start, end);
	}

	span<const RscHandle<CubeMap>> PbrFwdBindings::GetCubeMap(PbrCubeMapVars var) const
	{
		auto [start,end] = pbr_cube_maps_ranges[PbrCubeMapVarsInfo::map(var)];
		span<const RscHandle<CubeMap>> result{ std::data(pbr_cube_maps)+start,std::data(pbr_cube_maps)+end};
		return result;
	}


	void PbrFwdBindings::ResetTexVars(size_t reserve_size)
	{
		pbr_texs.clear();
		pbr_texs.reserve(reserve_size);
		pbr_texs_ranges.clear();
		pbr_texs_ranges.resize(PbrTexVarsInfo::size(), { 0,0 });
	}

	void PbrFwdBindings::AddTexVars(PbrTexVars var, span<const RscHandle<Texture>> tex_vars)
	{
		size_t start = pbr_texs.size();
		size_t end = pbr_texs.size() + tex_vars.size();
		pbr_texs.insert(pbr_texs.end(), tex_vars.begin(), tex_vars.end());
		pbr_texs_ranges[PbrTexVarsInfo::map(var)] = std::make_pair(start, end);
	}

	span<const RscHandle<Texture>> PbrFwdBindings::GetTexVars(PbrTexVars var) const
	{
		auto [start, end] = pbr_texs_ranges[PbrTexVarsInfo::map(var)];
		span<const RscHandle<Texture>> result{ std::data(pbr_texs) + start,std::data(pbr_texs) + end };
		return result;
	}


	void PbrFwdBindings::Bind(PipelineThingy& the_interface, const RenderObject& )
	{
		auto& state = State();
		the_interface.BindUniformBuffer("LightBlock", 0, light_block,true);//skip if pbr is already bound(not per instance)
		the_interface.BindUniformBuffer("PBRBlock", 0, pbr_trf, true);//skip if pbr is already bound(not per instance)

		{
			uint32_t i = 0;
			if (state.shadow_maps_2d.size() == 0)
			{
				//Make sure that it's there.
				auto& tex = RscHandle<Texture>{}.as<VknTexture>();
				the_interface.BindSampler("shadow_maps", 0, tex,true);
			}
			else
			{
				//Bind the shadow maps
				for (auto& shadow_map : state.shadow_maps_2d)
				{
					auto& sm_uni = shadow_map;
					{
						auto& depth_tex = sm_uni.as<VknTexture>();
						the_interface.BindSampler("shadow_maps", i++, depth_tex, true);
					}
				}
			}

		}

		for (size_t i = 0; i < std::size(pbr_cube_map_names); ++i)
		{
			uint32_t index = 0;
			auto [start, end] = pbr_cube_maps_ranges[i];
			auto cm_span = span<const RscHandle<CubeMap>>{ pbr_cube_maps.data()+start,pbr_cube_maps.data() + end};
			for (auto& cm : cm_span)
				the_interface.BindSampler(pbr_cube_map_names[i], index++, cm.as<VknCubemap>());
		}

		//TODO change to cubemap stuff
		//Bind the shadow cube maps
		//for (auto& shadow_map : state.shadow_maps_cube)
		//{
		//	auto& sm_uni = shadow_map;
		//	{
		//		auto& depth_tex = sm_uni.as<VknTexture>();
		//		the_interface.BindSampler("shadow_cube_maps", i++, depth_tex);
		//	}
		//}
	}


	//Assumes that the material is valid.

	void StandardMaterialFragBindings::Bind(PipelineThingy& the_interface, const RenderObject& dc)
	{
		auto& mat_inst = *dc.material_instance;
		auto& mat = *mat_inst.material;
		the_interface.BindShader(ShaderStage::Fragment, mat._shader_program);
	}

	void StandardMaterialBindings::SetState(const GraphicsState& vstate) {
		_state = &vstate;
		State();
	}

	//Assumes that the material is valid.

	void StandardMaterialBindings::Bind(PipelineThingy& the_interface, const RenderObject& dc)
	{
		//Bind the material uniforms
		{
			
			auto& mat_inst = _state->material_instances.find(dc.material_instance)->second;
			//[[maybe_unused]]auto& mat = *mat_inst.material;
			for (auto itr = mat_inst.ubo_table.begin(); itr != mat_inst.ubo_table.end(); ++itr)
			{
				the_interface.BindUniformBuffer(itr->first, 0, itr->second);
			}
			for (auto& [name, tex_array] : mat_inst.tex_table)
			{
				uint32_t i = 0;
				for (auto& img : tex_array)
				{
					the_interface.BindSampler(name, i++, img.as<VknTexture>());
				}
			}
		}
	}

	void StandardMaterialBindings::BindAni(PipelineThingy& , const AnimatedRenderObject& )
	{
	}

}
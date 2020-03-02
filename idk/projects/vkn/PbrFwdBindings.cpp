#include "pch.h"
#include "PbrFwdBindings.h"
#include <vkn/BufferHelpers.inl>
#include <vkn/GraphicsState.h>

#include <res/ResourceHandle.inl>
#include <vkn/VknCubemap.h>
#include <ds/index_span.inl>

namespace idk::vkn::bindings
{

	void PbrFwdBindings::LoadStuff(const GraphicsState& vstate)
	{
		ResetCubeMaps();
		pbr_cube_map_names[PbrCubeMapVarsInfo::map< PbrCubeMapVars::eIrradiance>()] = "irradiance_probe";
		pbr_cube_map_names[PbrCubeMapVarsInfo::map< PbrCubeMapVars::eEnvironmentProbe>()] = "environment_probe";


		vector<RscHandle<CubeMap>> cube_maps[PbrCubeMapVarsInfo::size()] = {};
		//TODO: actually bind something.
		cube_maps[PbrCubeMapVarsInfo::map< PbrCubeMapVars::eIrradiance>()] = {
			(vstate.camera.clear_data.index() == meta::IndexOf<CameraClear,RscHandle<CubeMap>>::value)
			? RscHandle<CubeMap>{std::get<RscHandle<CubeMap>>(vstate.camera.clear_data).as<VknCubemap>().GetConvoluted() } : RscHandle <CubeMap>{}
		};
		cube_maps[PbrCubeMapVarsInfo::map< PbrCubeMapVars::eEnvironmentProbe>()] = {
			(vstate.camera.clear_data.index() == meta::IndexOf<CameraClear,RscHandle<CubeMap>>::value)
			? std::get<RscHandle<CubeMap>>(vstate.camera.clear_data) : RscHandle < CubeMap>{}
		};
		AddCubeMaps(PbrCubeMapVars::eIrradiance, span{ cube_maps[PbrCubeMapVarsInfo::map(PbrCubeMapVars::eIrradiance)] });
		AddCubeMaps(PbrCubeMapVars::eEnvironmentProbe, span{ cube_maps[PbrCubeMapVarsInfo::map(PbrCubeMapVars::eEnvironmentProbe)] });
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
		auto [start, end] = pbr_cube_maps_ranges[PbrCubeMapVarsInfo::map(var)];
		span<const RscHandle<CubeMap>> result{ std::data(pbr_cube_maps) + start,std::data(pbr_cube_maps) + end };
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

#pragma optimize("",off)
	void PbrFwdBindings::Bind(RenderInterface& the_interface, const RenderObject&)
	{
		the_interface.BindUniform("LightBlock", 0, hlp::to_data(light_block), !rebind_light);//skip if pbr is already bound(not per instance)
		the_interface.BindUniform("PBRBlock", 0, hlp::to_data(pbr_trf), true);//skip if pbr is already bound(not per instance)

		{
			uint32_t i = 0;
			if (shadow_maps.size() == 0)
			{
				//Make sure that it's there.
				auto& tex = RscHandle<Texture>{}.as<VknTexture>();
				the_interface.BindUniform("shadow_maps", 0, tex, true);
			}
			else
			{
				//Bind the shadow maps
				for (auto& shadow_map : shadow_maps)
				{
					//auto& sm_uni = shadow_map;
					{
						auto& depth_tex = shadow_map.as<VknTexture>();
						the_interface.BindUniform("shadow_maps", i++, depth_tex, true);
					}
				}
			}
			i = 0;
			if (shadow_maps_directional.size() == 0)
			{
				//Make sure that it's there.
				auto& tex = RscHandle<Texture>{}.as<VknTexture>();
				the_interface.BindUniform("shadow_maps_directional", 0, tex, true);
			}
			else
			{
				//Bind the shadow maps for directional lights
				the_interface.BindUniform("DirectionalBlock", 0, hlp::to_data(dlight_block), true);
				for (auto& shadow_map : shadow_maps_directional)
				{
					auto& sm_uni = shadow_map;
					{
						auto& depth_tex = sm_uni.as<VknTexture>();
						the_interface.BindUniform("shadow_map_directional", i++, depth_tex, true);

					}
				}
			}

		}

		for (size_t i = 0; i < std::size(pbr_cube_map_names); ++i)
		{
			uint32_t index = 0;
			auto [start, end] = pbr_cube_maps_ranges[i];
			auto cm_span = span<const RscHandle<CubeMap>>{ pbr_cube_maps.data() + start,pbr_cube_maps.data() + end };
			for (auto& cm : cm_span)
				the_interface.BindUniform(pbr_cube_map_names[i], index++, cm.as<VknCubemap>());
		}

		//TODO change to cubemap stuff
		//Bind the shadow cube maps
		//for (auto& shadow_map : state.shadow_maps_cube)
		//{
		//	auto& sm_uni = shadow_map;
		//	{
		//		auto& depth_tex = sm_uni.as<VknTexture>();
		//		the_interface.BindUniform("shadow_cube_maps", i++, depth_tex);
		//	}
		//}
	}
}
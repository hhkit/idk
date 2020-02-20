#pragma once
#include <vkn/RenderBindings.h>
#include <vkn/StandardVertexBindings.h>
#include <vkn/StandardMaterialBindings.h>

namespace idk::vkn::bindings
{
	struct PbrFwdBindings : RenderBindings
	{
		const GraphicsState* _state;
		CameraData cam;
		const GraphicsState& State();
		string light_block;
		string dlight_block;
		mat4 view_trf, pbr_trf, proj_trf;
		bool rebind_light = false;

		std::optional<std::pair<size_t, size_t>> light_range;

		vector<RscHandle<Texture>> shadow_maps;
		vector <RscHandle<Texture>> shadow_maps_directional;
		//vector<mat4> directional_vp;

		string                     pbr_cube_map_names[PbrCubeMapVarsInfo::size()];
		vector<RscHandle<CubeMap>> pbr_cube_maps;
		vector<RscHandle<Texture>> pbr_texs;

		vector<std::pair<size_t, size_t>> pbr_cube_maps_ranges;
		vector<std::pair<size_t, size_t>> pbr_texs_ranges;

		void LoadStuff(const GraphicsState& vstate);

		void ResetCubeMaps(size_t reserve_size = 4);
		void AddCubeMaps(PbrCubeMapVars var, span<const RscHandle<CubeMap>> Cube_maps);
		span<const RscHandle<CubeMap>> GetCubeMap(PbrCubeMapVars var)const;

		void ResetTexVars(size_t reserve_size = 4);
		void AddTexVars(PbrTexVars var, span<const RscHandle<Texture>> Env_maps);
		span<const RscHandle<Texture>> GetTexVars(PbrTexVars var)const;


		void SetState(const GraphicsState& vstate);


		void Bind(RenderInterface& the_interface, const RenderObject& dc)override;
	};

	using PbrFwdMaterialBinding = CombinedBindings<StandardVertexBindings, StandardMaterialFragBindings, PbrFwdBindings, StandardMaterialBindings>;
}
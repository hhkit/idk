#include "pch.h"
#include "GraphicsState.h"
#include <gfx/RenderTarget.h>
namespace idk::vkn
{
	const LightData* GraphicsState::ActiveLight(size_t light_index) const
	{
		return &shared_gfx_state->Lights()[light_index];
	}
	//template<typename tex_t>
	//std::optional<tex_t> GetShadow(const SharedGraphicsState* shared_gfx_state, size_t light_index)
	//{
	//	std::optional<tex_t> result;
	//	auto& shadows = shared_gfx_state->ShadowMaps();
	//	auto& shadow = shadows[light_index];
	//	if (shadow.index() == meta::IndexOf<shadow_map_t, tex_t>::value)
	//	{
	//		result = std::get<tex_t>(shadow);
	//	}
	//	return result;
	//}
	//std::optional<RscHandle<Texture>> GraphicsState::Shadow2D(size_t light_index) const
	//{
	//	return GetShadow< RscHandle<Texture>>(shared_gfx_state, light_index);
	//}
	//std::optional<RscHandle<CubeMap>> GraphicsState::ShadowCube(size_t light_index) const
	//{
	//	return GetShadow< RscHandle<CubeMap>>(shared_gfx_state, light_index);
	//}
	void GraphicsState::Init(const CameraData& data, const vector<LightData>& lights_data, const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects, const vector<SkeletonTransforms>& s_transforms)
{
	camera = data;
	lights = &lights_data;
	mesh_render.clear();
	skinned_mesh_render.clear();
	{
		size_t i = 0;
		RscHandle<Texture> def_2d;
		RscHandle<CubeMap> def_cube;

		for (auto& light : lights_data)
		{
			active_lights.emplace_back(i);
			if (light.index == 0)//point
			{
				//shadow_maps_2d  .emplace_back(def_2d);
				//shadow_maps_cube.emplace_back(light.light_map->GetDepthBuffer());
			}
			else
			{
				shadow_maps_2d.emplace_back(light.light_map->GetDepthBuffer());
				shadow_maps_cube.emplace_back(def_cube);
			}
			++i;
		}
	}
	skeleton_transforms = &s_transforms;
	CullAndAdd(render_objects, skinned_render_objects);
}

void GraphicsState::CullAndAdd(const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects)
{
	//TODO actuall cull
	mesh_render.reserve(render_objects.size() + mesh_render.size());
	for (auto& ro : render_objects)
	{
		mesh_render.emplace_back(&ro);
	}
	skinned_mesh_render.reserve(skinned_render_objects.size() + skinned_mesh_render.size());
	for (auto& ro : skinned_render_objects)
	{
		skinned_mesh_render.emplace_back(&ro);
	}
}

void SharedGraphicsState::Init(const vector<LightData>& light_data)
{
	lights = &light_data;
	//shadow_maps.resize(light_data.size());
}

const vector<LightData>& SharedGraphicsState::Lights() const
{ 
	return *lights; 
}
/*
vector<shadow_map_t>& SharedGraphicsState::ShadowMaps()
{
	return shadow_maps;
}

const vector<shadow_map_t>& SharedGraphicsState::ShadowMaps() const
{
	return shadow_maps;
}
*/
void PreRenderData::Init(const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects, const vector<SkeletonTransforms>& s_transforms)
{
	mesh_render.clear();
	skinned_mesh_render.clear();

	mesh_render.reserve(render_objects.size());
	for (auto& ro : render_objects)
	{
		mesh_render.emplace_back(&ro);
	}
	skinned_mesh_render.reserve(skinned_render_objects.size() );
	for (auto& ro : skinned_render_objects)
	{
		skinned_mesh_render.emplace_back(&ro);
	}
	skeleton_transforms = &s_transforms;
}

}
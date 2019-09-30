#include "pch.h"
#include "GraphicsState.h"
namespace idk::vkn
{
void GraphicsState::Init(const CameraData& data, const vector<LightData>& lights_data, const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects)
{
	camera = data;
	lights = &lights_data;
	mesh_render.clear();
	skinned_mesh_render.clear();
	for (auto& light : lights_data)
	{
		active_lights.emplace_back(&light);
	}
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

}
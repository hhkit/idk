#pragma once
#include <gfx/GraphicsSystem.h>
#include <gfx/Light.h>

namespace idk::vkn
{
	struct GraphicsState
	{
		CameraData camera;
		const vector<LightData>* lights;
		vector<const RenderObject*> mesh_render;
		vector<const RenderObject*> skinned_mesh_render;
		void Init(const CameraData& data, const vector<LightData>& lights, const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects);
		void CullAndAdd(const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects);
	};
}
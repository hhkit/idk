#pragma once
#include <gfx/GraphicsSystem.h>

namespace idk::vkn
{
	struct GraphicsState
	{
		CameraData camera;
		vector<const RenderObject*> mesh_render;
		vector<const RenderObject*> skinned_mesh_render;
		void Init(const CameraData& data, const vector<RenderObject>& render_objects, const vector<RenderObject>& skinned_render_objects)
		{
			camera = data;
			mesh_render.clear();
			skinned_mesh_render.clear();
			CullAndAdd(render_objects, skinned_render_objects);
		}
		void CullAndAdd(const vector<RenderObject>& render_objects, const vector<RenderObject>& skinned_render_objects)
		{
			//TODO actuall cull
			mesh_render.reserve(render_objects.size()+mesh_render.size());
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
	};
}
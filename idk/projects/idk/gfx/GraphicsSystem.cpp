#include "stdafx.h"
#include "GraphicsSystem.h"
#include <core/GameObject.h>
#include <gfx/MeshRenderer.h>
#include <anim/Animator.h>
#include <anim/SkinnedMeshRenderer.h>
#include <gfx/RenderObject.h>

#include <gfx/CameraControls.h>

namespace idk
{
	void GraphicsSystem::PrepareLights(span<Light> lights)
	{
		for (auto& light : lights)
		{
			light.InitShadowMap();
		}
	}
	void GraphicsSystem::BufferedLightData(vector<LightData>& out)
	{
		out = object_buffer[curr_draw_buffer].lights;
	}
	void GraphicsSystem::RenderObjData(vector<RenderObject>& out)
	{
		out = object_buffer[curr_draw_buffer].mesh_render;
	}
	void GraphicsSystem::AnimatedRenderObjData(vector<AnimatedRenderObject>& out)
	{
		out = object_buffer[curr_draw_buffer].skinned_mesh_render;
	}
	void GraphicsSystem::BufferGraphicsState(
		span<MeshRenderer> mesh_renderers,
		span<Animator> animators,
		span<SkinnedMeshRenderer> skinned_mesh_renderers,
		span<const class Transform>, 
		span<const Camera> cameras, 
		span<const Light> lights)
	{
		if (!mesh_render_config)
		{
			mesh_render_config = std::make_shared<pipeline_config>();
			mesh_render_config->fill_type = FillType::eFill;
			mesh_render_config->prim_top = PrimitiveTopology::eTriangleList;
		}
		// todo: scenegraph traversal
		RenderBuffer result{};
		result.camera.reserve(cameras.size());

		// memcpy the lights until there is a smarter implementation
		result.lights.reserve(lights.size());
		for (auto& elem : lights)
		{
			result.light_camera_data.emplace_back(elem.GenerateCameraData());//Add the camera needed for the shadowmap
			result.lights.emplace_back(elem.GenerateLightData());
		}

		hash_table<Handle<Animator>, unsigned> skeleton_indices;
		unsigned i{};
		for (auto& elem : animators)
		{
			skeleton_indices.emplace(elem.GetHandle(), i++);
			result.skeleton_transforms.emplace_back(
				SkeletonTransforms{ std::move(elem.BoneTransforms()) } // generate this from the skeletons
				//SkeletonTransforms{ vector<mat4>{3} }
			);
		}

		for (auto& elem : skinned_mesh_renderers)
		{
			AnimatedRenderObject ro = elem.GenerateRenderObject();
			// @Joseph: GET PARENT IN THE FUTURE WHEN EACH MESH GO HAS ITS OWN SKINNED MESH RENDERER
			const auto parent = elem.GetGameObject()->Parent();
			const auto animator = parent->GetComponent<Animator>();
			ro.skeleton_index = skeleton_indices[animator];
			ro.config = mesh_render_config;
			result.skinned_mesh_render.emplace_back(std::move(ro));
		}

		for (auto& camera : cameras)
		{
			if(camera.is_scene_camera)
				result.curr_scene_camera = camera.GenerateCameraData();
			result.camera.emplace_back(camera.GenerateCameraData());
		}

		for (auto& elem : mesh_renderers)
			if (elem.IsActiveAndEnabled())
				result.mesh_render.emplace_back(elem.GenerateRenderObject()).config = mesh_render_config;

		
		SubmitBuffers(std::move(result));
	}

	void GraphicsSystem::SwapWritingBuffer()
	{
		//write_buffer_dirty = true;
		//tmp
		curr_draw_buffer= curr_write_buffer;
	}

	void GraphicsSystem::SubmitBuffers(RenderBuffer&& buffer)
	{
		object_buffer[curr_write_buffer] = std::move(buffer);
		SwapWritingBuffer();
	}

	//GraphicsSystem::TempLight::TempLight(const Light& l, GraphicsSystem& sys) :light{ l }
	//{
	//	auto init_map = [](auto& light) {
	//		return light.InitShadowMap();
	//	};
	//	auto get_id = [](auto& light) {
	//		return light.unique_id();
	//	};
	//	;
	//	auto id = std::visit(get_id, l.light);
	//	auto& pool = sys.shadow_map_pool[id];
	//	auto& next_index = sys.shadow_map_pool_index[id];
	//	if (pool.size() <= next_index)
	//	{
	//		pool.emplace_back(std::visit(init_map, l.light));
	//	}
	//	shadow_map = pool[next_index++];
	//}

	//CameraData GraphicsSystem::TempLight::GenerateCameraData()
	//{
	//	auto data =light.GenerateCameraData();
	//	data.render_target = shadow_map;
	//	return data;
	//}
	//
	//LightData GraphicsSystem::TempLight::GenerateLightData()
	//{
	//	auto data = light.GenerateLightData();
	//	data.light_map = shadow_map;
	//	return data;
	//}

}
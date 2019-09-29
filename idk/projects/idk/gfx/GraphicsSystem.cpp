#include "stdafx.h"
#include "GraphicsSystem.h"
#include <core/GameObject.h>
#include <gfx/MeshRenderer.h>
#include <anim/AnimationController.h>
#include <anim/SkinnedMeshRenderer.h>
#include <gfx/RenderObject.h>
namespace idk
{
	void GraphicsSystem::BufferGraphicsState(
		span<MeshRenderer> mesh_renderers,
		span<AnimationController> animators,
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
			result.camera.emplace_back(elem.GenerateCameraData());//Add the camera needed for the shadowmap
			result.lights.emplace_back(elem.GenerateLightData());
		}

		hash_table<Handle<AnimationController>, unsigned> skeleton_indices;
		unsigned i{};
		for (auto& elem : animators)
		{
			skeleton_indices.emplace(elem.GetHandle(), i++);
			result.skeleton_transforms.emplace_back(
				SkeletonTransforms{ std::move(elem.GenerateTransforms()) } // generate this from the skeletons
				//SkeletonTransforms{ vector<mat4>{3} }
			);
		}

		for (auto& elem : skinned_mesh_renderers)
		{
			AnimatedRenderObject ro = elem.GenerateRenderObject();
			// @Joseph: GET PARENT IN THE FUTURE WHEN EACH MESH GO HAS ITS OWN SKINNED MESH RENDERER
			auto parent = elem.GetGameObject()->Parent();
			auto animator = parent->GetComponent<AnimationController>();
			ro.skeleton_index = skeleton_indices[animator];
			result.skinned_mesh_render.emplace_back(ro);
		}

		for (auto& camera : cameras)
			result.camera.emplace_back(camera.GenerateCameraData());

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


}
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
		// todo: scenegraph traversal
		RenderBuffer result{};
		result.camera.reserve(cameras.size());

		// memcpy the lights until there is a smarter implementation
		result.lights.reserve(lights.size());
		for (auto& elem : lights)
			result.lights.emplace_back(elem.GenerateLightData());

		hash_table<Handle<AnimationController>, unsigned> skeleton_indices;
		unsigned i{};
		for (auto& elem : animators)
		{
			skeleton_indices.emplace(elem.GetHandle(), ++i);
			result.skeletons.emplace_back(
				SkeletonTransforms{} // generate this from the skeletons
			);
		}

		for (auto& elem : skinned_mesh_renderers)
		{
			AnimatedRenderObject ro;
			ro.RenderObject::operator=(elem.GenerateRenderObject());
			ro.skeleton_index = skeleton_indices[elem.GetGameObject()->Parent()->GetComponent<AnimationController>()];
		}

		for (auto& camera : cameras)
			result.camera.emplace_back(camera.GenerateCameraData());

		for (auto& elem : mesh_renderers)
			if (elem.IsActiveAndEnabled())
				result.mesh_render.emplace_back(elem.GenerateRenderObject());

		SubmitBuffers(std::move(result));
	}

	void GraphicsSystem::SwapWritingBuffer()
	{
		write_buffer_dirty = true;
	}

	void GraphicsSystem::SubmitBuffers(RenderBuffer&& buffer)
	{
		object_buffer[curr_write_buffer] = std::move(buffer);
		SwapWritingBuffer();
	}


}